#ifndef _SWITCH_CLIENT_H_
#define _SWITCH_CLIENT_H_

#define DEFAULT_SOCKET_PATH "/tmp/switch"
struct ClientRequest;
struct ClientResponse;

#define APP_NAME_NOT_PROVIDED_ERROR 1
#define ITEM_ID_NOT_PROVIDED_ERROR 2
#define INVALID_MOD_ERROR 3
#define INVALID_REQUEST_ERROR 4
#define SOCKET_ERROR 5
#define SOCKET_CONNECT_ERROR 6
#define SOCKET_SEND_ERROR 7
#define SOCKET_RECEIVE_ERROR 8

int send_request(const char *socket_file, struct ClientRequest *request, struct ClientResponse* response);

#endif // _SWITCH_CLIENT_H_
