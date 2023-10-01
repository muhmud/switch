#ifndef _SWITCH_APP_KEYMAP_H_
#define _SWITCH_APP_KEYMAP_H_

struct App;

int init_app_keymap();

int add_app(const char* app, int modcode);
struct App *find_app(const char *app);
struct App *find_app_by_modcode(int modcode);
int delete_app(const char *app);

int app_keymap_lock();
int app_keymap_unlock();

#endif // _SWITCH_APP_KEYMAP_H_
