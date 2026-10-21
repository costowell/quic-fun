#include <arpa/inet.h>
#include <asm-generic/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define PORT 8080

socklen_t addr_in_size = sizeof(struct sockaddr_in);

void error(const char *msg) {
  perror(msg);
  exit(1);
}

int main() {
  int server_fd;
  struct sockaddr_in bind_addr;

  // Create socket
  if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    error("Failed to create socket");
  }

  // Setup bind address
  memset(&bind_addr, 0, sizeof(struct sockaddr_in));
  bind_addr.sin_family = AF_INET;
  bind_addr.sin_addr.s_addr = INADDR_ANY;
  bind_addr.sin_port = htons(PORT);

  // Allow address reuse
  int opt = 1;
  setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

  // Bind to bind address
  if ((bind(server_fd, (const struct sockaddr *)&bind_addr, addr_in_size)) <
      0) {
    error("Failed to bind");
  }

  // Start listening for connections on the socket
  if ((listen(server_fd, 3)) < 0) {
    error("Failed to listen for new connections");
  }

  printf("Started listening on %s:%d\n", inet_ntoa(bind_addr.sin_addr), PORT);

  char buf[2048] = {0};
  struct sockaddr_in incoming_addr;
  memset(&incoming_addr, 0, sizeof(struct sockaddr_in));
  int incoming_fd;
  const char *msg = "HELLO!\n";
  while (1) {
    if ((incoming_fd = accept(server_fd, (struct sockaddr *)&incoming_addr,
                              &addr_in_size) < 0)) {
      close(server_fd);
      error("Failed to accept incoming connection");
    }
    printf("Incoming req from %s\n", inet_ntoa(incoming_addr.sin_addr));
    if (read(incoming_fd, buf, 2) < 0) {
      close(server_fd);
      error("Failed to read from socket");
    }
    printf("Data:\n%s\n", buf);
    if (send(incoming_fd, msg, strlen(msg), 0) < 0) {
      close(server_fd);
      error("Failed to send response");
    }
    close(incoming_fd);
  }
}
