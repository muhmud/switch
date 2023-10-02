#include "app_keymap.h"
#include "app.h"
#include "mods.h"
#include "stack.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static pthread_mutex_t keymap_mutex;

static struct App *keymap[] = {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
                               NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};

static int empty_string(const char *value) { return !value || strcmp(value, "") == 0 ? 1 : 0; }

static struct App *new_app(const char *name, int modcode) {
  struct App *app;

  app = (struct App *)malloc(sizeof(struct App));
  strncpy(app->name, name, sizeof(app->name));
  app->modcode = modcode;
  return app;
}

static int update_keymap(int modcode, struct App *app) {
  if (modcode != -1) {
    if (keymap[modcode]) {
      return -1;
    }
    keymap[modcode] = app;
  }
  return 0;
}

int init_keymap() { return pthread_mutex_init(&keymap_mutex, NULL); }

int add_app(const char *name, int modcode) {
  struct App *app;
  struct ModCodes modcodes;

  if (empty_string(name) || find_app(name)) {
    return -1;
  }
  modcodes = find_modcodes(modcode);
  if (modcodes.left == -1 && modcodes.right == -1) {
    return -1;
  }
  app = new_app(name, modcode);
  if (update_keymap(modcodes.left, app) == -1 || update_keymap(modcodes.right, app) == -1) {
    return -1;
  }
  return 0;
}

struct App *find_app(const char *name) {
  struct App *app;
  int i;

  if (empty_string(name)) {
    return NULL;
  }
  for (i = 0; i < sizeof(keymap) / sizeof(struct App *); ++i) {
    app = keymap[i];
    if (app && strcmp(app->name, name) == 0) {
      return app;
    }
  }
  return NULL;
}

struct App *find_app_by_modcode(int modcode) { return keymap[modcode]; }

void select_stack_item(struct App *app) {
  if (app->switching_current) {
    move_to_top_of_stack(app->top, app->switching_current);
    app->switching_current = NULL;
  }
}

int delete_app(const char *name) {
  struct App *app;
  int i;

  if (empty_string(name)) {
    return -1;
  }
  for (i = 0; i < sizeof(keymap) / sizeof(struct App *); ++i) {
    app = keymap[i];
    if (app && strcmp(app->name, name) == 0) {
      free(app);
      keymap[i] = NULL;
      return 0;
    }
  }
  return -1;
}

void clear_apps() {
  struct App *app;
  int i, j;

  for (i = 0; i < sizeof(keymap) / sizeof(struct App *); ++i) {
    app = keymap[i];
    if (app) {
      for (j = i + 1; j < sizeof(keymap) / sizeof(struct App *); ++j) {
        if (app == keymap[j]) {
          keymap[j] = NULL;
          break;
        }
      }
      free(app);
      keymap[i] = NULL;
    }
  }
}

int app_keymap_lock() { return pthread_mutex_lock(&keymap_mutex); }
int app_keymap_unlock() { return pthread_mutex_unlock(&keymap_mutex); }
