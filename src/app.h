#ifndef _SWITCH_APP_H_
#define _SWITCH_APP_H_

#define APP_NAME_SIZE 64

struct App {
  char name[APP_NAME_SIZE];
  int modcode;
  volatile int pressed;
  struct StackItem *switching_current;
  struct StackItem *top;
};

struct StackItem;

struct App *new_app(const char *name, int modcode);
void destroy_app(struct App* app);
int add_item(struct App* app, const char *id);
int set_item(struct App* app, const char *id);
int switch_item(struct App* app, int forward, struct StackItem **item);
int select_item(struct App *app);
int delete_item(struct App *app, const char* id);

#endif // _SWITCH_APP_H_
