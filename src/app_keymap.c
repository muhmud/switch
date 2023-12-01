#include "app_keymap.h"
#include "app.h"
#include "mods.h"
#include "stack.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static pthread_mutex_t keymap_mutex;

static struct AppNode *new_app_node(struct App *app) {
  struct AppNode *node;

  node = (struct AppNode *)malloc(sizeof(struct AppNode));
  node->app = app;
  return node;
}

static struct AppNode *keymap[] = {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
                                   NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};

static int empty_string(const char *value) { return !value || strcmp(value, "") == 0 ? 1 : 0; }

static int update_keymap(int modcode, struct AppNode *node) {
  if (modcode != -1) {
    node->next = keymap[modcode];
    keymap[modcode] = node;
  }
  return 0;
}

int init_keymap(void) { return pthread_mutex_init(&keymap_mutex, NULL); }

int add_app(const char *name, int modcode) {
  struct AppNode *node;
  struct ModCodes modcodes;

  if (empty_string(name) || find_app(name)) {
    return -1;
  }
  modcodes = find_modcodes(modcode);
  if (modcodes.left == -1 && modcodes.right == -1) {
    return -1;
  }
  node = new_app_node(new_app(name, modcode));
  if (update_keymap(modcodes.left, node) == -1 || update_keymap(modcodes.right, node) == -1) {
    return -1;
  }
  return 0;
}

struct App *find_app(const char *name) {
  struct AppNode *node;
  int i;

  if (empty_string(name)) {
    return NULL;
  }
  for (i = 0; i < sizeof(keymap) / sizeof(struct AppNode *); ++i) {
    node = keymap[i];
    while (node) {
      if (strcmp(node->app->name, name) == 0) {
        return node->app;
      }
      node = node->next;
    }
  }
  return NULL;
}

struct AppNode *find_apps_by_modcode(int modcode) { return keymap[modcode]; }

void select_stack_item(struct App *app) {
  if (app->switching_current) {
    app->top = move_to_top_of_stack(app->top, app->switching_current);
    app->switching_current = NULL;
  }
}

static struct AppNode *remove_item(int index, const char *name) {
  struct AppNode *node, *parent;

  node = keymap[index];
  parent = NULL;
  while (node) {
    if (node->app && strcmp(node->app->name, name) == 0) {
      if (parent) {
        parent->next = node->next;
      } else {
        keymap[index] = node->next;
      }
      return node;
    }
    parent = node;
    node = node->next;
  }
  return NULL;
}

int delete_app(const char *name) {
  struct AppNode *node, *parent;
  int i;

  if (empty_string(name)) {
    return -1;
  }
  for (i = 0; i < sizeof(keymap) / sizeof(struct AppNode *); ++i) {
    node = keymap[i];
    parent = NULL;
    while (node) {
      if (node->app && strcmp(node->app->name, name) == 0) {
        if (parent) {
          parent->next = node->next;
        } else {
          keymap[i] = node->next;
        }
        if (i != node->app->modcode) {
          // remove the item in related list
          remove_item(i + 1, node->app->name);
        }
        destroy_app(node->app);
        free(node);
        return 0;
      }
      parent = node;
      node = node->next;
    }
  }
  return -1;
}

int restart_app(const char *name) {
  struct App *app;

  if (empty_string(name)) {
    return -1;
  }
  app = find_app(name);
  if (!app) {
    return -1;
  }
  delete_stack(app->top);
  app->top = NULL;
  return 0;
}

void clear_apps(void) {
  struct AppNode *node, *next;
  int i;

  for (i = 0; i < sizeof(keymap) / sizeof(struct AppNode *); ++i) {
    node = keymap[i];
    while (node) {
      next = node->next;
      if (node->app) {
        if (i != node->app->modcode) {
          // remove the item in related list
          remove_item(i + 1, node->app->name);
        }
        destroy_app(node->app);
        free(node);
      }
      node = next;
    }
    keymap[i] = NULL;
  }
}

int app_keymap_lock(void) { return pthread_mutex_lock(&keymap_mutex); }
int app_keymap_unlock(void) { return pthread_mutex_unlock(&keymap_mutex); }
