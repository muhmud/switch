#include "keys.h"
#include "../../mods.h"
#include <X11/XKBlib.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/XInput.h>
#include <X11/keysym.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

static XDeviceInfo *find_device_info(Display *display, const char *name, int only_extended) {
  XDeviceInfo *devices;
  XDeviceInfo *found = NULL;
  int num_devices;
  int len;
  int i;
  int is_id = 1;
  XID id = (XID)-1;

  // Determine whether or not we were provided with a device ID
  len = strlen(name);
  for (i = 0; i < len; i++) {
    if (!isdigit(name[i])) {
      is_id = 0;
      break;
    }
  }
  if (is_id) {
    id = atoi(name);
  }
  // Find the device based on the provided name or ID
  devices = XListInputDevices(display, &num_devices);
  for (i = 0; i < num_devices; i++) {
    if ((!only_extended || (devices[i].use >= IsXExtensionDevice)) &&
        ((!is_id && strcmp(devices[i].name, name) == 0) || (is_id && devices[i].id == id))) {
      if (found) {
        return NULL;
      }
      found = &devices[i];
    }
  }
  return found;
}

int start_monitoring_mods_x11(const char *device, key_handler mod_press_handler,
                              key_handler mod_release_handler) {
  Display *display;
  Window root;
  XEvent event;
  XDeviceInfo *info;
  XDevice *dev;
  int key_press_type;
  int key_release_type;
  XEventClass event_classes[2];
  int i;
  int event_registered;
  XInputClassInfo *ip;
  XDeviceKeyEvent *key_event;
  KeySym keysym;
  int modcode;
  int ret;

  display = XOpenDisplay(NULL);
  if (!display) {
    return X_SERVER_ERROR;
  }
  root = DefaultRootWindow(display);
  XSelectInput(display, root, KeyPressMask | KeyReleaseMask);
  info = find_device_info(display, device, True);
  dev = XOpenDevice(display, info->id);
  if (!dev) {
    ret = X_INVALID_DEVICE_ERROR;
    goto fail;
  }
  key_press_type = -1;
  key_release_type = -1;
  event_registered = 0;
  if (dev->num_classes > 0) {
    for (ip = dev->classes, i = 0; i < info->num_classes; ip++, i++) {
      if (ip->input_class == KeyClass) {
        DeviceKeyPress(dev, key_press_type, event_classes[0]);
        DeviceKeyRelease(dev, key_release_type, event_classes[1]);
        event_registered = 1;
      }
    }
    if (XSelectExtensionEvent(display, root, event_classes, 2)) {
      ret = X_EVENT_ERROR;
      goto fail;
    }
  }
  if (event_registered == 0) {
    ret = X_EVENT_ERROR;
    goto fail;
  }
  while (1) {
    XNextEvent(display, &event);
    if (event.type == key_release_type || event.type == key_press_type) {
      key_event = (XDeviceKeyEvent *)&event;
      keysym = XkbKeycodeToKeysym(display, key_event->keycode, 0, 0);
      if (is_mod_key(keysym)) {
        modcode = convert_keysym_to_modcode(keysym);
        if (event.type == key_release_type) {
          mod_release_handler(modcode);
        } else {
          mod_press_handler(modcode);
        }
      }
    }
  }
fail:
  if (display) {
    XCloseDisplay(display);
  }
  return ret;
}
