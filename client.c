#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

const char *payload = "GET / HTTP/1.1\r\nUser-Agent: beepboop/1.1.1\r\nHost: example.com\r\n\r\n";

void usage(char *argv0) {
    fprintf(stderr, "usage: %s example.com 8080", argv0);
}

struct addr *get_addr(struct addrinfo *info) {
    switch (info->ai_family) {
    case AF_INET:
        return (struct addr *)(&((struct sockaddr_in *)info->ai_addr)->sin_addr);
    case AF_INET6:
        return (struct addr *)(&((struct sockaddr_in6 *)info->ai_addr)->sin6_addr);
    default:
        fprintf(stderr, "fatal: invalid address family type\n");
        exit(1);
    }
}

int request_sockaddr(struct sockaddr *addr, socklen_t addrlen, char *buf, size_t buflen) {
    int sockfd;
    ssize_t n;
    // Create socket
    if ((sockfd = socket(addr->sa_family, SOCK_STREAM, 0)) < 0) {
        perror("failed to create socket");
        return -1;
    }
    // Connect to addr
    if (connect(sockfd, addr, addrlen) < 0) {
        perror("failed to connect to addr");
        close(sockfd);
        return -2;
    }
    // Send init message
    if ((n = send(sockfd, payload, strlen(payload), 0)) < 0) {
        perror("failed to send from socket");
        close(sockfd);
        return -3;
    }
    // Read from addr
    if ((n = recv(sockfd, buf, buflen - 1, 0)) < 0) {
        perror("failed to read from socket");
        close(sockfd);
        return -4;
    }
    if (n == 0) {
        printf("Connection closed by remote.\n");
    }
    buf[n] = '\0';
    close(sockfd);
    return 0;
}

int request(char *addr, char *port, char *buf, size_t buflen) {
    struct addrinfo *servinfo, *p;
    struct addrinfo hints = {.ai_socktype = SOCK_STREAM, 0};
    char s[INET6_ADDRSTRLEN];

    // Resolve the addr into some addresses
    if ((getaddrinfo(addr, port, &hints, &servinfo)) < 0) {
        perror("failed to get addr info");
        return -1;
    }

    // Try all valid addrs
    for (p = servinfo; p != NULL; p = p->ai_next) {
        if (inet_ntop(p->ai_family, get_addr(p), s, sizeof(s)) == NULL) {
            perror("failed to convert addr");
            continue;
        }
        if (request_sockaddr(p->ai_addr, p->ai_addrlen, buf, buflen) < 0) {
            continue;
        }
        break;
    }

    // Print and clean up
    if (p == NULL) {
        fprintf(stderr, "Failed to connect to %s:%s\n", addr, port);
        return -2;
    } else {
        printf("Successfully read from %s!\n", s);
    }
    freeaddrinfo(servinfo);
    return 0;
}

int main(int argc, char **argv) {
    if (argc != 3) {
        usage(argv[0]);
        exit(1);
    }

    char buf[10240];
    if ((request(argv[1], argv[2], buf, sizeof(buf))) < 0) {
        exit(2);
    }
    printf("Buffer content:\n%s\n", buf);
}
