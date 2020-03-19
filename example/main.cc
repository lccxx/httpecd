// created by lich 2/24/2020, no copyright no copyleft

#include <thread>
#include <vector>

#include "httpecd/client.h"

int main(int argc, char **args) {
  std::array<uint64_t, 1> sums = {0};      // thread number
  constexpr uint64_t MAX_REQUEST = 3000;  // per thread request number

  for (auto &sum : sums) {
    std::thread([&sum, &MAX_REQUEST] {
      const char host[] = "127.0.0.1";
      const int port = 80;

      httpecd::client::Batch batch = {};

      for (sum = 0; sum < MAX_REQUEST; sum += httpecd::client::Batch::MAX_POLL) {
        for (int i = 0; i < httpecd::client::Batch::MAX_POLL; ++i) {
          std::string data = "GET /?";
          data.append(std::to_string(sum + i))
              .append(" HTTP/1.1\r\nHost: 127.0.0.1\r\n\r\n");

          batch.add({host, port, data.c_str()});
        }

        batch.run();
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
