#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <arpa/inet.h>
#include <unistd.h>

void process_packet(unsigned char* buffer, int size);
void print_ip_header(unsigned char* buffer, int size);
void print_tcp_packet(unsigned char* buffer, int size);
void print_udp_packet(unsigned char* buffer, int size);
void print_icmp_packet(unsigned char* buffer, int size);

int main() {
    int sockfd;
    struct sockaddr saddr;
    unsigned char* buffer = (unsigned char *)malloc(65536);
    socklen_t saddr_len = sizeof(saddr);

    // Create a raw socket that shall sniff
    sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_TCP);
    if (sockfd < 0) {
        perror("Socket error");
        exit(EXIT_FAILURE);
    }

    while (1) {
        // Receive a packet
        int data_size = recvfrom(sockfd, buffer, 65536, 0, &saddr, &saddr_len);
        if (data_size < 0) {
            perror("Recvfrom error");
            exit(EXIT_FAILURE);
        }

        // Process the packet
        process_packet(buffer, data_size);
    }

    close(sockfd);
    free(buffer);
    return 0;
}

void process_packet(unsigned char* buffer, int size) {
    // Get the IP Header part of this packet
    struct iphdr *iph = (struct iphdr*)buffer;
    switch (iph->protocol) {
        case 1:  // ICMP Protocol
            print_icmp_packet(buffer, size);
            break;

        case 6:  // TCP Protocol
            print_tcp_packet(buffer, size);
            break;

        case 17: // UDP Protocol
            print_udp_packet(buffer, size);
            break;

        default: // Some other protocol
            break;
    }
}

void print_ip_header(unsigned char* buffer, int size) {
    struct sockaddr_in source;
    struct iphdr *iph = (struct iphdr*)buffer;
    memset(&source, 0, sizeof(source));
    source.sin_addr.s_addr = iph->saddr;

    printf("Source IP: %s\n", inet_ntoa(source.sin_addr));
}

void print_tcp_packet(unsigned char* buffer, int size) {
    printf("TCP Packet:\n");
    print_ip_header(buffer, size);
}

void print_udp_packet(unsigned char* buffer, int size) {
    printf("UDP Packet:\n");
    print_ip_header(buffer, size);
}

void print_icmp_packet(unsigned char* buffer, int size) {
    printf("ICMP Packet:\n");
    print_ip_header(buffer, size);
}
