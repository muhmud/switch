#ifndef _SWITCH_APP_KEYMAP_H_
#define _SWITCH_APP_KEYMAP_H_

struct App;

struct AppNode {
  struct App *app;
  struct AppNode *next;
};

int init_app_keymap(void);

int add_app(const char* app, int modcode);
struct App* find_app(const char *app);
struct AppNode *find_apps_by_modcode(int modcode);
int delete_app(const char *app);
int restart_app(const char* app);
void clear_apps(void);

int app_keymap_lock(void);
int app_keymap_unlock(void);

#endif // _SWITCH_APP_KEYMAP_H_
