#include <X11/XKBlib.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/XInput.h>
#include <X11/keysym.h>
#include <ctype.h>
#include <fcntl.h>
#include <getopt.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define DEFAULT_SOCKET_PATH "/tmp/switch"

#define Shift_L 0
#define Shift_R 1
#define Shift 2
#define Control_L 3
#define Control_R 4
#define Control 5
#define Meta_L 6
#define Meta_R 7
#define Meta 8
#define Alt_L 9
#define Alt_R 10
#define Alt 11
#define Super_L 12
#define Super_R 13
#define Super 14
#define Hyper_L 15
#define Hyper_R 16
#define Hyper 17

#define CLIENT_REQUEST_SHUTDOWN 0
#define CLIENT_REQUEST_ADD_APP 1
#define CLIENT_REQUEST_DELETE_APP 2
#define CLIENT_REQUEST_ADD_STACK_ITEM 3
#define CLIENT_REQUEST_DELETE_STACK_ITEM 4
#define CLIENT_REQUEST_SWITCH 5
#define CLIENT_REQUEST_SET 6
#define CLIENT_REQUEST_GET_TOP 7

int server_fd = -1;
const char *sock_file = NULL;

pthread_mutex_t client_handler_mutex;
pthread_cond_t client_handler_started;
int client_handler_running = 0;

pthread_mutex_t keymap_mutex;

struct KeySyms {
  KeySym left;
  KeySym right;
};

struct ModCodes {
  int left;
  int right;
};

struct ClientRequest {
  int request;
  int modcode;
  int forward;
  char app[64];
  char id[64];
};

struct ClientResponse {
  int ret;
  char id[64];
};

struct AppStackItem {
  char id[64];
  struct AppStackItem *prev;
  struct AppStackItem *next;
};

struct AppData {
  char app[64];
  int modcode;
  volatile int pressed;
  struct AppStackItem *switching_current;
  struct AppStackItem *top;
};

int convert_to_modcode(KeySym keysym) {
  int modcode;

  modcode = -1;
  switch (keysym) {
  case XK_Shift_L:
    modcode = Shift_L;
    break;
  case XK_Shift_R:
    modcode = Shift_R;
    break;
  case XK_Control_L:
    modcode = Control_L;
    break;
  case XK_Control_R:
    modcode = Control_R;
    break;
  case XK_Meta_L:
    modcode = Meta_L;
    break;
  case XK_Meta_R:
    modcode = Meta_R;
    break;
  case XK_Alt_L:
    modcode = Alt_L;
    break;
  case XK_Alt_R:
    modcode = Alt_R;
    break;
  case XK_Super_L:
    modcode = Super_L;
    break;
  case XK_Super_R:
    modcode = Super_R;
    break;
  case XK_Hyper_L:
    modcode = Hyper_L;
    break;
  case XK_Hyper_R:
    modcode = Hyper_R;
    break;
  }
  return modcode;
}

struct KeySyms find_keysyms(int modcode) {
  struct KeySyms keysyms = {-1, -1};

  switch (modcode) {
  case Shift_L:
    keysyms.left = XK_Shift_L;
    break;
  case Shift_R:
    keysyms.right = XK_Shift_R;
    break;
  case Shift:
    keysyms.left = XK_Shift_L;
    keysyms.right = XK_Shift_R;
    break;
  case Control_L:
    keysyms.left = XK_Control_L;
    break;
  case Control_R:
    keysyms.right = XK_Control_R;
    break;
  case Control:
    keysyms.left = XK_Control_L;
    keysyms.right = XK_Control_R;
    break;
  case Meta_L:
    keysyms.left = XK_Meta_L;
    break;
  case Meta_R:
    keysyms.right = XK_Meta_R;
    break;
  case Meta:
    keysyms.left = XK_Meta_L;
    keysyms.right = XK_Meta_R;
    break;
  case Alt_L:
    keysyms.left = XK_Alt_L;
    break;
  case Alt_R:
    keysyms.right = XK_Alt_R;
    break;
  case Alt:
    keysyms.left = XK_Alt_L;
    keysyms.right = XK_Alt_R;
    break;
  case Super_L:
    keysyms.left = XK_Super_L;
    break;
  case Super_R:
    keysyms.right = XK_Super_R;
    break;
  case Super:
    keysyms.left = XK_Super_L;
    keysyms.right = XK_Super_R;
    break;
  case Hyper_L:
    keysyms.left = XK_Hyper_L;
    break;
  case Hyper_R:
    keysyms.right = XK_Hyper_R;
    break;
  case Hyper:
    keysyms.left = XK_Hyper_L;
    keysyms.right = XK_Hyper_R;
    break;
  }
  return keysyms;
}

struct ModCodes find_modcodes(int modcode) {
  struct ModCodes modcodes = {-1, -1};

  switch (modcode) {
  case Shift_L:
    modcodes.left = Shift_L;
    break;
  case Shift_R:
    modcodes.right = Shift_R;
    break;
  case Shift:
    modcodes.left = Shift_L;
    modcodes.right = Shift_R;
    break;
  case Control_L:
    modcodes.left = Control_L;
    break;
  case Control_R:
    modcodes.right = Control_R;
    break;
  case Control:
    modcodes.left = Control_L;
    modcodes.right = Control_R;
    break;
  case Meta_L:
    modcodes.left = Meta_L;
    break;
  case Meta_R:
    modcodes.right = Meta_R;
    break;
  case Meta:
    modcodes.left = Meta_L;
    modcodes.right = Meta_R;
    break;
  case Alt_L:
    modcodes.left = Alt_L;
    break;
  case Alt_R:
    modcodes.right = Alt_R;
    break;
  case Alt:
    modcodes.left = Alt_L;
    modcodes.right = Alt_R;
    break;
  case Super_L:
    modcodes.left = Super_L;
    break;
  case Super_R:
    modcodes.right = Super_R;
    break;
  case Super:
    modcodes.left = Super_L;
    modcodes.right = Super_R;
    break;
  case Hyper_L:
    modcodes.left = Hyper_L;
    break;
  case Hyper_R:
    modcodes.right = Hyper_R;
    break;
  case Hyper:
    modcodes.left = Hyper_L;
    modcodes.right = Hyper_R;
    break;
  }
  return modcodes;
}

int modcode_from_str(const char *code) {
  int modcode;

  modcode = -1;
  if (code) {
    if (strcmp(code, "shift_l") == 0) {
      modcode = Shift_L;
    } else if (strcmp(code, "shift_r") == 0) {
      modcode = Shift_R;
    } else if (strcmp(code, "shift") == 0) {
      modcode = Shift;
    } else if (strcmp(code, "ctrl_l") == 0) {
      modcode = Control_L;
    } else if (strcmp(code, "ctrl_r") == 0) {
      modcode = Control_R;
    } else if (strcmp(code, "ctrl") == 0) {
      modcode = Control;
    } else if (strcmp(code, "meta_l") == 0) {
      modcode = Meta_L;
    } else if (strcmp(code, "meta_r") == 0) {
      modcode = Meta_R;
    } else if (strcmp(code, "meta") == 0) {
      modcode = Meta;
    } else if (strcmp(code, "alt_l") == 0) {
      modcode = Alt_L;
    } else if (strcmp(code, "alt_r") == 0) {
      modcode = Alt_R;
    } else if (strcmp(code, "alt") == 0) {
      modcode = Alt;
    } else if (strcmp(code, "super_l") == 0) {
      modcode = Super_L;
    } else if (strcmp(code, "super_r") == 0) {
      modcode = Super_R;
    } else if (strcmp(code, "Super") == 0) {
      modcode = Super;
    } else if (strcmp(code, "hyper_l") == 0) {
      modcode = Hyper_L;
    } else if (strcmp(code, "hyper_r") == 0) {
      modcode = Hyper_R;
    } else if (strcmp(code, "hyper") == 0) {
      modcode = Hyper;
    }
  }
  return modcode;
}

int request_from_str(const char *code) {
  int request;

  request = -1;
  if (code) {
    if (strcmp(code, "shutdown") == 0) {
      request = CLIENT_REQUEST_SHUTDOWN;
    } else if (strcmp(code, "add-app") == 0) {
      request = CLIENT_REQUEST_ADD_APP;
    } else if (strcmp(code, "delete-app") == 0) {
      request = CLIENT_REQUEST_DELETE_APP;
    } else if (strcmp(code, "add") == 0) {
      request = CLIENT_REQUEST_ADD_STACK_ITEM;
    } else if (strcmp(code, "delete") == 0) {
      request = CLIENT_REQUEST_DELETE_STACK_ITEM;
    } else if (strcmp(code, "switch") == 0) {
      request = CLIENT_REQUEST_SWITCH;
    } else if (strcmp(code, "set") == 0) {
      request = CLIENT_REQUEST_SET;
    } else if (strcmp(code, "top") == 0) {
      request = CLIENT_REQUEST_GET_TOP;
    }
  }
  return request;
}

struct AppData *keymap[] = {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
                            NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};

int contains_app(const char *app) {
  int i;

  for (i = 0; i < sizeof(keymap); ++i) {
    struct AppData *app_data = keymap[i];
    if (app_data != NULL && strcmp(app_data->app, app) == 0) {
      return -1;
    }
  }
  return 0;
}

int update_keymap(int modcode, struct AppData *app_data) {
  if (modcode == -1) {
    return 0;
  }
  if (keymap[modcode] == NULL) {
    keymap[modcode] = app_data;
    return 0;
  }
  return -1;
}

int add_app(struct AppData *app_data) {
  struct ModCodes modcodes;

  if (contains_app(app_data->app)) {
    return -1;
  }
  modcodes = find_modcodes(app_data->modcode);
  if (update_keymap(modcodes.left, app_data) == -1 ||
      update_keymap(modcodes.right, app_data) == -1) {
    return -1;
  }
  return 0;
}

struct AppData *find_app(const char *app) {
  int i;

  for (i = 0; i < sizeof(keymap); ++i) {
    struct AppData *app_data = keymap[i];
    if (app_data != NULL && strcmp(app_data->app, app) == 0) {
      return app_data;
    }
  }
  return NULL;
}

int delete_app(const char *app) {
  int i;

  for (i = 0; i < sizeof(keymap); ++i) {
    struct AppData *app_data = keymap[i];
    if (app_data != NULL && strcmp(app_data->app, app) == 0) {
      free(app_data);
      keymap[i] = NULL;
      return 0;
    }
  }
  return -1;
}

struct AppStackItem *new_stack_item(const char *id) {
  struct AppStackItem *item;

  item = (struct AppStackItem *)malloc(sizeof(struct AppStackItem));
  strncpy(item->id, id, sizeof(item->id));
  item->next = NULL;
  item->prev = NULL;
  return item;
}

void move_to_top_of_stack(struct AppData *app_data, struct AppStackItem *item) {
  if (app_data->top == item) {
    return;
  }
  if (!app_data->top) {
    app_data->top = item;
    return;
  }
  if (item->prev && item->next) {
    item->prev->next = item->next;
    item->next->prev = item->prev;
  }
  item->next = app_data->top;
  if (app_data->top->prev && app_data->top->next) {
    item->prev = app_data->top->prev;
    app_data->top->prev->next = item;
  } else {
    item->prev = app_data->top;
    app_data->top->next = item;
  }
  app_data->top->prev = item;
  app_data->top = item;
}

struct AppStackItem *find_stack_item(struct AppData *app_data, const char *id) {
  struct AppStackItem *current;

  current = app_data->top;
  do {
    if (current == NULL) {
      return NULL;
    }
    if (strcmp(id, current->id) == 0) {
      return current;
    }
    current = current->next;
  } while (current != app_data->top);
  return NULL;
}

int add_stack_item(const char *app, const char *id) {
  struct AppData *app_data;
  struct AppStackItem *item;

  app_data = find_app(app);
  if (!app_data) {
    return -1;
  }
  item = find_stack_item(app_data, id);
  if (item) {
    return -1;
  }
  move_to_top_of_stack(app_data, new_stack_item(id));
  return 0;
}

void select_stack_item(struct AppData *app_data) {
  if (app_data->switching_current) {
    move_to_top_of_stack(app_data, app_data->switching_current);
    app_data->switching_current = NULL;
  }
}

int set_stack_item(const char *app, const char *id) {
  struct AppData *app_data;
  struct AppStackItem *item;
  int moved_to_top;

  app_data = find_app(app);
  if (!app_data) {
    return -1;
  }
  moved_to_top = 0;
  item = find_stack_item(app_data, id);
  if (!item) {
    move_to_top_of_stack(app_data, new_stack_item(id));
    moved_to_top = 1;
  }
  if (app_data->pressed == 1) {
    app_data->switching_current = item;
  } else if (moved_to_top == 0) {
    move_to_top_of_stack(app_data, item);
  }
  return 0;
}

const char *get_top(const char *app) {
  struct AppData *app_data;

  app_data = find_app(app);
  if (!app_data) {
    return NULL;
  }
  if (app_data->top) {
    return app_data->top->id;
  }
  return NULL;
}

int switch_stack_item(const char *app, int forward, struct AppStackItem **item) {
  struct AppData *app_data;

  app_data = find_app(app);
  if (!app_data) {
    return -1;
  }
  if (!app_data->top) {
    return -1;
  }
  if (app_data->switching_current) {
    if (forward == 1) {
      if (app_data->switching_current->next) {
        app_data->switching_current = app_data->switching_current->next;
      }
    } else {
      if (app_data->switching_current->prev) {
        app_data->switching_current = app_data->switching_current->prev;
      }
    }
  } else {
    if (app_data->top) {
      if (forward == 1 && app_data->top->next) {
        app_data->switching_current = app_data->top->next;
      } else if (forward == 0 && app_data->top->prev) {
        app_data->switching_current = app_data->top->prev;
      } else {
        app_data->switching_current = app_data->top;
      }
    }
  }
  *item = app_data->switching_current;
  return 0;
}

int delete_stack_item(const char *app, const char *id) {
  struct AppData *app_data;
  struct AppStackItem *item;

  app_data = find_app(app);
  if (app_data == NULL) {
    return -1;
  }
  item = find_stack_item(app_data, id);
  if (item == NULL) {
    return -1;
  }
  if (item->next == app_data->top) {
    app_data->top->next = NULL;
    app_data->top->prev = NULL;
  } else if (item == app_data->top && app_data->top->next &&
             app_data->top->next->next == app_data->top) {
    app_data->top = app_data->top->next;
    app_data->top->next = NULL;
    app_data->top->prev = NULL;
  } else if (item->prev && item->next) {
    item->prev->next = item->next;
    item->next->prev = item->prev;
    if (item == app_data->top) {
      app_data->top = item->next;
    }
  } else {
    app_data->top = NULL;
  }
  free(item);
  return 0;
}

int is_mod_key(KeySym keysym) {
  return (keysym == XK_Shift_L || keysym == XK_Shift_R || keysym == XK_Control_L ||
          keysym == XK_Control_R || keysym == XK_Meta_L || keysym == XK_Meta_R ||
          keysym == XK_Alt_L || keysym == XK_Alt_R || keysym == XK_Super_L ||
          keysym == XK_Super_R || keysym == XK_Hyper_L || keysym == XK_Hyper_R);
}

int process_mod_release(KeySym keysym) {
  int modcode;
  struct AppData *app_data;

  modcode = convert_to_modcode(keysym);
  if (modcode == -1) {
    return -1;
  }
  pthread_mutex_lock(&keymap_mutex);
  app_data = keymap[modcode];
  if (!app_data) {
    pthread_mutex_unlock(&keymap_mutex);
    return -1;
  }
  app_data->pressed = 0;
  select_stack_item(app_data);
  pthread_mutex_unlock(&keymap_mutex);
  return 0;
}

int process_mod_press(KeySym keysym) {
  int modcode;
  struct AppData *app_data;

  modcode = convert_to_modcode(keysym);
  if (modcode == -1) {
    return -1;
  }
  pthread_mutex_lock(&keymap_mutex);
  app_data = keymap[modcode];
  if (!app_data) {
    pthread_mutex_unlock(&keymap_mutex);
    return -1;
  }
  app_data->pressed = 1;
  pthread_mutex_unlock(&keymap_mutex);
  return 0;
}

XDeviceInfo *find_device_info(Display *display, char *name, int only_extended) {
  XDeviceInfo *devices;
  XDeviceInfo *found = NULL;
  int num_devices;
  int len;
  int i;
  int is_id = 1;
  XID id = (XID)-1;

  // Determine whether or not we were provided with a device ID
  len = strlen(name);
  for (i = 0; i < len; i++) {
    if (!isdigit(name[i])) {
      is_id = 0;
      break;
    }
  }
  if (is_id) {
    id = atoi(name);
  }
  // Find the device based on the provided name or ID
  devices = XListInputDevices(display, &num_devices);
  for (i = 0; i < num_devices; i++) {
    if ((!only_extended || (devices[i].use >= IsXExtensionDevice)) &&
        ((!is_id && strcmp(devices[i].name, name) == 0) || (is_id && devices[i].id == id))) {
      if (found) {
        fprintf(stderr,
                "Warning: There are multiple devices named '%s'.\n"
                "To ensure the correct one is selected, please use "
                "the device ID instead.\n\n",
                name);
        return NULL;
      } else {
        found = &devices[i];
      }
    }
  }
  return found;
}

int process_client_request(struct ClientRequest *request, struct ClientResponse *response) {
  struct AppData *app_data;
  struct AppStackItem *item;
  const char *id;
  int ret;

  ret = -1;
  pthread_mutex_lock(&keymap_mutex);
  switch (request->request) {
  case CLIENT_REQUEST_SHUTDOWN:
    exit(0);
  case CLIENT_REQUEST_ADD_APP:
    app_data = (struct AppData *)malloc(sizeof(struct AppData));
    strncpy(app_data->app, request->app, sizeof(app_data->app));
    app_data->modcode = request->modcode;
    ret = add_app(app_data);
    break;
  case CLIENT_REQUEST_DELETE_APP:
    ret = delete_app(request->app);
    break;
  case CLIENT_REQUEST_SWITCH:
    ret = switch_stack_item(request->app, request->forward, &item);
    if (ret == 0) {
      strncpy(response->id, item->id, sizeof(response->id));
    }
    break;
  case CLIENT_REQUEST_SET:
    ret = set_stack_item(request->app, request->id);
    break;
  case CLIENT_REQUEST_ADD_STACK_ITEM:
    ret = add_stack_item(request->app, request->id);
    break;
  case CLIENT_REQUEST_DELETE_STACK_ITEM:
    ret = delete_stack_item(request->app, request->id);
    break;
  case CLIENT_REQUEST_GET_TOP:
    id = get_top(request->app);
    if (id != NULL) {
      strncpy(response->id, id, sizeof(response->id));
    }
    ret = 0;
    break;
  }
  pthread_mutex_unlock(&keymap_mutex);
  return ret;
}

void *client_handler(void *arg) {
  int client_fd;
  struct sockaddr_un server_addr, client_addr;
  int reuse;
  socklen_t client_len;
  ssize_t byte_count;
  struct timeval timeout;
  struct ClientRequest request;
  struct ClientResponse response;
  int ret;

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
    perror("bind");
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

int send_client_request(const char *socket_file, struct ClientRequest *request,
                        struct ClientResponse *response) {
  int client_fd;
  struct sockaddr_un server_addr;
  ssize_t byte_count;
  int ret;

  ret = -1;

  if ((client_fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
    fprintf(stderr, "could not create socket\n");
    return -1;
  }
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sun_family = AF_UNIX;
  strncpy(server_addr.sun_path, socket_file, sizeof(server_addr.sun_path) - 1);
  if (connect(client_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
    fprintf(stderr, "could not connect to server\n");
    return -1;
  }
  memset(response, 0, sizeof(struct ClientResponse));
  byte_count = send(client_fd, request, sizeof(struct ClientRequest), 0);
  if (byte_count == -1) {
    fprintf(stderr, "could not send request to server\n");
    goto quit;
  }
  byte_count = recv(client_fd, response, sizeof(struct ClientResponse), 0);
  if (byte_count == -1) {
    fprintf(stderr, "could not receive response from server\n");
    goto quit;
  }
  ret = 0;
quit:
  close(client_fd);
  return ret;
}

int process_request(const char *socket_file, struct ClientRequest *request) {
  struct ClientResponse response;
  int ret;

  if (!socket_file) {
    socket_file = DEFAULT_SOCKET_PATH;
  }
  if (request->request != CLIENT_REQUEST_SHUTDOWN) {
    if (strlen(request->app) == 0) {
      fprintf(stderr, "app name not provided\n");
      return -1;
    }
  }
  switch (request->request) {
  case CLIENT_REQUEST_ADD_APP:
    if (request->modcode == -1) {
      fprintf(stderr, "invalid mod\n");
      return -1;
    }
  case CLIENT_REQUEST_SHUTDOWN:
  case CLIENT_REQUEST_DELETE_APP:
  case CLIENT_REQUEST_SWITCH:
  case CLIENT_REQUEST_GET_TOP:
    goto do_request;
  case CLIENT_REQUEST_SET:
  case CLIENT_REQUEST_ADD_STACK_ITEM:
  case CLIENT_REQUEST_DELETE_STACK_ITEM:
    if (strlen(request->id) == 0) {
      fprintf(stderr, "item id not provided\n");
      return -1;
    }
    goto do_request;
  default:
    return -1;
  }
do_request:
  ret = send_client_request(socket_file, request, &response);
  if (ret != 0) {
    fprintf(stderr, "failure sending request\n");
    return ret;
  }
  if (response.ret != 0) {
    fprintf(stderr, "server error\n");
    return response.ret;
  }
  if (request->request == CLIENT_REQUEST_SWITCH || request->request == CLIENT_REQUEST_GET_TOP) {
    fprintf(stdout, "%s\n", response.id);
  }
  return 0;
}

void server_cleanup(int signum) {
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

void exit_cleanup() { server_cleanup(0); }

int start_server(const char *socket_file) {
  Display *display;
  Window root;
  XEvent event;
  XDeviceInfo *info;
  XDevice *device;
  int key_press_type;
  int key_release_type;
  XEventClass event_classes[2];
  int i;
  int event_registered;
  XInputClassInfo *ip;
  XDeviceKeyEvent *key_event;
  KeySym keysym;
  pthread_t client_handler_thread_id;
  int mutex_initialized;
  int keymap_mutex_initialized;
  int condition_initialized;
  struct sigaction sa;

  sa.sa_handler = server_cleanup;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;
  if (sigaction(SIGTERM, &sa, NULL) == -1 || sigaction(SIGINT, &sa, NULL) == -1) {
    perror("sigaction");
    return EXIT_FAILURE;
  }
  atexit(exit_cleanup);
  display = XOpenDisplay(NULL);
  if (display == NULL) {
    fprintf(stderr, "Unable to connect to X server\n");
    goto fail;
  }
  root = DefaultRootWindow(display);
  XSelectInput(display, root, KeyPressMask | KeyReleaseMask);
  info = find_device_info(display, "10", True);
  device = XOpenDevice(display, info->id);
  if (!device) {
    fprintf(stderr, "unable to open device\n");
    goto fail;
  }
  key_press_type = -1;
  key_release_type = -1;
  event_registered = 0;
  if (device->num_classes > 0) {
    for (ip = device->classes, i = 0; i < info->num_classes; ip++, i++) {
      if (ip->input_class == KeyClass) {
        DeviceKeyPress(device, key_press_type, event_classes[0]);
        DeviceKeyRelease(device, key_release_type, event_classes[1]);
        event_registered = 1;
      }
    }
    if (XSelectExtensionEvent(display, root, event_classes, 2)) {
      fprintf(stderr, "error selecting extended events\n");
      goto fail;
    }
  }
  if (event_registered == 0) {
    fprintf(stderr, "error registering events\n");
    goto fail;
  }
  keymap_mutex_initialized = 0;
  mutex_initialized = 0;
  condition_initialized = 0;
  if (pthread_mutex_init(&client_handler_mutex, NULL) != 0) {
    fprintf(stderr, "could not initialize client handler mutex\n");
    goto fail;
  }
  mutex_initialized = 1;
  if (pthread_mutex_init(&keymap_mutex, NULL) != 0) {
    fprintf(stderr, "could not initialize keymap mutex\n");
    goto fail;
  }
  keymap_mutex_initialized = 1;
  if (pthread_cond_init(&client_handler_started, NULL) != 0) {
    fprintf(stderr, "could not initialize client handler condition\n");
    goto fail;
  }
  condition_initialized = 1;
  pthread_mutex_lock(&client_handler_mutex);
  if (!socket_file) {
    socket_file = DEFAULT_SOCKET_PATH;
  }
  if (pthread_create(&client_handler_thread_id, NULL, client_handler, (void *)socket_file) != 0) {
    fprintf(stderr, "error creating client handler thread\n");
    goto fail;
  }
  pthread_cond_wait(&client_handler_started, &client_handler_mutex);
  pthread_mutex_unlock(&client_handler_mutex);
  if (client_handler_running != 1) {
    fprintf(stderr, "error starting client handler thread\n");
    goto fail;
  }
  fprintf(stderr, "running\n");
  // Check that the client handler started successfully
  setvbuf(stdout, NULL, _IOLBF, 0);
  while (1) {
    XNextEvent(display, &event);
    if (event.type == key_release_type || event.type == key_press_type) {
      key_event = (XDeviceKeyEvent *)&event;
      keysym = XkbKeycodeToKeysym(display, key_event->keycode, 0, 0);
      if (is_mod_key(keysym)) {
        if (event.type == key_release_type) {
          process_mod_release(keysym);
        } else {
          process_mod_press(keysym);
        }
      }
    }
  }
fail:
  if (display) {
    XCloseDisplay(display);
  }
  if (mutex_initialized == 1) {
    pthread_mutex_destroy(&client_handler_mutex);
  }
  if (keymap_mutex_initialized == 1) {
    pthread_mutex_destroy(&keymap_mutex);
  }
  if (condition_initialized == 1) {
    pthread_cond_destroy(&client_handler_started);
  }
  return EXIT_FAILURE;
}

int main(int argc, char *argv[]) {
  struct ClientRequest client_request;
  int server;
  int forward;
  const char *socket_file;
  const char *request;
  const char *app;
  const char *modcode;
  const char *id;
  int option;

  struct option long_options[] = {
      {"server", no_argument, NULL, 's'},
      {"forward", no_argument, NULL, 'f'},
      {"socket-file", required_argument, NULL, 'c'},
      {"request", required_argument, NULL, 'r'},
      {"app", required_argument, NULL, 'a'},
      {"mod", required_argument, NULL, 'm'},
      {"id", required_argument, NULL, 'i'},
      {NULL, 0, NULL, 0} // End of options
  };
  server = 0;
  forward = 0;
  socket_file = NULL;
  request = NULL;
  app = NULL;
  modcode = NULL;
  id = NULL;
  while ((option = getopt_long(argc, argv, "sc:r:a:m:i:f", long_options, NULL)) != -1) {
    switch (option) {
    case 's':
      server = 1;
      break;
    case 'c':
      socket_file = optarg;
      break;
    case 'r':
      request = optarg;
      break;
    case 'a':
      app = optarg;
      break;
    case 'm':
      modcode = optarg;
      break;
    case 'i':
      id = optarg;
      break;
    case 'f':
      forward = 1;
      break;
    case '?':
      fprintf(stderr,
              "Usage: %s [-s | --server] [-c | --socket-file <file>] |\n"
              "          [-r | --request <type>] [-c | --socket-file <file>]\n"
              "              [-a | --app <app>] [-i | --id <id>] [-m | --mod <mod>] [-f | "
              "--forward]\n",
              argv[0]);
      exit(EXIT_FAILURE);
    default:
      exit(EXIT_FAILURE);
    }
  }
  if (server == 1) {
    return start_server(socket_file);
  }
  if (request) {
    memset(&client_request, 0, sizeof(struct ClientRequest));
    client_request.request = request_from_str(request);
    client_request.modcode = modcode_from_str(modcode);
    client_request.forward = forward;
    if (app) {
      strncpy(client_request.app, app, sizeof(client_request.app));
    }
    if (id) {
      strncpy(client_request.id, id, sizeof(client_request.id));
    }
    return process_request(socket_file, &client_request);
  }
  return EXIT_FAILURE;
}
