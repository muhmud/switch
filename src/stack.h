#ifndef _SWITCH_STACK_H_
#define _SWITCH_STACK_H_

#define STACK_ITEM_ID_SIZE 64

struct StackItem {
  char id[STACK_ITEM_ID_SIZE];
  struct StackItem *prev;
  struct StackItem *next;
};

struct StackItem *new_stack_item(const char *id);
int add_stack_item(struct StackItem* top, const char *id);
struct StackItem *find_stack_item(struct StackItem *top, const char *id);
void move_to_top_of_stack(struct StackItem *top, struct StackItem *item);
int delete_stack_item(struct StackItem *top, const char *id);

#endif // _SWITCH_STACK_H_
