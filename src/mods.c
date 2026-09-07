#include "mods.h"
#include <X11/keysym.h>
#include <linux/input-event-codes.h>
#include <string.h>

struct ModCodes find_modcodes(int modcode) {
  struct ModCodes modcodes = {-1, -1};

  switch (modcode) {
  case Shift_L:
    modcodes.left = Shift_L;
    break;
  case Shift_R:
    modcodes.right = Shift_R;
    break;
  case Shift:
    modcodes.left = Shift_L;
    modcodes.right = Shift_R;
    break;
  case Control_L:
    modcodes.left = Control_L;
    break;
  case Control_R:
    modcodes.right = Control_R;
    break;
  case Control:
    modcodes.left = Control_L;
    modcodes.right = Control_R;
    break;
  case Meta_L:
    modcodes.left = Meta_L;
    break;
  case Meta_R:
    modcodes.right = Meta_R;
    break;
  case Meta:
    modcodes.left = Meta_L;
    modcodes.right = Meta_R;
    break;
  case Alt_L:
    modcodes.left = Alt_L;
    break;
  case Alt_R:
    modcodes.right = Alt_R;
    break;
  case Alt:
    modcodes.left = Alt_L;
    modcodes.right = Alt_R;
    break;
  case Super_L:
    modcodes.left = Super_L;
    break;
  case Super_R:
    modcodes.right = Super_R;
    break;
  case Super:
    modcodes.left = Super_L;
    modcodes.right = Super_R;
    break;
  case Hyper_L:
    modcodes.left = Hyper_L;
    break;
  case Hyper_R:
    modcodes.right = Hyper_R;
    break;
  case Hyper:
    modcodes.left = Hyper_L;
    modcodes.right = Hyper_R;
    break;
  }
  return modcodes;
}

int convert_keysym_to_modcode(unsigned long keysym) {
  int modcode;

  modcode = -1;
  switch (keysym) {
  case XK_Shift_L:
    modcode = Shift_L;
    break;
  case XK_Shift_R:
    modcode = Shift_R;
    break;
  case XK_Control_L:
    modcode = Control_L;
    break;
  case XK_Control_R:
    modcode = Control_R;
    break;
  case XK_Meta_L:
    modcode = Meta_L;
    break;
  case XK_Meta_R:
    modcode = Meta_R;
    break;
  case XK_Alt_L:
    modcode = Alt_L;
    break;
  case XK_Alt_R:
    modcode = Alt_R;
    break;
  case XK_Super_L:
    modcode = Super_L;
    break;
  case XK_Super_R:
    modcode = Super_R;
    break;
  case XK_Hyper_L:
    modcode = Hyper_L;
    break;
  case XK_Hyper_R:
    modcode = Hyper_R;
    break;
  }
  return modcode;
}

int convert_libinput_to_modcode(uint32_t key) {
  int modcode;

  modcode = -1;
  switch (key) {
  case KEY_LEFTSHIFT:
    modcode = Shift_L;
    break;
  case KEY_RIGHTSHIFT:
    modcode = Shift_R;
    break;
  case KEY_LEFTCTRL:
    modcode = Control_L;
    break;
  case KEY_RIGHTCTRL:
    modcode = Control_R;
    break;
  case KEY_LEFTALT:
    modcode = Alt_L;
    break;
  case KEY_RIGHTALT:
    modcode = Alt_R;
    break;
  case KEY_LEFTMETA:
    modcode = Super_L;
    break;
  case KEY_RIGHTMETA:
    modcode = Super_R;
    break;
  }
  return modcode;
}

int convert_string_to_modcode(const char *code) {
  int modcode;

  modcode = -1;
  if (code) {
    if (strcmp(code, "shift_l") == 0) {
      modcode = Shift_L;
    } else if (strcmp(code, "shift_r") == 0) {
      modcode = Shift_R;
    } else if (strcmp(code, "shift") == 0) {
      modcode = Shift;
    } else if (strcmp(code, "ctrl_l") == 0) {
      modcode = Control_L;
    } else if (strcmp(code, "ctrl_r") == 0) {
      modcode = Control_R;
    } else if (strcmp(code, "ctrl") == 0) {
      modcode = Control;
    } else if (strcmp(code, "meta_l") == 0) {
      modcode = Meta_L;
    } else if (strcmp(code, "meta_r") == 0) {
      modcode = Meta_R;
    } else if (strcmp(code, "meta") == 0) {
      modcode = Meta;
    } else if (strcmp(code, "alt_l") == 0) {
      modcode = Alt_L;
    } else if (strcmp(code, "alt_r") == 0) {
      modcode = Alt_R;
    } else if (strcmp(code, "alt") == 0) {
      modcode = Alt;
    } else if (strcmp(code, "super_l") == 0) {
      modcode = Super_L;
    } else if (strcmp(code, "super_r") == 0) {
      modcode = Super_R;
    } else if (strcmp(code, "Super") == 0) {
      modcode = Super;
    } else if (strcmp(code, "hyper_l") == 0) {
      modcode = Hyper_L;
    } else if (strcmp(code, "hyper_r") == 0) {
      modcode = Hyper_R;
    } else if (strcmp(code, "hyper") == 0) {
      modcode = Hyper;
    }
  }
  return modcode;
}

int is_mod_key(unsigned long keysym) {
  return (keysym == XK_Shift_L || keysym == XK_Shift_R || keysym == XK_Control_L ||
          keysym == XK_Control_R || keysym == XK_Meta_L || keysym == XK_Meta_R ||
          keysym == XK_Alt_L || keysym == XK_Alt_R || keysym == XK_Super_L ||
          keysym == XK_Super_R || keysym == XK_Hyper_L || keysym == XK_Hyper_R);
}

int is_valid_mod_key(int modcode) { return modcode >= 0 && modcode <= Hyper ? 1 : 0; }

/* Trigger keys the daemon can watch for. Letters and digits cover the useful
   cases; tab and space are here because they are the conventional choices for
   this sort of switching. */
static const struct {
  const char *name;
  unsigned int code;
} keycodes[] = {
    {"a", KEY_A}, {"b", KEY_B}, {"c", KEY_C}, {"d", KEY_D}, {"e", KEY_E},
    {"f", KEY_F}, {"g", KEY_G}, {"h", KEY_H}, {"i", KEY_I}, {"j", KEY_J},
    {"k", KEY_K}, {"l", KEY_L}, {"m", KEY_M}, {"n", KEY_N}, {"o", KEY_O},
    {"p", KEY_P}, {"q", KEY_Q}, {"r", KEY_R}, {"s", KEY_S}, {"t", KEY_T},
    {"u", KEY_U}, {"v", KEY_V}, {"w", KEY_W}, {"x", KEY_X}, {"y", KEY_Y},
    {"z", KEY_Z}, {"0", KEY_0}, {"1", KEY_1}, {"2", KEY_2}, {"3", KEY_3},
    {"4", KEY_4}, {"5", KEY_5}, {"6", KEY_6}, {"7", KEY_7}, {"8", KEY_8},
    {"9", KEY_9}, {"tab", KEY_TAB}, {"space", KEY_SPACE},
    {"grave", KEY_GRAVE}, {"backslash", KEY_BACKSLASH},
};

int convert_string_to_keycode(const char *name) {
  size_t i;

  if (!name || !*name) {
    return -1;
  }
  for (i = 0; i < sizeof(keycodes) / sizeof(keycodes[0]); i++) {
    if (strcasecmp(name, keycodes[i].name) == 0) {
      return (int)keycodes[i].code;
    }
  }
  return -1;
}
