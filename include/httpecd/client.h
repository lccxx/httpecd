// created by lich 3/15/2020, no copyright no copyleft

#ifndef HTTPECD_CLIENT_H_
#define HTTPECD_CLIENT_H_

#include <netinet/in.h>
#include <sys/epoll.h>

#include <string>
#include <vector>
#include <map>

namespace httpecd::client {

int init_sockaddr_in_ipv4(const char *host, int port, struct sockaddr_in *dest);

struct Request {
  const char *host;
  int port;
  const char *data;
};

class Batch {
 public:
  static constexpr int MAX_POLL = 1000;

  void add(const Request &request);

  void run();

 private:
  std::map<int, int> epoll_sockets;
};

}  // httpecd::client

#endif //HTTPECD_CLIENT_H_
