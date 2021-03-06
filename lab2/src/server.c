#include <arpa/inet.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>

#include "utils.h"

static void print_usage()
{
    printf(
        "Usage: server [FILE]\n"
        "    File: the file to be transfered\n");
    exit(-1);
}

int main(int argc, char *argv[])
{
    if (argc < 2)
        print_usage();

    // open UDP socket
    printf("Opening a datagram socket...");

    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        printf("error\n");
        perror("[Error] open socket");
        exit(-1);
    }
    printf("OK\n");

    struct sockaddr_in gaddr;
    memset(&gaddr, 0, sizeof(gaddr));
    gaddr.sin_family = AF_INET;
    gaddr.sin_addr.s_addr = inet_addr("226.1.1.1");
    gaddr.sin_port = htons(8083);

    printf("Setting local interface...");

    struct in_addr local_interface = {.s_addr = inet_addr("127.0.0.1")};
    if (setsockopt(sockfd, IPPROTO_IP, IP_MULTICAST_IF,
                   (char *) &local_interface, sizeof(local_interface)) < 0) {
        printf("error\n");
        perror("[Error] setsockopt IP_MULTICAST_IF");
        exit(-1);
    } else
        printf("OK\n");

    int fd = open(argv[1], O_RDONLY);
    if (fd < 0) {
        perror("[Error] fail to open the file\n");
        exit(-1);
    }

    // read file info into buf
    DEBUG("read file info: %s\n", argv[1]);
    file_info_t info;
    read_info(&info, argv[1]);

    printf("Sending datagram packets...");
    DEBUG("send file info\n");
    sendto(sockfd, &info, sizeof(info), 0, (struct sockaddr *) &gaddr,
           sizeof(gaddr));

    // transfer the file
    DEBUG("transfer file from server\n");
    int ret;
    char buf[BUF_SIZE];
    while ((ret = read(fd, buf, BUF_SIZE))) {
        // DEBUG("test content: %s\n", buf);
        sendto(sockfd, buf, ret, 0, (struct sockaddr *) &gaddr, sizeof(gaddr));
    }

    sendto(sockfd, UDP_ACK, 4, 0, (struct sockaddr *) &gaddr, sizeof(gaddr));

    printf("OK\n");
    print_file_size(info.size);

    // cleanup fds
    close(fd);
    close(sockfd);

    return 0;
}
