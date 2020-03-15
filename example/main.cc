// created by lich 2/24/2020, no copyright no copyleft

#include <thread>
#include <vector>

#include "httpecd/client.h"

int main(int argc, char **args) {
  std::array<uint64_t, 1> sums = {0};      // thread number
  constexpr uint64_t MAX_REQUEST = 10000;  // per thread request number

  for (auto &sum : sums) {
    std::thread([&sum, &MAX_REQUEST] {
      constexpr int EPOLL_SIZE = 1024;

      const char host[] = "127.0.0.1";
      const int port = 80;
      const std::string req = "GET / HTTP/1.1\r\nHost: 127.0.0.1\r\n\r\n";

      std::vector<int> epolls(EPOLL_SIZE);
      std::vector<struct epoll_event> events(EPOLL_SIZE);

      for (sum = 0; sum < MAX_REQUEST; sum += EPOLL_SIZE) {
        httpecd::client::batch_request(host, port, req, epolls, events);
      }
    }).detach();
  }

  for (int64_t old_sum = 0;;) {
    std::this_thread::sleep_for(std::chrono::seconds(1));
    uint64_t total = 0;
    for (const auto sum : sums) total += sum;
    printf("per_second: %ld, total: %ld\n", (total - old_sum), total);
    if (total - old_sum == 0) break;
    old_sum = total;
  }

  return 0;
}
