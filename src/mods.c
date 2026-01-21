#include "mods.h"
#include <X11/keysym.h>
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

int convert_libinput_to_modcode(const char *code) {
  int modcode;

  modcode = -1;
  if (code) {
    if (strcmp(code, "KEY_LEFTSHIFT") == 0) {
      modcode = Shift_L;
    } else if (strcmp(code, "KEY_RIGHTSHIFT") == 0) {
      modcode = Shift_R;
    } else if (strcmp(code, "KEY_LEFTCTRL") == 0) {
      modcode = Control_L;
    } else if (strcmp(code, "KEY_RIGHTCTRL") == 0) {
      modcode = Control_R;
    } else if (strcmp(code, "KEY_LEFTALT") == 0) {
      modcode = Alt_L;
    } else if (strcmp(code, "KEY_RIGHTALT") == 0) {
      modcode = Alt_R;
    } else if (strcmp(code, "KEY_LEFTMETA") == 0) {
      modcode = Super_L;
    } else if (strcmp(code, "KEY_RIGHTMETA") == 0) {
      modcode = Super_R;
    }
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
