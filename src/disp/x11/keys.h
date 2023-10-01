#ifndef _SWITCH_DISP_X11_KEYS_H_
#define _SWITCH_DISP_X11_KEYS_H_
#include <pthread.h>

#define X_SERVER_ERROR 1
#define X_EVENT_ERROR 2
#define X_INVALID_DEVICE_ERROR 3

typedef int key_handler(int);

int start_monitoring_mods_x11(
    const char* device, key_handler mod_press_handler, key_handler mod_release_handler);

#endif // _SWITCH_DISP_X11_KEYS_H_
