#include "stack.h"
#include <stdlib.h>
#include <string.h>

struct StackItem *new_stack_item(const char *id) {
  struct StackItem *item;

  item = (struct StackItem *)malloc(sizeof(struct StackItem));
  strncpy(item->id, id, sizeof(item->id));
  item->next = NULL;
  item->prev = NULL;
  return item;
}

int add_stack_item(struct StackItem *top, const char *id) {
  struct StackItem *item;

  item = find_stack_item(top, id);
  if (item) {
    return -1;
  }
  move_to_top_of_stack(top, new_stack_item(id));
  return 0;
}

struct StackItem *find_stack_item(struct StackItem *top, const char *id) {
  struct StackItem *current;

  current = top;
  do {
    if (current == NULL) {
      return NULL;
    }
    if (strcmp(id, current->id) == 0) {
      return current;
    }
    current = current->next;
  } while (current != top);
  return NULL;
}

void move_to_top_of_stack(struct StackItem *top, struct StackItem *item) {
  if (top == item) {
    return;
  }
  if (item->prev && item->next) {
    item->prev->next = item->next;
    item->next->prev = item->prev;
  }
  item->next = top;
  if (top->prev && top->next) {
    item->prev = top->prev;
    top->prev->next = item;
  } else {
    item->prev = top;
    top->next = item;
  }
  top->prev = item;
  top = item;
}

int delete_stack_item(struct StackItem *top, const char *id) {
  struct StackItem *item;

  item = find_stack_item(top, id);
  if (item == NULL) {
    return -1;
  }
  if (item->next == top) {
    top->next = NULL;
    top->prev = NULL;
  } else if (item == top && top->next && top == top->next->next) {
    top = top->next;
    top->next = NULL;
    top->prev = NULL;
  } else if (item->prev && item->next) {
    item->prev->next = item->next;
    item->next->prev = item->prev;
    if (item == top) {
      top = item->next;
    }
  } else {
    top = NULL;
  }
  free(item);
  return 0;
}
