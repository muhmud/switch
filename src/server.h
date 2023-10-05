#ifndef _SWITCH_SERVER_H_
#define _SWITCH_SERVER_H_

#define SIGNAL_HANDLER_ERROR 1
#define CLIENT_HANDLER_ERROR 2

int start_server(const char *socket_file, const char* device, int daemonize);

#define SERVER_ERROR 1
#define TIMEOUT_ERROR 2
int wait_for_server_ready(const char* socket_file, int sleep_interval_milliseconds, int timeout_milliseconds);

#endif // _SWITCH_SERVER_H_
