#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

void error(const char *msg) {
  perror(msg);
  exit(1);
}

int main() {
  int server_fd;
  if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    error("Cooked");
  }
  struct sockaddr_in addr;
  memset(&addr, 0, sizeof(struct sockaddr_in));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = INADDR_ANY;
  addr.sin_port = htons(8080);

  if ((bind(server_fd, (const struct sockaddr *)&addr,
            sizeof(struct sockaddr_in))) < 0) {
    error("Double cooked");
  }

  if ((listen(server_fd, 3)) < 0) {
    error("Guh");
  }

  struct sockaddr_in incoming_addr;
  memset(&incoming_addr, 0, sizeof(struct sockaddr_in));
  int addrlen = sizeof(struct sockaddr_in);
  int incoming_fd;
  if ((incoming_fd = accept(server_fd, (struct sockaddr *)&incoming_addr,
                            (socklen_t *)&addrlen) < 0)) {
    error("Tripple cooked");
  }
  printf("Incoming req from %s\n", inet_ntoa(incoming_addr.sin_addr));
}
