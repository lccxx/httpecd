// created by lich 3/15/2020, no copyright no copyleft

#ifndef HTTPECD_CLIENT_H_
#define HTTPECD_CLIENT_H_

#include <netinet/in.h>
#include <sys/epoll.h>

#include <string>
#include <vector>

namespace httpecd::client {

int init_sockaddr_in_ipv4(const std::string &host, int port, struct sockaddr_in *dest);

void batch_request(const char *host, int port, const std::string &request,
                   std::vector<int> &epolls, std::vector<struct epoll_event> &events);

}  // httpecd::client

#endif //HTTPECD_CLIENT_H_
