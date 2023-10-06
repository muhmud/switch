#include "src.hpp"
#include <gtest/gtest.h>

#define TEST_APP "test"
#define SECOND_APP "second"
#define THIRD_APP "third"
#define FOURTH_APP "fourth"

#define TOP "top"
#define MIDDLE "middle"
#define BOTTOM "bottom"

TEST(AppKeymapTest, add_app) {
  clear_apps();

  int modcode = Shift_L;
  EXPECT_EQ(add_app(TEST_APP, modcode), 0);

  auto node = find_apps_by_modcode(modcode);
  EXPECT_TRUE(node);
  EXPECT_STREQ(node->app->name, TEST_APP);
  EXPECT_EQ(node->app->modcode, modcode);
}

TEST(AppKeymapTest, add_app_mods_lr) {
  clear_apps();

  int modcode = Super;
  EXPECT_EQ(add_app(TEST_APP, modcode), 0);

  auto left_node = find_apps_by_modcode(Super_L);
  EXPECT_TRUE(left_node);
  EXPECT_STREQ(left_node->app->name, TEST_APP);
  EXPECT_EQ(left_node->app->modcode, modcode);

  auto right_node = find_apps_by_modcode(Super_R);
  EXPECT_TRUE(right_node);
  EXPECT_STREQ(right_node->app->name, TEST_APP);
  EXPECT_EQ(right_node->app->modcode, modcode);
}

TEST(AppKeymapTest, add_app_multi_for_mod) {
  clear_apps();

  int modcode = Shift_L;
  EXPECT_EQ(add_app(TEST_APP, modcode), 0);
  EXPECT_EQ(add_app(SECOND_APP, modcode), 0);

  auto node = find_apps_by_modcode(modcode);
  EXPECT_TRUE(node);
  EXPECT_STREQ(node->app->name, SECOND_APP);
  EXPECT_EQ(node->app->modcode, modcode);

  node = node->next;
  EXPECT_TRUE(node);
  EXPECT_STREQ(node->app->name, TEST_APP);
  EXPECT_EQ(node->app->modcode, modcode);
  EXPECT_FALSE(node->next);
}

TEST(AppKeymapTest, add_app_invalid_name) {
  clear_apps();

  EXPECT_EQ(add_app("", Alt), -1);
}

TEST(AppKeymapTest, add_app_already_exists) {
  clear_apps();

  EXPECT_EQ(add_app(TEST_APP, Alt), 0);
  EXPECT_EQ(add_app(TEST_APP, Shift), -1);
}

TEST(AppKeymapTest, find_app) {
  clear_apps();
  EXPECT_EQ(add_app(TEST_APP, Alt), 0);

  auto app = find_app(TEST_APP);
  EXPECT_TRUE(app);
  EXPECT_STREQ(app->name, TEST_APP);
  EXPECT_EQ(app->modcode, Alt);
  EXPECT_FALSE(find_app(SECOND_APP));

  EXPECT_EQ(add_app(SECOND_APP, Control), 0);
  app = find_app(SECOND_APP);
  EXPECT_TRUE(app);
  EXPECT_STREQ(app->name, SECOND_APP);
  EXPECT_EQ(app->modcode, Control);
  EXPECT_TRUE(find_app(TEST_APP));
  EXPECT_FALSE(find_app(THIRD_APP));
}

TEST(AppKeymapTest, find_app_multi_for_mod) {
  clear_apps();
  EXPECT_EQ(add_app(TEST_APP, Alt), 0);
  EXPECT_EQ(add_app(SECOND_APP, Alt), 0);

  auto app = find_app(TEST_APP);
  EXPECT_TRUE(app);
  EXPECT_STREQ(app->name, TEST_APP);
  EXPECT_EQ(app->modcode, Alt);
  EXPECT_FALSE(find_app(THIRD_APP));

  app = find_app(SECOND_APP);
  EXPECT_TRUE(app);
  EXPECT_STREQ(app->name, SECOND_APP);
  EXPECT_EQ(app->modcode, Alt);
  EXPECT_TRUE(find_app(TEST_APP));
  EXPECT_FALSE(find_app(THIRD_APP));
}

TEST(AppKeymapTest, find_app_invalid_name) {
  clear_apps();
  EXPECT_EQ(add_app(TEST_APP, Alt), 0);

  EXPECT_FALSE(find_app(""));
  EXPECT_FALSE(find_app(NULL));
}

TEST(AppKeymapTest, delete_app) {
  clear_apps();

  EXPECT_EQ(add_app(TEST_APP, Alt), 0);
  EXPECT_EQ(add_app(SECOND_APP, Control), 0);
  EXPECT_EQ(delete_app(THIRD_APP), -1);

  EXPECT_EQ(delete_app(TEST_APP), 0);
  EXPECT_FALSE(find_app(TEST_APP));
  EXPECT_TRUE(find_app(SECOND_APP));
}

TEST(AppKeymapTest, delete_app_multi_for_mod_first) {
  clear_apps();

  EXPECT_EQ(add_app(TEST_APP, Alt), 0);
  EXPECT_EQ(add_app(SECOND_APP, Alt), 0);
  EXPECT_EQ(add_app(THIRD_APP, Alt), 0);

  EXPECT_EQ(delete_app(TEST_APP), 0);
  auto node = find_apps_by_modcode(Alt_L);
  EXPECT_TRUE(node);
  EXPECT_STREQ(node->app->name, THIRD_APP);
  EXPECT_EQ(node->app->modcode, Alt);

  node = node->next;
  EXPECT_TRUE(node);
  EXPECT_STREQ(node->app->name, SECOND_APP);
  EXPECT_EQ(node->app->modcode, Alt);
  EXPECT_FALSE(node->next);
}

TEST(AppKeymapTest, delete_app_multi_for_mod_middle) {
  clear_apps();

  EXPECT_EQ(add_app(TEST_APP, Alt), 0);
  EXPECT_EQ(add_app(SECOND_APP, Alt), 0);
  EXPECT_EQ(add_app(THIRD_APP, Alt), 0);

  EXPECT_EQ(delete_app(SECOND_APP), 0);
  auto node = find_apps_by_modcode(Alt_L);
  EXPECT_TRUE(node);
  EXPECT_STREQ(node->app->name, THIRD_APP);
  EXPECT_EQ(node->app->modcode, Alt);

  node = node->next;
  EXPECT_TRUE(node);
  EXPECT_STREQ(node->app->name, TEST_APP);
  EXPECT_EQ(node->app->modcode, Alt);
  EXPECT_FALSE(node->next);
}

TEST(AppKeymapTest, delete_app_multi_for_mod_third) {
  clear_apps();

  EXPECT_EQ(add_app(TEST_APP, Alt), 0);
  EXPECT_EQ(add_app(SECOND_APP, Alt), 0);
  EXPECT_EQ(add_app(THIRD_APP, Alt), 0);

  EXPECT_EQ(delete_app(THIRD_APP), 0);
  auto node = find_apps_by_modcode(Alt_L);
  EXPECT_TRUE(node);
  EXPECT_STREQ(node->app->name, SECOND_APP);
  EXPECT_EQ(node->app->modcode, Alt);

  node = node->next;
  EXPECT_TRUE(node);
  EXPECT_STREQ(node->app->name, TEST_APP);
  EXPECT_EQ(node->app->modcode, Alt);
  EXPECT_FALSE(node->next);
}

TEST(AppKeymapTest, restart_app) {
  clear_apps();

  EXPECT_EQ(add_app(TEST_APP, Alt), 0);
  auto app = find_app(TEST_APP);
  EXPECT_TRUE(app);
  EXPECT_EQ(add_item(app, BOTTOM), 0);
  EXPECT_EQ(add_item(app, MIDDLE), 0);
  EXPECT_EQ(add_item(app, TOP), 0);
  EXPECT_TRUE(app->top);

  restart_app(TEST_APP);
  app = find_app(TEST_APP);
  EXPECT_TRUE(app);
  EXPECT_FALSE(app->top);
}
