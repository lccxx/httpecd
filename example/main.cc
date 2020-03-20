// created by lich 2/24/2020, no copyright no copyleft

#include <thread>
#include <vector>

#include "httpecd/client.h"

int main(int argc, char **args) {
  std::array<uint64_t, 6> sums = {0};      // thread number
  constexpr uint64_t MAX_REQUESTS = 1000000;  // per thread request number

  for (auto &sum : sums) {
    std::thread([&sum, &MAX_REQUESTS] {
      httpecd::Client batch = {};
      batch.connect_host("127.0.0.1", 80);

      constexpr size_t BUFFER_SIZE = 8192;
      char buffer[BUFFER_SIZE];

      for (sum = 0; sum < MAX_REQUESTS; ++sum) {
        std::string data = "GET /?";
        data.append(std::to_string(sum))
            .append(" HTTP/1.1\r\nHost: 127.0.0.1\r\n\r\n");

        batch.send_request(data.c_str(), data.size());
        batch.wait_response(buffer, BUFFER_SIZE);
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
