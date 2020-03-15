// created by lich 2/24/2020, no copyright no copyleft

#include "httpecd/client.h"

#include <arpa/inet.h>
#include <sys/socket.h>

#include <strings.h>
#include <zconf.h>

namespace httpecd::client {

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

void batch_request(const char *host, int port, const std::string &request,
                   std::vector<int> &epolls, std::vector<struct epoll_event> &events) {
  for (int &epollfd : epolls) {
    if ((epollfd = epoll_create1(0)) == -1) {
      perror("epoll_create1");
      exit(EXIT_FAILURE);
    }

    int sockfd;
    if ((sockfd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0)) < 0) {
      perror("socket");
      exit(errno);
    }

    struct epoll_event event = {};
    event.events = EPOLLIN;
    event.data.fd = sockfd;
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, sockfd, &event) == -1) {
      perror("epoll_ctl: ");
      exit(EXIT_FAILURE);
    }

    struct sockaddr_in dest{};
    if (init_sockaddr_in_ipv4(host, port, &dest) != 200) {
      exit(errno);
    }

    if (connect(sockfd, (struct sockaddr *) &dest, sizeof(dest)) != 0) {
      if (errno != EINPROGRESS) {
        perror(std::string("Connect ").append(host).append(":").append(std::to_string(port)).c_str());
        exit(errno);
      }
    }

    if (send(sockfd, request.c_str(), request.size(), MSG_DONTWAIT) == -1) {
      perror("send");
      exit(EXIT_FAILURE);
    }
  }

  constexpr int MAX_BUFF = 16 * 1024;
  for (int epollfd : epolls) {
    int num_ready = epoll_wait(epollfd, events.data(), events.size(), 5000 /* TIMEOUT_MS */);
    if (num_ready < 1) {
      perror("epoll_wait");
      exit(EXIT_FAILURE);
    }
    for (int i = 0; i < num_ready; ++i) {
      if (events[i].events) {
        char buffer[MAX_BUFF] = {0};
        if (recv(events[i].data.fd, buffer, 16 * 1024, MSG_DONTWAIT) == -1) {
          perror("recv");
          exit(EXIT_FAILURE);
        }
        if (epoll_ctl(epollfd, EPOLL_CTL_DEL, events[i].data.fd, &events[i]) == -1) {
          perror("epoll_ctl del: ");
          exit(EXIT_FAILURE);
        }
        close(events[i].data.fd);
      }
    }
  }
}

}  // httpecd::client
