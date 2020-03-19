// created by lich 2/24/2020, no copyright no copyleft

#include "httpecd/client.h"

#include <arpa/inet.h>
#include <sys/socket.h>

#include <strings.h>
#include <zconf.h>

#include <cstring>
#include <map>

namespace httpecd::client {

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

void Batch::add(const Request &request) {
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
  epoll_sockets[epollfd] = sockfd;

  struct epoll_event event = {};
  event.events = EPOLLIN;
  if (epoll_ctl(epollfd, EPOLL_CTL_ADD, sockfd, &event) == -1) {
    perror("epoll_ctl: ");
    exit(EXIT_FAILURE);
  }

  struct sockaddr_in dest{};
  if (init_sockaddr_in_ipv4(request.host, request.port, &dest) != 200) {
    exit(errno);
  }

  if (connect(sockfd, (struct sockaddr *) &dest, sizeof(dest)) != 0) {
    if (errno != EINPROGRESS) {
      perror(std::string("Connect ").append(request.host).append(":").append(std::to_string(request.port)).c_str());
      exit(errno);
    }
  }

  if (send(sockfd, request.data, strlen(request.data), MSG_DONTWAIT) == -1) {
    perror("send");
    // exit(EXIT_FAILURE);
  }
}

void Batch::run() {
  std::vector<struct epoll_event> events(epoll_sockets.size());
  constexpr int MAX_BUFF = 16 * 1024;
  for (auto epoll_socket = epoll_sockets.begin(); epoll_socket != epoll_sockets.end(); epoll_socket = epoll_sockets.erase(epoll_socket)) {
    int num_ready = epoll_wait(epoll_socket->first, events.data(), events.size(), 5000 /* TIMEOUT_MS */);
    if (num_ready < 1) {
      perror("epoll_wait");
      exit(EXIT_FAILURE);
    }
    for (int i = 0; i < num_ready; ++i) {
      if (events[i].events) {
        char buffer[MAX_BUFF] = {0};
        if (recv(epoll_socket->second, buffer, 16 * 1024, MSG_DONTWAIT) == -1) {
          perror("recv");
          exit(EXIT_FAILURE);
        }
        if (epoll_ctl(epoll_socket->first, EPOLL_CTL_DEL, epoll_socket->second, &events[i]) == -1) {
          perror("epoll_ctl del: ");
          exit(EXIT_FAILURE);
        }
        close(events[i].data.fd);
      }
    }
  }
}

}  // namespace httpecd::client
