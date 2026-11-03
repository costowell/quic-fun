#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

struct addr *get_addr(struct sockaddr *addr) {
    switch (addr->sa_family) {
    case AF_INET:
        return (struct addr *)(&((struct sockaddr_in *)addr)->sin_addr);
    case AF_INET6:
        return (struct addr *)(&((struct sockaddr_in6 *)addr)->sin6_addr);
    default:
        fprintf(stderr, "fatal: invalid address family type\n");
        exit(1);
    }
}

void usage(const char *argv0) {
    fprintf(stderr, "usage: %s 0.0.0.0 8080", argv0);
}

int main(int argc, const char *argv[]) {
    if (argc != 3) {
        usage(argv[0]);
    }

    const char *addr = argv[1];
    const char *port = argv[2];

    int sockfd;
    struct addrinfo *servinfo, *p;
    struct addrinfo hints = {.ai_socktype = SOCK_STREAM, 0};

    // Resolve the addr into some addresses
    if ((getaddrinfo(addr, port, &hints, &servinfo)) < 0) {
        perror("failed to get addr info");
        return -1;
    }

    for (p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
            perror("failed to create socket");
            continue;
        }
        int yes = 1;
        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) < 0) {
            perror("failed to set socket option");
            close(sockfd);
            continue;
        }
        if (bind(sockfd, p->ai_addr, p->ai_addrlen) < 0) {
            perror("failed to bind");
            close(sockfd);
            continue;
        }
        break;
    }

    freeaddrinfo(servinfo);

    if (p == NULL) {
        fprintf(stderr, "failed to bind on any address\n");
        return -1;
    }

    if (listen(sockfd, 5) < 0) {
        perror("failed to listen");
        close(sockfd);
        return -1;
    }

    while (1) {
        int clientfd;
        struct sockaddr_storage client_addr;
        socklen_t client_addr_size = sizeof(client_addr);
        char s[INET6_ADDRSTRLEN];
        if ((clientfd = accept(sockfd, (struct sockaddr *)&client_addr, &client_addr_size)) < 0) {
            perror("failed to accept");
            continue;
        }
        inet_ntop(client_addr.ss_family, get_addr((struct sockaddr *)&client_addr), s,
                  INET6_ADDRSTRLEN);
        printf("connection from %s\n", s);

        char buf[100];
        ssize_t n;
        if ((n = recv(clientfd, buf, sizeof(buf), 0)) < 0) {
            perror("failed to recv from peer");
            close(clientfd);
            continue;
        }
        buf[n] = '\0';
        printf("From %s: %s\n", s, buf);

        if (send(clientfd, "lol\n", 4, 0) < 0) {
            perror("failed to send to peer");
            close(clientfd);
            continue;
        }
        close(clientfd);
    }
}
