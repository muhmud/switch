#ifndef _SWITCH_APP_H_
#define _SWITCH_APP_H_
/* Under C++ (the tests) <stdatomic.h> does not provide atomic_int — it became
   available only in C++23 — and <atomic> cannot be included here because these
   headers are pulled in under extern "C". The C++ harness supplies the name
   instead; see src/test/src.hpp. */
#ifndef __cplusplus
#include <stdatomic.h>
#endif

#define APP_NAME_SIZE 64
#define APP_EXEC_SIZE 512

struct App {
  char name[APP_NAME_SIZE];
  int modcode;
  /* Trigger key, as an evdev keycode; 0 when the app has none.
     An app with a trigger key is switched by the daemon itself: it sees the
     whole chord on the input device, so no client request is needed and the
     application need not bind the key. */
  unsigned int key;
  /* Command run when the daemon switches this app on its own. The chosen id is
     appended as a final argument. Empty when unused. */
  char exec[APP_EXEC_SIZE];
  atomic_int pressed;
  struct StackItem *switching_current;
  struct StackItem *top;
};

struct StackItem;

struct App *new_app(const char *name, int modcode, unsigned int key, const char *exec);
void destroy_app(struct App *app);
int add_item(struct App *app, const char *id);
int set_item(struct App *app, const char *id);
int switch_item(struct App *app, int forward, struct StackItem **item);
int select_item(struct App *app);
int delete_item(struct App *app, const char *id);

#endif // _SWITCH_APP_H_
