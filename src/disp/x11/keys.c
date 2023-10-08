#include "keys.h"
#include "../../mods.h"
#include <X11/extensions/XI2.h>
#include <X11/keysym.h>
#include <ctype.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

XIDeviceInfo *find_device_x11(Display *display, const char *device) {
  XIDeviceInfo *dev;
  int close_display;
  int num_devices;
  char *endptr;
  long int deviceid;

  deviceid = XIAllDevices;
  if (device) {
    deviceid = strtoimax(device, &endptr, 10);
    if (*endptr != '\0') {
      return NULL;
    }
  }
  close_display = 0;
  if (!display) {
    display = XOpenDisplay(NULL);
    if (!display) {
      return NULL;
    }
    close_display = 1;
  }
  dev = XIQueryDevice(display, deviceid, &num_devices);
  if (close_display) {
    XCloseDisplay(display);
  }
  return dev;
}

int start_monitoring_mods_x11(const char *device, KeyHandlerX11 mod_press_handler,
                              KeyHandlerX11 mod_release_handler) {
  Display *display;
  XEvent xev;
  XIDeviceInfo *dev;
  int opcode, event, error;
  XIEventMask evmask;
  unsigned char mask[(XI_LASTEVENT + 7) / 8] = {0};
  XIDeviceEvent *xiev;
  KeySym keysym;
  int modcode;
  int ret;

  display = XOpenDisplay(NULL);
  if (!display) {
    return X_SERVER_ERROR;
  }
  dev = NULL;
  if (!XQueryExtension(display, "XInputExtension", &opcode, &event, &error)) {
    ret = X_INPUT_EXTENSION_ERROR;
    goto fail;
  }
  dev = find_device_x11(display, device);
  if (!dev) {
    ret = X_INVALID_DEVICE_ERROR;
    goto fail;
  }
  evmask.deviceid = device ? dev->deviceid : XIAllDevices;
  evmask.mask_len = sizeof(mask);
  evmask.mask = mask;
  XISetMask(mask, XI_KeyPress);
  XISetMask(mask, XI_KeyRelease);
  XISelectEvents(display, DefaultRootWindow(display), &evmask, 1);
  while (1) {
    XNextEvent(display, &xev);
    if (xev.xcookie.type == GenericEvent && xev.xcookie.extension == opcode) {
      XGetEventData(display, &xev.xcookie);
      if (xev.xcookie.evtype == XI_KeyPress || xev.xcookie.evtype == XI_KeyRelease) {
        xiev = (XIDeviceEvent *)xev.xcookie.data;
        keysym = XkbKeycodeToKeysym(display, xiev->detail, 0, 0);
        if (is_mod_key(keysym)) {
          modcode = convert_keysym_to_modcode(keysym);
          if (xev.xcookie.evtype == XI_KeyPress) {
            mod_press_handler(modcode);
          } else {
            mod_release_handler(modcode);
          }
        }
      }
      XFreeEventData(display, &xev.xcookie);
    }
  }
fail:
  if (dev) {
    XIFreeDeviceInfo(dev);
  }
  XCloseDisplay(display);
  return ret;
}

int is_x11() {
  Display *display;

  display = XOpenDisplay(NULL);
  if (!display) {
    return 0;
  }
  XCloseDisplay(display);
  return 1;
}
