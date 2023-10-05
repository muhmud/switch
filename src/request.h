#ifndef _SWITCH_REQUEST_H_
#define _SWITCH_REQUEST_H_
#include "app.h"
#include "stack.h"

#define CLIENT_REQUEST_SHUTDOWN 1
#define CLIENT_REQUEST_ADD_APP 2
#define CLIENT_REQUEST_DELETE_APP 3
#define CLIENT_REQUEST_ADD_STACK_ITEM 4
#define CLIENT_REQUEST_DELETE_STACK_ITEM 5
#define CLIENT_REQUEST_SWITCH 6
#define CLIENT_REQUEST_SET 7
#define CLIENT_REQUEST_GET_TOP 8

struct ClientRequest {
  int request;
  int modcode;
  int forward;
  char app[APP_NAME_SIZE];
  char id[STACK_ITEM_ID_SIZE];
};

struct ClientResponse {
  int ret;
  char id[STACK_ITEM_ID_SIZE];
};

int convert_string_to_request(const char *code);

#endif // _SWITCH_REQUEST_H_
