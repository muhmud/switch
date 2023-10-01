#ifndef _SWITCH_SERVER_H_
#define _SWITCH_SERVER_H_

#define SIGNAL_HANDLER_ERROR 1
#define CLIENT_HANDLER_ERROR 2

int start_server(const char *socket_file, const char* device);

#endif // _SWITCH_SERVER_H_
