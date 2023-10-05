#include "src.hpp"
#include <gtest/gtest.h>

#define TEST_APP "test"
#define TOP "top"
#define MIDDLE "middle"
#define BOTTOM "bottom"

TEST(AppTest, new_app) {
  auto app = allocate(new_app(TEST_APP, Alt));

  EXPECT_STREQ(app->name, TEST_APP);
  EXPECT_EQ(app->modcode, Alt);
  EXPECT_EQ(app->pressed, 0);
  EXPECT_FALSE(app->switching_current);
  EXPECT_FALSE(app->top);
}

TEST(AppTest, add_item) {
  auto app = allocate(new_app(TEST_APP, Alt));

  EXPECT_EQ(add_item(app.get(), TOP), 0);
  EXPECT_TRUE(app->top);
  EXPECT_STREQ(app->top->id, TOP);
}

TEST(AppTest, add_item_two_items) {
  auto app = allocate(new_app(TEST_APP, Alt));

  EXPECT_EQ(add_item(app.get(), BOTTOM), 0);
  EXPECT_EQ(add_item(app.get(), TOP), 0);

  EXPECT_TRUE(app->top);
  EXPECT_STREQ(app->top->id, TOP);

  EXPECT_TRUE(app->top->prev);
  EXPECT_STREQ(app->top->prev->id, BOTTOM);
  EXPECT_TRUE(app->top->next);
  EXPECT_STREQ(app->top->next->id, BOTTOM);
}

TEST(AppTest, add_item_three_items) {
  auto app = allocate(new_app(TEST_APP, Alt));

  EXPECT_EQ(add_item(app.get(), BOTTOM), 0);
  EXPECT_EQ(add_item(app.get(), MIDDLE), 0);
  EXPECT_EQ(add_item(app.get(), TOP), 0);

  EXPECT_TRUE(app->top);
  EXPECT_STREQ(app->top->id, TOP);

  // Check previous to top
  EXPECT_TRUE(app->top->prev);
  EXPECT_STREQ(app->top->prev->id, BOTTOM);
  EXPECT_EQ(app->top->prev->next, app->top);
  EXPECT_EQ(app->top->prev->prev, app->top->next);

  // Check next to top
  EXPECT_TRUE(app->top->next);
  EXPECT_STREQ(app->top->next->id, MIDDLE);
  EXPECT_EQ(app->top->next->prev, app->top);
  EXPECT_EQ(app->top->next->next, app->top->prev);
}

TEST(AppTest, set_item) {
  auto app = allocate(new_app(TEST_APP, Alt));

  EXPECT_EQ(add_item(app.get(), BOTTOM), 0);
  EXPECT_EQ(add_item(app.get(), MIDDLE), 0);
  EXPECT_EQ(set_item(app.get(), BOTTOM), 0);

  EXPECT_TRUE(app->top);
  EXPECT_STREQ(app->top->id, BOTTOM);
  EXPECT_TRUE(app->top->next);
  EXPECT_STREQ(app->top->next->id, MIDDLE);
}

TEST(AppTest, set_item_new) {
  auto app = allocate(new_app(TEST_APP, Alt));

  EXPECT_EQ(add_item(app.get(), BOTTOM), 0);
  EXPECT_EQ(add_item(app.get(), MIDDLE), 0);
  EXPECT_EQ(set_item(app.get(), TOP), 0);

  EXPECT_TRUE(app->top);
  EXPECT_STREQ(app->top->id, TOP);
  EXPECT_TRUE(app->top->next);
  EXPECT_STREQ(app->top->next->id, MIDDLE);
  EXPECT_TRUE(app->top->next->next);
  EXPECT_STREQ(app->top->next->next->id, BOTTOM);
}

TEST(AppTest, set_item_switching) {
  auto app = allocate(new_app(TEST_APP, Alt));

  EXPECT_EQ(add_item(app.get(), BOTTOM), 0);
  EXPECT_EQ(add_item(app.get(), TOP), 0);

  app->pressed = 1;
  EXPECT_EQ(set_item(app.get(), BOTTOM), 0);

  EXPECT_TRUE(app->top);
  EXPECT_STREQ(app->top->id, TOP);
  EXPECT_TRUE(app->top->next);
  EXPECT_STREQ(app->top->next->id, BOTTOM);
  EXPECT_EQ(app->switching_current, app->top->next);
}

TEST(AppTest, set_item_switching_new) {
  auto app = allocate(new_app(TEST_APP, Alt));

  EXPECT_EQ(add_item(app.get(), BOTTOM), 0);
  EXPECT_EQ(add_item(app.get(), MIDDLE), 0);

  app->pressed = 1;
  EXPECT_EQ(set_item(app.get(), TOP), 0);

  EXPECT_TRUE(app->top);
  EXPECT_STREQ(app->top->id, TOP);
  EXPECT_TRUE(app->top->next);
  EXPECT_STREQ(app->top->next->id, MIDDLE);
  EXPECT_TRUE(app->top->next->next);
  EXPECT_STREQ(app->top->next->next->id, BOTTOM);
  EXPECT_EQ(app->switching_current, app->top);
}

TEST(AppTest, switch_item) {
  auto app = allocate(new_app(TEST_APP, Alt));

  EXPECT_EQ(add_item(app.get(), BOTTOM), 0);
  EXPECT_EQ(add_item(app.get(), MIDDLE), 0);
  EXPECT_EQ(add_item(app.get(), TOP), 0);

  StackItem *item;
  EXPECT_EQ(switch_item(app.get(), 1, &item), 0);
  EXPECT_STREQ(item->id, MIDDLE);
  EXPECT_EQ(switch_item(app.get(), 1, &item), 0);
  EXPECT_STREQ(item->id, BOTTOM);

  EXPECT_EQ(select_item(app.get()), 0);
  EXPECT_STREQ(app->top->id, BOTTOM);
  EXPECT_STREQ(app->top->next->id, TOP);
  EXPECT_STREQ(app->top->next->next->id, MIDDLE);

  EXPECT_EQ(switch_item(app.get(), 1, &item), 0);
  EXPECT_STREQ(item->id, TOP);
}

TEST(AppTest, switch_item_reverse) {
  auto app = allocate(new_app(TEST_APP, Alt));

  EXPECT_EQ(add_item(app.get(), BOTTOM), 0);
  EXPECT_EQ(add_item(app.get(), MIDDLE), 0);
  EXPECT_EQ(add_item(app.get(), TOP), 0);

  StackItem *item;
  EXPECT_EQ(switch_item(app.get(), 0, &item), 0);
  EXPECT_STREQ(item->id, BOTTOM);
  EXPECT_EQ(switch_item(app.get(), 0, &item), 0);
  EXPECT_STREQ(item->id, MIDDLE);

  EXPECT_EQ(select_item(app.get()), 0);
  EXPECT_STREQ(app->top->id, MIDDLE);
  EXPECT_STREQ(app->top->next->id, TOP);
  EXPECT_STREQ(app->top->next->next->id, BOTTOM);

  EXPECT_EQ(switch_item(app.get(), 0, &item), 0);
  EXPECT_STREQ(item->id, BOTTOM);
}

TEST(AppTest, switch_item_invalid) {
  StackItem *item;
  EXPECT_EQ(switch_item(NULL, 1, &item), -1);

  auto app = allocate(new_app(TEST_APP, Alt));
  EXPECT_EQ(switch_item(app.get(), 1, &item), -1);
}

TEST(AppTest, select_item_not_switching) {
  auto app = allocate(new_app(TEST_APP, Alt));

  EXPECT_EQ(add_item(app.get(), BOTTOM), 0);
  EXPECT_EQ(add_item(app.get(), MIDDLE), 0);
  EXPECT_EQ(add_item(app.get(), TOP), 0);

  EXPECT_EQ(select_item(app.get()), 0);
  EXPECT_STREQ(app->top->id, TOP);
  EXPECT_STREQ(app->top->next->id, MIDDLE);
  EXPECT_STREQ(app->top->next->next->id, BOTTOM);
}

TEST(AppTest, select_item_invalid) { EXPECT_EQ(select_item(NULL), -1); }

TEST(AppTest, delete_item) {
  auto app = allocate(new_app(TEST_APP, Alt));

  EXPECT_EQ(add_item(app.get(), BOTTOM), 0);
  EXPECT_EQ(add_item(app.get(), MIDDLE), 0);
  EXPECT_EQ(add_item(app.get(), TOP), 0);

  EXPECT_EQ(delete_item(app.get(), TOP), 0);
  EXPECT_STREQ(app->top->id, MIDDLE);
  EXPECT_STREQ(app->top->next->id, BOTTOM);
  EXPECT_STREQ(app->top->prev->id, BOTTOM);
  EXPECT_STREQ(app->top->next->next->id, MIDDLE);
  EXPECT_STREQ(app->top->next->prev->id, MIDDLE);
}

TEST(AppTest, delete_item_middle) {
  auto app = allocate(new_app(TEST_APP, Alt));

  EXPECT_EQ(add_item(app.get(), BOTTOM), 0);
  EXPECT_EQ(add_item(app.get(), MIDDLE), 0);
  EXPECT_EQ(add_item(app.get(), TOP), 0);

  EXPECT_EQ(delete_item(app.get(), MIDDLE), 0);
  EXPECT_STREQ(app->top->id, TOP);
  EXPECT_STREQ(app->top->next->id, BOTTOM);
  EXPECT_STREQ(app->top->prev->id, BOTTOM);
  EXPECT_STREQ(app->top->next->next->id, TOP);
  EXPECT_STREQ(app->top->next->prev->id, TOP);
}

TEST(AppTest, delete_item_bottom) {
  auto app = allocate(new_app(TEST_APP, Alt));

  EXPECT_EQ(add_item(app.get(), BOTTOM), 0);
  EXPECT_EQ(add_item(app.get(), MIDDLE), 0);
  EXPECT_EQ(add_item(app.get(), TOP), 0);

  EXPECT_EQ(delete_item(app.get(), BOTTOM), 0);
  EXPECT_STREQ(app->top->id, TOP);
  EXPECT_STREQ(app->top->next->id, MIDDLE);
  EXPECT_STREQ(app->top->prev->id, MIDDLE);
  EXPECT_STREQ(app->top->next->next->id, TOP);
  EXPECT_STREQ(app->top->next->prev->id, TOP);
}
