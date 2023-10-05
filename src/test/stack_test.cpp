#include "src.hpp"
#include <gtest/gtest.h>
#include <memory>
#include <string>

#define BOTTOM "bottom"
#define MIDDLE "middle"
#define TOP "top"

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
  auto bottom = allocate(new_stack_item(BOTTOM));
  auto top = allocate(add_stack_item(bottom.get(), TOP));

  // Check we got something back
  EXPECT_TRUE(top);

  // Check the top
  EXPECT_STREQ(top->id, TOP);
  EXPECT_EQ(top->prev, bottom.get());
  EXPECT_EQ(top->next, bottom.get());

  // Check bottom
  EXPECT_STREQ(bottom->id, BOTTOM);
  EXPECT_EQ(bottom->prev, top.get());
  EXPECT_EQ(bottom->next, top.get());
}

TEST(StackTest, add_stack_item_three_items) {
  auto bottom = allocate(new_stack_item(BOTTOM));
  auto middle = allocate(add_stack_item(bottom.get(), MIDDLE));
  auto top = allocate(add_stack_item(middle.get(), TOP));

  // Check we got something back
  EXPECT_TRUE(top);

  // Check the top
  EXPECT_STREQ(top->id, TOP);
  EXPECT_EQ(top->next, middle.get());
  EXPECT_EQ(top->prev, bottom.get());

  // Check the middle
  EXPECT_STREQ(middle->id, MIDDLE);
  EXPECT_EQ(middle->next, bottom.get());
  EXPECT_EQ(middle->prev, top.get());

  // Check the bottom
  EXPECT_STREQ(bottom->id, BOTTOM);
  EXPECT_EQ(bottom->next, top.get());
  EXPECT_EQ(bottom->prev, middle.get());
}

TEST(StackTest, find_stack_item) {
  auto bottom = allocate(new_stack_item(BOTTOM));

  auto found = find_stack_item(bottom.get(), TOP);
  EXPECT_FALSE(found);

  found = find_stack_item(bottom.get(), BOTTOM);
  EXPECT_TRUE(found);
  EXPECT_STREQ(found->id, BOTTOM);

  // Add an additional item
  auto middle = allocate(add_stack_item(bottom.get(), MIDDLE));

  found = find_stack_item(bottom.get(), TOP);
  EXPECT_FALSE(found);

  found = find_stack_item(bottom.get(), BOTTOM);
  EXPECT_TRUE(found);
  EXPECT_STREQ(found->id, BOTTOM);

  found = find_stack_item(bottom.get(), MIDDLE);
  EXPECT_TRUE(found);
  EXPECT_STREQ(found->id, MIDDLE);

  // Add an another additional item
  auto top = allocate(add_stack_item(bottom.get(), TOP));

  found = find_stack_item(bottom.get(), TOP);
  EXPECT_TRUE(found);
  EXPECT_STREQ(found->id, TOP);

  found = find_stack_item(bottom.get(), MIDDLE);
  EXPECT_TRUE(found);
  EXPECT_STREQ(found->id, MIDDLE);

  found = find_stack_item(bottom.get(), BOTTOM);
  EXPECT_TRUE(found);
  EXPECT_STREQ(found->id, BOTTOM);
}

TEST(StackTest, move_to_top_of_stack) {
  auto bottom = allocate(new_stack_item(BOTTOM));

  auto current_top = move_to_top_of_stack(bottom.get(), bottom.get());
  EXPECT_EQ(current_top, bottom.get());

  // Add an additional item
  auto middle = allocate(new_stack_item(MIDDLE));
  current_top = move_to_top_of_stack(bottom.get(), middle.get());
  EXPECT_EQ(current_top, middle.get());
  EXPECT_EQ(middle->next, bottom.get());
  EXPECT_EQ(middle->prev, bottom.get());
  EXPECT_EQ(bottom->next, middle.get());
  EXPECT_EQ(bottom->prev, middle.get());

  // Add another additional item
  auto top = allocate(new_stack_item(TOP));
  current_top = move_to_top_of_stack(middle.get(), top.get());
  EXPECT_EQ(current_top, top.get());
  EXPECT_EQ(top->next, middle.get());
  EXPECT_EQ(top->prev, bottom.get());
  EXPECT_EQ(middle->next, bottom.get());
  EXPECT_EQ(middle->prev, top.get());
  EXPECT_EQ(bottom->next, top.get());
  EXPECT_EQ(bottom->prev, middle.get());
}

TEST(StackTest, delete_stack_item) {
  auto bottom = new_stack_item(BOTTOM);
  auto middle = allocate(new_stack_item(MIDDLE));

  EXPECT_EQ(delete_stack_item(bottom, MIDDLE), bottom);

  move_to_top_of_stack(bottom, middle.get());
  EXPECT_EQ(delete_stack_item(bottom, BOTTOM), middle.get());
  EXPECT_FALSE(middle->prev);
  EXPECT_FALSE(middle->next);
}

TEST(StackTest, delete_stack_item_three_items_middle) {
  auto bottom = new_stack_item(BOTTOM);
  auto middle = add_stack_item(bottom, MIDDLE);
  auto top = add_stack_item(middle, TOP);

  EXPECT_EQ(delete_stack_item(top, MIDDLE), top);
  EXPECT_EQ(top->next, bottom);
  EXPECT_EQ(top->prev, bottom);
  EXPECT_EQ(bottom->next, top);
  EXPECT_EQ(bottom->prev, top);

  EXPECT_EQ(delete_stack_item(top, TOP), bottom);
  EXPECT_FALSE(bottom->next);
  EXPECT_FALSE(bottom->prev);

  EXPECT_FALSE(delete_stack_item(top, BOTTOM));
}
