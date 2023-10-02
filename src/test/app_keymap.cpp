#include "src.hpp"
#include <gtest/gtest.h>

#define TEST_APP "test"
#define SECOND_APP "second"
#define THIRD_APP "third"
#define FOURTH_APP "fourth"

TEST(AppKeymap, add_app) {
  clear_apps();

  int modcode = Shift_L;
  EXPECT_EQ(add_app(TEST_APP, modcode), 0);

  auto app = find_app_by_modcode(modcode);
  EXPECT_TRUE(app);
  EXPECT_STREQ(app->name, TEST_APP);
  EXPECT_EQ(app->modcode, modcode);
}

TEST(AppKeymap, add_app_mods_lr) {
  clear_apps();

  int modcode = Super;
  EXPECT_EQ(add_app(TEST_APP, modcode), 0);

  auto app = find_app_by_modcode(Super_L);
  EXPECT_TRUE(app);
  EXPECT_STREQ(app->name, TEST_APP);
  EXPECT_EQ(app->modcode, modcode);

  app = find_app_by_modcode(Super_R);
  EXPECT_TRUE(app);
  EXPECT_STREQ(app->name, TEST_APP);
  EXPECT_EQ(app->modcode, modcode);
}

TEST(AppKeymap, add_app_invalid_name) {
  clear_apps();

  EXPECT_EQ(add_app("", Alt), -1);
}

TEST(AppKeymap, add_app_already_exists) {
  clear_apps();

  EXPECT_EQ(add_app(TEST_APP, Alt), 0);
  EXPECT_EQ(add_app(TEST_APP, Shift), -1);
}

TEST(AppKeymap, add_app_modcode_already_mapped) {
  clear_apps();

  EXPECT_EQ(add_app(TEST_APP, Alt_L), 0);
  EXPECT_EQ(add_app(SECOND_APP, Alt_L), -1);
  EXPECT_EQ(add_app(SECOND_APP, Alt_R), 0);

  EXPECT_EQ(add_app(THIRD_APP, Alt), -1);
  EXPECT_EQ(add_app(THIRD_APP, Control), 0);

  EXPECT_EQ(add_app(FOURTH_APP, Control_L), -1);
  EXPECT_EQ(add_app(FOURTH_APP, Control_R), -1);
}

TEST(AppKeymap, find_app) {
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

TEST(AppKeymap, find_app_invalid_name) {
  clear_apps();
  EXPECT_EQ(add_app(TEST_APP, Alt), 0);

  EXPECT_FALSE(find_app(""));
  EXPECT_FALSE(find_app(NULL));
}

TEST(AppKeymap, delete_app) {
  clear_apps();

  EXPECT_EQ(add_app(TEST_APP, Alt), 0);
  EXPECT_EQ(add_app(SECOND_APP, Control), 0);
  EXPECT_EQ(delete_app(THIRD_APP), -1);

  EXPECT_EQ(delete_app(TEST_APP), 0);
  EXPECT_FALSE(find_app(TEST_APP));
  EXPECT_TRUE(find_app(SECOND_APP));
}
