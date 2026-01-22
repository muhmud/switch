#include "monitor.h"
#include "../mods.h"
#include <errno.h>
#include <fcntl.h>
#include <libinput.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_DEVICES 16
#define BUFFER_SIZE 4096

static int open_restricted(const char *path, int flags, void *user_data) {
  int fd = open(path, flags);
  if (fd < 0) {
    fprintf(stderr, "Failed to open %s: %s\n", path, strerror(errno));
  }
  return fd < 0 ? -errno : fd;
}

static void close_restricted(int fd, void *user_data) { close(fd); }

static const struct libinput_interface interface = {
    .open_restricted = open_restricted,
    .close_restricted = close_restricted,
};

// Get device paths for a given device name using libinput list-devices
int get_device_paths(const char *device_name, char **paths, int max_paths) {
  FILE *fp;
  char buffer[BUFFER_SIZE];
  int count = 0;
  int in_target_device = 0;

  // Run libinput list-devices
  fp = popen("libinput list-devices", "r");
  if (fp == NULL) {
    perror("popen");
    return -1;
  }

  // Parse output to find device paths
  while (fgets(buffer, BUFFER_SIZE, fp) != NULL && count < max_paths) {
    // Check if this line contains "Device:" followed by our target name
    if (strstr(buffer, "Device:") != NULL) {
      // Extract the device name from the line (after "Device:")
      char *name_start = strstr(buffer, "Device:");
      if (name_start != NULL) {
        name_start += 7; // Skip "Device:"
        // Skip whitespace
        while (*name_start == ' ' || *name_start == '\t') {
          name_start++;
        }
        // Remove trailing newline/whitespace
        char *name_end = name_start + strlen(name_start) - 1;
        while (name_end > name_start &&
               (*name_end == '\n' || *name_end == '\r' || *name_end == ' ' || *name_end == '\t')) {
          *name_end = '\0';
          name_end--;
        }
        // Check for exact match
        in_target_device = (strcmp(name_start, device_name) == 0);
      }
    }

    // If we're in the target device section, look for "Kernel:" line
    if (in_target_device && strstr(buffer, "Kernel:") != NULL) {
      // Extract the device path (e.g., /dev/input/event3)
      char *dev_start = strstr(buffer, "/dev/input/event");
      if (dev_start != NULL) {
        // Find the end of the device path
        char *dev_end = dev_start;
        while (*dev_end != '\0' && *dev_end != '\n' && *dev_end != ' ') {
          dev_end++;
        }

        // Allocate and copy the device path
        int len = dev_end - dev_start;
        paths[count] = malloc(len + 1);
        if (paths[count] != NULL) {
          strncpy(paths[count], dev_start, len);
          paths[count][len] = '\0';
          count++;
        }

        in_target_device = 0; // Reset for next device
      }
    }
  }

  pclose(fp);
  return count;
}

int handle_event(uint32_t key, enum libinput_key_state state, KeyHandlerLibInput mod_press_handler,
                 KeyHandlerLibInput mod_release_handler) {
  int is_released = 0;
  int modcode;

  if (state == LIBINPUT_KEY_STATE_RELEASED) {
    is_released = 1;
  }
  modcode = convert_libinput_to_modcode(key);

  if (modcode != -1) {
    if (is_released == 1) {
      mod_release_handler(modcode);
    } else {
      mod_press_handler(modcode);
    }
  }

  return 0;
}

int start_monitoring_mods_libinput(const char *device_name, KeyHandlerLibInput mod_press_handler,
                                   KeyHandlerLibInput mod_release_handler) {
  char *device_paths[MAX_DEVICES];
  int num_devices = 0;
  struct libinput *li;
  struct libinput_device *device;
  struct pollfd fds;

  num_devices = get_device_paths(device_name, device_paths, MAX_DEVICES);
  if (num_devices < 0) {
    return LIBINPUT_FAILED_TO_LIST_DEVICES;
  }
  if (num_devices == 0) {
    return LIBINPUT_NO_MATCHING_DEVICES;
  }

  // Create libinput context
  li = libinput_path_create_context(&interface, NULL);
  if (!li) {
    return LIBINPUT_ERROR;
  }
  // Add all device paths with --device flag
  for (int i = 0; i < num_devices; i++) {
    device = libinput_path_add_device(li, device_paths[i]);
    if (!device) {
      return LIBINPUT_ERROR;
    }
  }

  // Set up polling
  fds.fd = libinput_get_fd(li);
  fds.events = POLLIN;
  fds.revents = 0;

  // Main event loop
  while (1) {
    // Poll for events with timeout
    int ret = poll(&fds, 1, -1); // 100ms timeout
    if (ret < 0) {
      if (errno == EINTR) {
        continue; // Interrupted by signal, continue
      }
      perror("poll");
      break;
    }
    if (ret == 0) {
      // Timeout
      continue;
    }
    // Dispatch events
    libinput_dispatch(li);
    struct libinput_event *event;
    while ((event = libinput_get_event(li)) != NULL) {
      enum libinput_event_type type = libinput_event_get_type(event);
      if (type == LIBINPUT_EVENT_KEYBOARD_KEY) {
        struct libinput_event_keyboard *kb_event = libinput_event_get_keyboard_event(event);

        uint32_t key = libinput_event_keyboard_get_key(kb_event);
        enum libinput_key_state state = libinput_event_keyboard_get_key_state(kb_event);

        handle_event(key, state, mod_press_handler, mod_release_handler);
      }
      libinput_event_destroy(event);
    }
  }
  return 0;
}
