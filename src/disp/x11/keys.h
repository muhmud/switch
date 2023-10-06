#ifndef _SWITCH_DISP_X11_KEYS_H_
#define _SWITCH_DISP_X11_KEYS_H_
#include <X11/XKBlib.h>
#include <X11/extensions/XInput2.h>
#include <pthread.h>

#define X_SERVER_ERROR 1
#define X_INPUT_EXTENSION_ERROR 2
#define X_EVENT_ERROR 3
#define X_INVALID_DEVICE_ERROR 4

typedef int KeyHandlerX11(int);

XIDeviceInfo* find_device_x11(Display* display, const char* device);

int start_monitoring_mods_x11(
    const char* deviceid, KeyHandlerX11 mod_press_handler, KeyHandlerX11 mod_release_handler);

int is_x11();

#endif // _SWITCH_DISP_X11_KEYS_H_
