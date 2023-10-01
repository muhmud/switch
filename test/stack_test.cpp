#include "src.h"
#include <gtest/gtest.h>

TEST(StackTest, new_stack_item) {
  const char *id = "abc";
  struct StackItem *item = new_stack_item(id);

  EXPECT_STREQ(item->id, id);
}
