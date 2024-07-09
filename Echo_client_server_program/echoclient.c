#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 12345
#define BUFFER_SIZE 1024

int main(int argc, char const *argv[]) {
    int client_socket, valread;
    struct sockaddr_in server_address;
    char buffer[BUFFER_SIZE] = {0};


    // create a TCP/IP socket
    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation error");
        exit(EXIT_FAILURE);
    }

    // set server address and port
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);

    // convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET,argv[1], &server_address.sin_addr) <= 0) {
        perror("Invalid address / Address not supported");
        exit(EXIT_FAILURE);
    }

    // connect to server
    if (connect(client_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        perror("Connection error");
        exit(EXIT_FAILURE);
    }

    printf("Connected to server.\n");

    // loop for interactive communication
    while (1) {
        // send message to server
        printf("Enter message: ");
        fgets(buffer, BUFFER_SIZE, stdin);
        send(client_socket, buffer, strlen(buffer), 0);

        // receive response from server
        valread = read(client_socket, buffer, BUFFER_SIZE);
        if (valread == 0) {
            printf("Server disconnected.\n");
            break;
        }
    printf("Response from server (%s:%d): %.*s",inet_ntoa(server_address.sin_addr),ntohs(server_address.sin_port), valread, buffer);

    }

    close(client_socket);
    return 0;
}
