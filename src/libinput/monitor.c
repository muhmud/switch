#include "monitor.h"
#include "../mods.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_DEVICES 16
#define BUFFER_SIZE 4096

// Check for common modifier keys
static const char *modifiers[] = {"KEY_LEFTSHIFT", "KEY_RIGHTSHIFT", "KEY_LEFTCTRL",
                                  "KEY_RIGHTCTRL", "KEY_LEFTALT",    "KEY_RIGHTALT",
                                  "KEY_LEFTMETA",  "KEY_RIGHTMETA",  NULL};

// Get device paths for a given device name using libinput list-devices
int get_device_paths(const char *device_name, char **paths, int max_paths) {
  FILE *fp;
  char buffer[BUFFER_SIZE];
  char cmd[BUFFER_SIZE];
  int count = 0;
  int in_target_device = 0;

  // Run libinput list-devices
  fp = popen("sudo libinput list-devices", "r");
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

int handle_event(const char *line, KeyHandlerLibInput mod_press_handler,
                 KeyHandlerLibInput mod_release_handler) {
  int is_released = 0;
  int modcode = 0;

  // Check for KEY_RELEASED events
  if (strstr(line, "released") == NULL) {
    is_released = 1;
  }

  for (int i = 0; modifiers[i] != NULL; i++) {
    if (strstr(line, modifiers[i]) != NULL) {
      modcode = convert_libinput_to_modcode(modifiers[i]);
    }
  }

  if (modcode != 0) {
    if (is_released == 1) {
      mod_release_handler(modcode);
    } else {
      mod_press_handler(modcode);
    }
  }

  return 0;
}

// Process a modifier release event
void handle_modifier_release(const char *line) {
  printf(">>> MODIFIER RELEASED: %s", line);
  // Add your custom handling here
}

int start_libinput_child_process(const char *device_name, pid_t *child_pid, int *fd) {
  int pipefd[2];
  char *device_paths[MAX_DEVICES];
  int num_devices = 0;

  num_devices = get_device_paths(device_name, device_paths, MAX_DEVICES);
  if (num_devices < 0) {
    return LIBINPUT_FAILED_TO_LIST_DEVICES;
  }
  if (num_devices == 0) {
    return LIBINPUT_NO_MATCHING_DEVICES;
  }

  if (pipe(pipefd) == -1) {
    return EXIT_FAILURE;
  }

  // Fork process
  *child_pid = fork();
  if (*child_pid == -1) {
    return EXIT_FAILURE;
  }

  if (*child_pid == 0) {
    close(pipefd[0]); // Close read end

    // Redirect stdout and stderr to pipe
    dup2(pipefd[1], STDOUT_FILENO);
    dup2(pipefd[1], STDERR_FILENO);
    close(pipefd[1]);

    // Build command arguments with device paths
    // Args: "sudo" + "libinput" + "debug-events" + "--show-keycodes" +
    //       (num_devices * 2 for "--device" "path") + NULL
    int total_args = 4 + (num_devices * 2) + 1;
    char **args = malloc(total_args * sizeof(char *));
    if (args == NULL) {
      perror("malloc");
      exit(EXIT_FAILURE);
    }

    int idx = 0;
    args[idx++] = "sudo";
    args[idx++] = "libinput";
    args[idx++] = "debug-events";
    args[idx++] = "--show-keycodes";

    // Add all device paths with --device flag
    for (int i = 0; i < num_devices; i++) {
      args[idx++] = "--device";
      args[idx++] = device_paths[i];
    }
    args[idx] = NULL;

    // Execute libinput command
    execvp("sudo", args);

    // If execvp returns, an error occurred
    free(args);
    exit(EXIT_FAILURE);
  }

  // Parent process
  close(pipefd[1]); // Close write end
  *fd = pipefd[0];
  return 0;
}

int start_monitoring_mods_libinput(int fd, KeyHandlerLibInput mod_press_handler,
                                   KeyHandlerLibInput mod_release_handler) {
  FILE *stream;
  char buffer[BUFFER_SIZE];
  const char *modifier = NULL;

  stream = fdopen(fd, "r");
  if (stream == NULL) {
    return EXIT_FAILURE;
  }

  // Read output line by line
  while (fgets(buffer, BUFFER_SIZE, stream) != NULL) {
    handle_event(buffer, mod_press_handler, mod_release_handler);
  }

  fclose(stream);
  return 0;
}
