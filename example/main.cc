// created by lich 2/24/2020, no copyright no copyleft

#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <strings.h>

#include <deque>
#include <thread>

class Coroutine {
 public:
  static constexpr int MAX_QUE_SIZE = 99;

  int64_t sum = 0;

  Coroutine() { this->max_sum_ = 1e+8; }
  explicit Coroutine(uint64_t max_sum) { this->max_sum_ = max_sum; }

  void producer() {
    while (sum < max_sum_) {
      while (que.size() < MAX_QUE_SIZE) {
        que.push_back('a');
        sum++;
      }
      consumer();
    }
  }

  void consumer() {
    while (!que.empty())
      que.pop_front();
  }

 private:
  std::deque<char> que;
  uint64_t max_sum_;
};

int init_sockaddr_in_ipv4(const std::string &host, int port, struct sockaddr_in *dest) {
  bzero(dest, sizeof(*dest));
  dest->sin_family = AF_INET;
  dest->sin_port = htons(port);
  if (inet_pton(AF_INET, host.c_str(), &((*dest).sin_addr.s_addr)) == 0) {
    perror(host.c_str());
    return -1;
  }
  return 200;
}

int main(int argc, char **args) {
  int epollfd;
  if ((epollfd = epoll_create1(0)) == -1) {
    perror("epoll_create1");
    exit(EXIT_FAILURE);
  }

  int sockfd;
  if ((sockfd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0)) < 0) {
    perror("socket");
    exit(errno);
  }

  struct epoll_event event{};
  event.events = EPOLLIN;
  event.data.fd = sockfd;
  if (epoll_ctl(epollfd, EPOLL_CTL_ADD, sockfd, &event) == -1) {
    perror("epoll_ctl: sockfd");
    exit(EXIT_FAILURE);
  }

  constexpr int TIMEOUT_MS = 5000;
  std::array<struct epoll_event, 1024> events{};
  int num_ready = 0;

  struct sockaddr_in dest{};
  if (init_sockaddr_in_ipv4("127.0.0.1", 80, &dest) != 200) {
    exit(errno);
  }

  if (connect(sockfd, (struct sockaddr *) &dest, sizeof(dest)) != 0) {
    if (errno != EINPROGRESS) {
      perror("Connect ");
      exit(errno);
    }
  }

  printf("to wait epollfd: %d\n", epollfd);
  num_ready = epoll_wait(epollfd, events.data(), events.size(), TIMEOUT_MS);
  printf("num_ready: %d\n", num_ready);
  if (num_ready == -1) {
    perror("epoll_wait");
    exit(EXIT_FAILURE);
  }
  for (int i = 0; i < num_ready; ++i) {
    printf("Socket %d event, connect\n", events[i].data.fd);
  }

  std::array<Coroutine, 1> coroutines = {};

  for (auto &coroutine : coroutines)
    std::thread([&coroutine] {
      coroutine.producer();
    }).detach();

  for (int64_t old_sum = 0;;) {
    std::this_thread::sleep_for(std::chrono::seconds(1));
    uint64_t total = 0;
    for (const auto &coroutine : coroutines) total += coroutine.sum;
    if (total - old_sum == 0) break;
    printf("per_second: %ld, total: %ld\n", (total - old_sum), total);
    old_sum = total;
  }

  return 0;
}
