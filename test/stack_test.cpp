#include "src.hpp"
#include <gtest/gtest.h>
#include <memory>
#include <string>

#define TOP "top"
#define NEW "new"

TEST(StackTest, new_stack_item) {
  const char *id = "abc";
  auto item = allocate(new_stack_item(id));

  EXPECT_STREQ(item->id, id);
  EXPECT_FALSE(item->prev);
  EXPECT_FALSE(item->next);
}

TEST(StackTest, new_stack_item_invalid_id) {
  const char *id = "";
  auto item = allocate(new_stack_item(id));
  EXPECT_FALSE(item);

  id = NULL;
  item = allocate(new_stack_item(id));
  EXPECT_FALSE(item);
}

TEST(StackTest, new_stack_item_id_too_long) {
  auto id = std::string(STACK_ITEM_ID_SIZE + 5, 'c');
  auto item = allocate(new_stack_item(id.c_str()));

  EXPECT_STREQ(item->id, id.substr(0, STACK_ITEM_ID_SIZE).c_str());
}

TEST(StackTest, add_stack_item) {
  auto stack = allocate(new_stack_item(TOP));

  EXPECT_STREQ(stack->id, TOP);
  EXPECT_FALSE(stack->prev);
  EXPECT_FALSE(stack->next);
}

TEST(StackTest, add_stack_item_two_items) {
  auto stack = allocate(new_stack_item(TOP));
  auto new_top = allocate(add_stack_item(stack.get(), NEW));

  // Check we got something back
  EXPECT_TRUE(new_top);

  // Check the new top
  EXPECT_STREQ(new_top->id, NEW);
  EXPECT_EQ(new_top->prev, stack.get());
  EXPECT_EQ(new_top->next, stack.get());
  EXPECT_EQ(new_top->prev, new_top->next);

  // Check previous top (stack)
  EXPECT_STREQ(stack->id, TOP);
  EXPECT_EQ(stack->prev, new_top.get());
  EXPECT_EQ(stack->next, new_top.get());
  EXPECT_EQ(stack->prev, stack->next);
}
