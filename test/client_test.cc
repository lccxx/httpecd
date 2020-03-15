// created by lich 2/24/2020, no copyright no copyleft

#include "gtest/gtest.h"

#include "httpecd/version.h"

TEST(RandomTest, Create) {  // NOLINT

  EXPECT_EQ(1, 1);
  EXPECT_EQ(HTTPECD_VERSION_MAJOR, 0);
  EXPECT_EQ(HTTPECD_VERSION_MINOR, 1);

}