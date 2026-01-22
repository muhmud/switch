#ifndef _SWITCH_LIBINPUT_MONITOR_H_
#define _SWITCH_LIBINPUT_MONITOR_H_
#include <sys/wait.h>

#define LIBINPUT_FAILED_TO_LIST_DEVICES 20
#define LIBINPUT_NO_MATCHING_DEVICES 21
#define LIBINPUT_ERROR 22

typedef int KeyHandlerLibInput(int);

int start_monitoring_mods_libinput(const char *device_name, KeyHandlerLibInput mod_press_handler,
                                   KeyHandlerLibInput mod_release_handler);

#endif // _SWITCH_LIBINPUT_MONITOR_H_
