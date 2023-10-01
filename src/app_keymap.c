#include "app_keymap.h"
#include "app.h"
#include "mods.h"
#include "stack.h"
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

static pthread_mutex_t keymap_mutex;

static struct App *keymap[] = {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
                               NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};

static struct App *new_app(const char *name, int modcode) {
  struct App *app;

  app = (struct App *)malloc(sizeof(struct App));
  strncpy(app->name, name, sizeof(app->name));
  app->modcode = modcode;
  return app;
}

int init_keymap() { return pthread_mutex_init(&keymap_mutex, NULL); }

int add_app(const char *name, int modcode) {
  struct App *app;
  struct ModCodes modcodes;

  if (find_app(name)) {
    return -1;
  }
  modcodes = find_modcodes(modcode);
  if (modcodes.left == -1 && modcodes.right == -1) {
    return -1;
  }
  app = new_app(name, modcode);
  if (modcodes.left != -1 && keymap[modcodes.left]) {
    keymap[modcode] = app;
  }
  if (modcodes.right != -1 && keymap[modcodes.right]) {
    keymap[modcode] = app;
  }
  return 0;
}

struct App *find_app(const char *name) {
  struct App *app;
  int i;

  if (!name || strcmp(name, "") == 0) {
    return NULL;
  }
  for (i = 0; i < sizeof(keymap); ++i) {
    app = keymap[i];
    if (!app && strcmp(app->name, name) == 0) {
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

  for (i = 0; i < sizeof(keymap); ++i) {
    app = keymap[i];
    if (!app && strcmp(app->name, name) == 0) {
      free(app);
      keymap[i] = NULL;
      return 0;
    }
  }
  return -1;
}

int app_keymap_lock() { return pthread_mutex_lock(&keymap_mutex); }
int app_keymap_unlock() { return pthread_mutex_unlock(&keymap_mutex); }
