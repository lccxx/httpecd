// created by lich 2/24/2020, no copyright no copyleft

#include <thread>
#include <vector>

#include "httpecd/client.h"

int main(int argc, char **args) {
  constexpr int EPOLL_SIZE = 1024;

  const char host[] = "127.0.0.1";
  const int port = 80;
  const std::string req = "GET / HTTP/1.1\r\nHost: 127.0.0.1\r\n\r\n";

  std::vector<int> epolls(EPOLL_SIZE);
  std::vector<struct epoll_event> events(EPOLL_SIZE);

  httpecd::client::batch_request(host, port, req, epolls, events);

  return 0;
}
