#ifndef _SWITCH_MODS_H_
#define _SWITCH_MODS_H_

#define Shift_L 0
#define Shift_R 1
#define Shift 2
#define Control_L 3
#define Control_R 4
#define Control 5
#define Meta_L 6
#define Meta_R 7
#define Meta 8
#define Alt_L 9
#define Alt_R 10
#define Alt 11
#define Super_L 12
#define Super_R 13
#define Super 14
#define Hyper_L 15
#define Hyper_R 16
#define Hyper 17

struct ModCodes {
  int left;
  int right;
};

struct ModCodes find_modcodes(int modcode);
int convert_keysym_to_modcode(unsigned long keysym);
int convert_string_to_modcode(const char *code);
int is_mod_key(unsigned long keysym);
int is_valid_mod_key(int modcode);

#endif // _SWITCH_MODS_H_
