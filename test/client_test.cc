// created by lich 2/24/2020, no copyright no copyleft

#include "gtest/gtest.h"

#include "httpecd/version.h"
#include "httpecd/client.h"

TEST(RandomTest, Create) {  // NOLINT

  EXPECT_EQ(1, 1);
  EXPECT_EQ(HTTPECD_VERSION_MAJOR, 0);
  EXPECT_EQ(HTTPECD_VERSION_MINOR, 1);

  char host[] = "127.0.0.1";
  int port = 80;
  struct sockaddr_in dest = {};
  EXPECT_EQ(httpecd::client::init_sockaddr_in_ipv4(host, port, &dest), 200);
  EXPECT_EQ(dest.sin_port, 20480);
  EXPECT_EQ(dest.sin_addr.s_addr, 0x100007F);
}