#include "server.h"
#include "app.h"
#include "app_keymap.h"
#include "disp/x11/keys.h"
#include "request.h"
#include <fcntl.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <time.h>
#include <unistd.h>

int server_fd = -1;
const char *sock_file = NULL;

pthread_mutex_t client_handler_mutex;
pthread_cond_t client_handler_started;
int client_handler_running = 0;

static int mod_press_handler(int modcode) {
  struct AppNode *node;

  app_keymap_lock();
  node = find_apps_by_modcode(modcode);
  if (!node) {
    app_keymap_unlock();
    return -1;
  }
  while (node) {
    node->app->pressed = 1;
    node = node->next;
  }
  app_keymap_unlock();
  return 0;
}

static int mod_release_handler(int modcode) {
  struct AppNode *node;

  app_keymap_lock();
  node = find_apps_by_modcode(modcode);
  if (!node) {
    app_keymap_unlock();
    return -1;
  }
  while (node) {
    node->app->pressed = 0;
    select_item(node->app);
    node = node->next;
  }
  app_keymap_unlock();
  return 0;
}

static int process_client_request(struct ClientRequest *request, struct ClientResponse *response) {
  struct App *app;
  struct StackItem *item;
  int ret;

  ret = -1;
  app_keymap_lock();
  switch (request->request) {
  case CLIENT_REQUEST_SHUTDOWN:
    exit(0);
  case CLIENT_REQUEST_ADD_APP:
    ret = add_app(request->app, request->modcode);
    break;
  case CLIENT_REQUEST_DELETE_APP:
    ret = delete_app(request->app);
    break;
  case CLIENT_REQUEST_SWITCH:
    ret = switch_item(find_app(request->app), request->forward, &item);
    if (ret == 0) {
      strncpy(response->id, item->id, sizeof(response->id));
    }
    break;
  case CLIENT_REQUEST_SET:
    ret = set_item(find_app(request->app), request->id);
    break;
  case CLIENT_REQUEST_ADD_STACK_ITEM:
    ret = add_item(find_app(request->app), request->id);
    break;
  case CLIENT_REQUEST_DELETE_STACK_ITEM:
    ret = delete_item(find_app(request->app), request->id);
    break;
  case CLIENT_REQUEST_GET_TOP:
    app = find_app(request->app);
    if (app && app->top) {
      strncpy(response->id, app->top->id, sizeof(response->id));
      ret = 0;
    }
    break;
  }
  app_keymap_unlock();
  return ret;
}

static void *client_handler(void *arg) {
  int client_fd;
  struct sockaddr_un server_addr, client_addr;
  int reuse;
  socklen_t client_len;
  ssize_t byte_count;
  struct timeval timeout;
  struct ClientRequest request;
  struct ClientResponse response;

  pthread_mutex_lock(&client_handler_mutex);
  if ((server_fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
    goto fail;
  }
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sun_family = AF_UNIX;
  strncpy(server_addr.sun_path, arg, sizeof(server_addr.sun_path) - 1);
  reuse = 1;
  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int)) == -1) {
    goto fail;
  }
  if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
    goto fail;
  }
  sock_file = arg;
  if (listen(server_fd, 3) == -1) {
    goto fail;
  }
  timeout.tv_sec = 5;
  timeout.tv_usec = 0;
  if (setsockopt(server_fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) == -1) {
    goto fail;
  }
  client_handler_running = 1;
  pthread_mutex_unlock(&client_handler_mutex);
  pthread_cond_signal(&client_handler_started);
  while (1) {
    client_len = sizeof(client_addr);
    if ((client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len)) == -1) {
      continue;
    }
    byte_count = recv(client_fd, &request, sizeof(struct ClientRequest), 0);
    if (byte_count == -1) {
      close(client_fd);
      continue;
    }
    memset(&response, 0, sizeof(struct ClientResponse));
    int ret = process_client_request(&request, &response);
    response.ret = ret;
    byte_count = send(client_fd, &response, sizeof(struct ClientResponse), 0);
    close(client_fd);
  }
fail:
  client_handler_running = 0;
  pthread_mutex_unlock(&client_handler_mutex);
  pthread_cond_signal(&client_handler_started);
  return NULL;
}

static void server_cleanup(int signum) {
  if (server_fd != -1) {
    close(server_fd);
    server_fd = -1;
  }
  if (sock_file) {
    unlink(sock_file);
    sock_file = NULL;
  }
  exit(EXIT_SUCCESS);
}

static void exit_cleanup() { server_cleanup(0); }

static int server_daemonize() {
  int fd;

  switch (fork()) {
  case 0:
    break;
  case -1:
    return -1;
  default:
    return 1;
  }
  if (setsid() == -1) {
    return -1;
  }
  switch (fork()) {
  case 0:
    break;
  case -1:
    return -1;
  default:
    _exit(EXIT_SUCCESS);
  }
  umask(0);
  chdir("/");
  close(STDIN_FILENO);
  fd = open("/dev/null", O_RDWR);
  if (fd != STDIN_FILENO) {
    return -1;
  }
  if (dup2(STDIN_FILENO, STDOUT_FILENO) != STDOUT_FILENO) {
    return -1;
  }
  if (dup2(STDIN_FILENO, STDERR_FILENO) != STDERR_FILENO) {
    return -1;
  }
  return 0;
}

int start_server(const char *socket_file, const char *device, int daemonize) {
  struct sigaction sa;
  int mutex_initialized;
  int condition_initialized;
  pthread_t client_handler_thread_id;
  int ret;

  if (daemonize && (ret = server_daemonize()) != 0) {
    return ret;
  }
  sa.sa_handler = server_cleanup;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;
  if (sigaction(SIGTERM, &sa, NULL) == -1 || sigaction(SIGINT, &sa, NULL) == -1) {
    return SIGNAL_HANDLER_ERROR;
  }
  signal(SIGPIPE, SIG_IGN);
  atexit(exit_cleanup);
  ret = 0;
  mutex_initialized = 0;
  condition_initialized = 0;
  if (pthread_mutex_init(&client_handler_mutex, NULL) != 0) {
    return CLIENT_HANDLER_ERROR;
  }
  mutex_initialized = 1;
  if (pthread_cond_init(&client_handler_started, NULL) != 0) {
    ret = CLIENT_HANDLER_ERROR;
    goto quit;
  }
  condition_initialized = 1;
  pthread_mutex_lock(&client_handler_mutex);
  if (pthread_create(&client_handler_thread_id, NULL, client_handler, (void *)socket_file) != 0) {
    ret = CLIENT_HANDLER_ERROR;
    goto quit;
  }
  pthread_cond_wait(&client_handler_started, &client_handler_mutex);
  pthread_mutex_unlock(&client_handler_mutex);
  if (client_handler_running != 1) {
    ret = CLIENT_HANDLER_ERROR;
    goto quit;
  }
  start_monitoring_mods_x11(device, mod_press_handler, mod_release_handler);
quit:
  if (mutex_initialized == 1) {
    pthread_mutex_destroy(&client_handler_mutex);
  }
  if (condition_initialized == 1) {
    pthread_cond_destroy(&client_handler_started);
  }
  return ret;
}

int wait_for_server_ready(const char *socket_file, int sleep_interval_milliseconds,
                          int timeout_milliseconds) {
  int sockfd;
  int ret;
  time_t timeout_end;

  timeout_end = time(NULL) + timeout_milliseconds / 1000 + 1;
  while (1) {
    if (access(socket_file, F_OK) != -1) {
      sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
      if (sockfd == -1) {
        return SERVER_ERROR;
      }
      struct sockaddr_un server_addr;
      server_addr.sun_family = AF_UNIX;
      strncpy(server_addr.sun_path, socket_file, sizeof(server_addr.sun_path) - 1);
      ret = connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
      close(sockfd);
      if (ret == 0) {
        return 0;
      }
    }
    if (time(NULL) >= timeout_end) {
      break;
    }
    usleep((useconds_t)sleep_interval_milliseconds * 1000);
  }
  return TIMEOUT_ERROR;
}
