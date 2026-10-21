#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 8080

socklen_t addr_in_size = sizeof(struct sockaddr_in);

void error(const char *msg) {
  perror(msg);
  exit(1);
}

int main() {
  int socket_fd;
  struct sockaddr_in server_addr = {0};
  if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    error("Failed to create socket");
  }
  server_addr.sin_port = htons(PORT);
  server_addr.sin_family = AF_INET;
  if (inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr) <= 0) {
    error("Failed write server addr");
  }

  if ((connect(socket_fd, (const struct sockaddr *)&server_addr,
               addr_in_size)) < 0) {
    error("Failed to connect to server");
  }

  char *msg = "hello!!";
  send(socket_fd, msg, strlen(msg), 0);
  printf("Sent !:)\n");
  close(socket_fd);
}
