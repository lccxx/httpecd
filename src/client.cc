// created by lich 2/24/2020, no copyright no copyleft

#include "httpecd/client.h"

#include <arpa/inet.h>
#include <sys/socket.h>

#include <strings.h>

#include <string>

namespace httpecd {

int init_sockaddr_in_ipv4(const char *host, int port, struct sockaddr_in *dest) {
  bzero(dest, sizeof(*dest));
  dest->sin_family = AF_INET;
  dest->sin_port = htons(port);
  if (inet_pton(AF_INET, host, &((*dest).sin_addr.s_addr)) == 0) {
    perror(host);
    return -1;
  }
  return 200;
}

int Client::connect_host(const char *host, int port) {
  int epollfd;
  if ((epollfd = epoll_create1(0)) == -1) {
    perror("epoll_create1");
    return EXIT_FAILURE;
  }

  int sockfd;
  if ((sockfd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0)) < 0) {
    perror("socket");
    return errno;
  }

  struct epoll_event event = {};
  event.events = EPOLLIN;
  if (epoll_ctl(epollfd, EPOLL_CTL_ADD, sockfd, &event) == -1) {
    perror("epoll_ctl: ");
    return EXIT_FAILURE;
  }

  struct sockaddr_in dest{};
  if (init_sockaddr_in_ipv4(host, port, &dest) != 200) {
    return errno;
  }

  if (connect(sockfd, (struct sockaddr *) &dest, sizeof(dest)) != 0) {
    if (errno != EINPROGRESS) {
      perror(std::string("Connect ").append(host).append(":").append(std::to_string(port)).c_str());
      return errno;
    }
  }

  curr_epollfd = epollfd;
  curr_sockfd = sockfd;
  return 0;
}

int Client::send_request(const char *data, size_t size) {
  if (send(curr_sockfd, data, size, MSG_DONTWAIT) == -1) {
    perror("send");
    return EXIT_FAILURE;
  }

  return 0;
}

int Client::wait_response(char *buffer, size_t buffer_size) {
  if (epoll_wait(curr_epollfd, events, MAX_EVENTS, TIME_OUT_MS) < 1) {
    perror("epoll_wait");
    return -1;
  }

  size_t response_size;
  if ((response_size = recv(curr_sockfd, buffer, buffer_size, MSG_DONTWAIT)) == -1) {
    perror("recv");
    return -1;
  }
  return response_size;
}

}  // namespace httpecd
