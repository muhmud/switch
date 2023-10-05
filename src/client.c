#include "client.h"
#include "request.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

static int dispatch_request(const char *socket_file, struct ClientRequest *request,
                            struct ClientResponse *response) {
  int client_fd;
  struct sockaddr_un server_addr;
  ssize_t byte_count;
  int ret;

  if ((client_fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
    return SOCKET_ERROR;
  }
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sun_family = AF_UNIX;
  strncpy(server_addr.sun_path, socket_file, sizeof(server_addr.sun_path) - 1);
  if (connect(client_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
    return SOCKET_CONNECT_ERROR;
  }
  memset(response, 0, sizeof(struct ClientResponse));
  byte_count = send(client_fd, request, sizeof(struct ClientRequest), 0);
  if (byte_count == -1) {
    ret = SOCKET_SEND_ERROR;
    goto quit;
  }
  byte_count = recv(client_fd, response, sizeof(struct ClientResponse), 0);
  if (byte_count == -1) {
    ret = SOCKET_RECEIVE_ERROR;
    goto quit;
  }
  ret = 0;
quit:
  close(client_fd);
  return ret;
}

int send_request(const char *socket_file, struct ClientRequest *request,
                 struct ClientResponse **response) {
  int ret;

  assert(socket_file && request);
  if (request->request != CLIENT_REQUEST_SHUTDOWN) {
    if (strlen(request->app) == 0) {
      return APP_NAME_NOT_PROVIDED_ERROR;
    }
  }
  switch (request->request) {
  case CLIENT_REQUEST_ADD_APP:
    if (request->modcode == -1) {
      return INVALID_MOD_ERROR;
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
      return ITEM_ID_NOT_PROVIDED_ERROR;
    }
    goto do_request;
  default:
    return INVALID_REQUEST_ERROR;
  }
do_request:
  ret = dispatch_request(socket_file, request, *response);
  if (ret != 0) {
    return ret;
  }
  return 0;
}
