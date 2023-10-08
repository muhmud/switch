#include "client.h"
#include "disp/x11/keys.h"
#include "mods.h"
#include "request.h"
#include "server.h"
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SERVER_START_SLEEP_INTERVAL_MILLISECONDS 20
#define SERVER_START_TIMEOUT_MILLISECONDS 5000

int main(int argc, char *argv[]) {
  XIDeviceInfo *dev;
  struct ClientRequest client_request;
  struct ClientResponse client_response;
  int server;
  int daemonize;
  int forward;
  const char *socket_file;
  const char *request;
  const char *app;
  const char *modcode;
  const char *id;
  const char *device;
  int option;
  const char *err_msg;

  struct option long_options[] = {
      {"server", no_argument, NULL, 's'},       {"reverse", no_argument, NULL, 'b'},
      {"daemonize", no_argument, NULL, 'S'},    {"socket-file", required_argument, NULL, 'c'},
      {"device", required_argument, NULL, 'd'}, {"request", required_argument, NULL, 'r'},
      {"app", required_argument, NULL, 'a'},    {"mod", required_argument, NULL, 'm'},
      {"id", required_argument, NULL, 'i'},     {NULL, 0, NULL, 0} // End of options
  };

  server = 0;
  daemonize = 0;
  forward = 1;
  socket_file = DEFAULT_SOCKET_PATH;
  request = NULL;
  app = NULL;
  modcode = NULL;
  id = NULL;
  device = NULL;
  while ((option = getopt_long(argc, argv, "Ssc:d:ba:m:i:f", long_options, NULL)) != -1) {
    switch (option) {
    case 's':
      server = 1;
      break;
    case 'S':
      daemonize = 1;
      break;
    case 'c':
      socket_file = optarg;
      break;
    case 'd':
      device = optarg;
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
    case 'b':
      forward = 0;
      break;
    case '?':
      fprintf(stderr,
              "Usage: %s\n"
              "         [-s | --server] [-S | --daemonize] [-c | --socket-file <file>] [-d | "
              "--device <device>] |\n"
              "         [-r | --request <type>] [-c | --socket-file <file>]\n"
              "         [-a | --app <app>] [-i | --id <id>] [-m | --mod <mod>] [-b | "
              "--reverse]\n",
              argv[0]);
    default:
      exit(EXIT_FAILURE);
    }
  }
  if (server == 1) {
    if (device) {
      dev = find_device_x11(NULL, device);
      if (!dev) {
        fprintf(stderr, "invalid device\n");
        exit(EXIT_FAILURE);
      }
      XIFreeDeviceInfo(dev);
    }
    switch (start_server(socket_file, device, daemonize)) {
    case -1:
      fprintf(stderr, "failed to start server\n");
      exit(EXIT_FAILURE);
    case 0:
      exit(EXIT_SUCCESS);
    default:
      switch (wait_for_server_ready(socket_file, SERVER_START_SLEEP_INTERVAL_MILLISECONDS,
                                    SERVER_START_TIMEOUT_MILLISECONDS)) {
      case 0:
        exit(EXIT_SUCCESS);
      case TIMEOUT_ERROR:
        fprintf(stderr, "timeout occurred waiting for server to start\n");
      default:
        exit(EXIT_FAILURE);
      }
    }
    exit(EXIT_SUCCESS);
  }
  if (request) {
    memset(&client_request, 0, sizeof(struct ClientRequest));
    memset(&client_response, 0, sizeof(struct ClientResponse));
    client_request.request = convert_string_to_request(request);
    client_request.modcode = convert_string_to_modcode(modcode);
    client_request.forward = forward;
    if (app) {
      strncpy(client_request.app, app, sizeof(client_request.app));
    }
    if (id) {
      strncpy(client_request.id, id, sizeof(client_request.id));
    }
    err_msg = NULL;
    switch (send_request(socket_file, &client_request, &client_response)) {
    case 0:
      break;
    case APP_NAME_NOT_PROVIDED_ERROR:
      err_msg = "app name not provided";
      break;
    case ITEM_ID_NOT_PROVIDED_ERROR:
      err_msg = "id not provided";
      break;
    case INVALID_MOD_ERROR:
      err_msg = "invalid mod";
      break;
    case INVALID_REQUEST_ERROR:
      err_msg = "invalid request type";
      break;
    default:
      err_msg = "server error";
    }
    if (client_response.ret != 0) {
      err_msg = "server failed to process request";
    }
    if (err_msg) {
      fprintf(stderr, "request failed: %s\n", err_msg);
      exit(EXIT_FAILURE);
    }
    if (client_request.request == CLIENT_REQUEST_SWITCH ||
        client_request.request == CLIENT_REQUEST_GET_TOP) {
      fprintf(stdout, "%s\n", client_response.id);
      exit(EXIT_SUCCESS);
    }
  }
  return EXIT_FAILURE;
}
