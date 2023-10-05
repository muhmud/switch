#include "request.h"
#include <string.h>

int convert_string_to_request(const char *code) {
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
    } else if (strcmp(code, "restart-app") == 0) {
      request = CLIENT_REQUEST_RESTART_APP;
    }
  }
  return request;
}
