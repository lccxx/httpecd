// created by lich 3/15/2020, no copyright no copyleft

#ifndef HTTPECD_CLIENT_H_
#define HTTPECD_CLIENT_H_

#include <netinet/in.h>
#include <sys/epoll.h>

#include <vector>

namespace httpecd {

int init_sockaddr_in_ipv4(const char *host, int port, struct sockaddr_in *dest);

class Client {
 public:
  static constexpr int TIME_OUT_MS = 5000;

  int connect_host(const char *host, int port);

  int send_request(const char *data, size_t size);

  int wait_response(char *buffer, size_t size);

 private:
  int curr_epollfd;
  int curr_sockfd;
  std::vector<struct epoll_event> events = std::vector<struct epoll_event>(1);
};

}  // httpecd

#endif //HTTPECD_CLIENT_H_
