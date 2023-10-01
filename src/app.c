#include "app.h"
#include "stack.h"
#include <stdlib.h>

int add_item(struct App *app, const char *id) {
  return app && add_stack_item(app->top, id) ? 0 : -1;
}

int set_item(struct App *app, const char *id) {
  struct StackItem *item;
  int moved_to_top;

  if (!app) {
    return -1;
  }
  moved_to_top = 0;
  item = find_stack_item(app->top, id);
  if (!item) {
    move_to_top_of_stack(app->top, new_stack_item(id));
    moved_to_top = 1;
  }
  if (app->pressed == 1) {
    app->switching_current = item;
  } else if (moved_to_top == 0) {
    move_to_top_of_stack(app->top, item);
  }
  return 0;
}

int switch_item(struct App *app, int forward, struct StackItem **item) {
  if (!app || !app->top) {
    return -1;
  }
  if (app->switching_current) {
    if (forward == 1) {
      if (app->switching_current->next) {
        app->switching_current = app->switching_current->next;
      }
    } else {
      if (app->switching_current->prev) {
        app->switching_current = app->switching_current->prev;
      }
    }
  } else {
    if (app->top) {
      if (forward == 1 && app->top->next) {
        app->switching_current = app->top->next;
      } else if (forward == 0 && app->top->prev) {
        app->switching_current = app->top->prev;
      } else {
        app->switching_current = app->top;
      }
    }
  }
  *item = app->switching_current;
  return 0;
}

int select_item(struct App *app) {
  if (!app) {
    return -1;
  }
  if (app->switching_current) {
    move_to_top_of_stack(app->top, app->switching_current);
    app->switching_current = NULL;
  }
  return 0;
}

int delete_item(struct App *app, const char *id) {
  if (!app) {
    return -1;
  }
  return delete_stack_item(app->top, id);
}
