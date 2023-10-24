#include "stack.h"
#include <stdlib.h>
#include <string.h>

struct StackItem *new_stack_item(const char *id) {
  struct StackItem *item;

  if (!id || strlen(id) == 0) {
    return NULL;
  }
  item = (struct StackItem *)malloc(sizeof(struct StackItem));
  strncpy(item->id, id, sizeof(item->id));
  item->next = item->prev = NULL;
  return item;
}

struct StackItem *add_stack_item(struct StackItem *top, const char *id) {
  struct StackItem *item;

  item = find_stack_item(top, id);
  if (item) {
    return move_to_top_of_stack(top, item);
  }
  return move_to_top_of_stack(top, new_stack_item(id));
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

struct StackItem *move_to_top_of_stack(struct StackItem *top, struct StackItem *item) {
  if (!top || top == item) {
    return item;
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
  return item;
}

struct StackItem *delete_stack_item(struct StackItem *top, const char *id) {
  struct StackItem *item;

  item = find_stack_item(top, id);
  if (item == NULL) {
    return top;
  }
  if (item == top && top->next && top == top->next->next) {
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
  return top;
}

void delete_stack(struct StackItem *top) {
  struct StackItem *current, *next;

  if (top) {
    current = top;
    do {
      next = current->next;
      free(current);
      current = next;
    } while (current && current != top);
  }
}
