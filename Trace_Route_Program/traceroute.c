#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip_icmp.h>
#include <netinet/udp.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PACKET_SIZE 4096
#define MAX_HOPS 30
#define PORT_NUM 33434

unsigned short checksum(void *b, int len);
void print_ip(struct sockaddr_in *addr);

int main(int argc, char *argv[]) {
    int sockfd, ttl, ret, addr_len, i;
    struct sockaddr_in dest_addr, recv_addr;
    struct icmp *icmp_packet;
    struct ip *ip_packet;
    unsigned char *packet = (unsigned char *)malloc(PACKET_SIZE);
    struct timeval timeout = {3, 0}; // 3 seconds timeout

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <hostname>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Resolve the destination address
    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(PORT_NUM);
    if (inet_pton(AF_INET, argv[1], &dest_addr.sin_addr) <= 0) {
        perror("inet_pton error");
        exit(EXIT_FAILURE);
    }

    // Create a raw socket
    if ((sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) < 0) {
        perror("socket error");
        exit(EXIT_FAILURE);
    }

    // Set the receive timeout for the socket
    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
        perror("setsockopt error");
        exit(EXIT_FAILURE);
    }

    printf("Tracing route to %s over a maximum of %d hops:\n", argv[1], MAX_HOPS);

    for (ttl = 1; ttl <= MAX_HOPS; ttl++) {
        // Set the TTL value for the packet
        if (setsockopt(sockfd, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl)) < 0) {
            perror("setsockopt error");
            exit(EXIT_FAILURE);
        }

        // Create a UDP packet
        memset(packet, 0, PACKET_SIZE);
        ip_packet = (struct ip *)packet;
        ip_packet->ip_hl = 5;
        ip_packet->ip_v = 4;
        ip_packet->ip_tos = 0;
        ip_packet->ip_len = htons(sizeof(struct ip) + sizeof(struct udphdr));
        ip_packet->ip_id = htons(0);
        ip_packet->ip_off = 0;
        ip_packet->ip_ttl = ttl;
        ip_packet->ip_p = IPPROTO_UDP;
        ip_packet->ip_sum = 0;
        ip_packet->ip_src.s_addr = INADDR_ANY;
        ip_packet->ip_dst = dest_addr.sin_addr;
        ip_packet->ip_sum = checksum(ip_packet, sizeof(struct ip));

        // Send the UDP packet
        if (sendto(sockfd, packet, sizeof(struct ip) + sizeof(struct udphdr), 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr)) < 0) {
            perror("sendto error");
            exit(EXIT_FAILURE);
        }

        // Receive the ICMP reply
        addr_len = sizeof(recv_addr);
        if ((ret = recvfrom(sockfd, packet, PACKET_SIZE, 0, (struct sockaddr *)&recv_addr, &addr_len)) < 0) {
            printf("%2d  *\n", ttl);
        } else {
            // Check if the received packet is an ICMP Time Exceeded message
            ip_packet = (struct ip *)packet;
            icmp_packet = (struct icmp *)(packet + (ip_packet->ip_hl << 2));
            if (icmp_packet->icmp_type == ICMP_TIMXCEED && icmp_packet->icmp_code == ICMP_TIMXCEED_INTRANS) {
                printf("%2d  ", ttl);
                print_ip(&recv_addr);
                printf("\n");
            } else if (icmp_packet->icmp_type == ICMP_UNREACH) {
                printf("%2d  Destination reached\n", ttl);
                break;
            }
        }
    }

    free(packet);
    close(sockfd);
    return 0;
}

unsigned short checksum(void *b, int len) {
    unsigned short *buf = b;
    unsigned int sum = 0;
    unsigned short result;

    for (sum = 0; len > 1; len -= 2)
        sum += *buf++;
    if (len == 1)
        sum += *(unsigned char *)buf;
    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    result = ~sum;
    return result;
}

void print_ip(struct sockaddr_in *addr) {
    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(addr->sin_addr), ip, INET_ADDRSTRLEN);
    printf("%s", ip);
}
