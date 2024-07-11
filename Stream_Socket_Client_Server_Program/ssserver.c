#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 5000
#define MAX_CONNECTIONS 5
#define BUFFER_SIZE 1024

int main(int argc, char const *argv[]) {
    int server_socket, client_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};

    // create a TCP/IP socket
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket creation error");
        exit(EXIT_FAILURE);
    }

    // set socket options
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("Setsockopt error");
        exit(EXIT_FAILURE);
    }

    // set server address and port
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // bind the socket to the address and port
    if (bind(server_socket, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind error");
        exit(EXIT_FAILURE);
    }

    // listen for incoming connections
    if (listen(server_socket, MAX_CONNECTIONS) < 0) {
        perror("Listen error");
        exit(EXIT_FAILURE);
    }

    printf("Waiting for connection...\n");

    // accept incoming connection
    if ((client_socket = accept(server_socket, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
        perror("Accept error");
        exit(EXIT_FAILURE);
    }

    printf("Connected.\n");

    // loop for interactive communication
    while (1) {
        // receive message from client
        valread = read(client_socket, buffer, BUFFER_SIZE);
        if (valread == 0) {
            printf("Client disconnected.\n");
            break;
        }
        printf("Received message: %s\n", buffer);

        // send response back to client
        printf("Enter response: ");
        fgets(buffer, BUFFER_SIZE, stdin);
        send(client_socket, buffer, strlen(buffer), 0);
    }

    close(client_socket);
    close(server_socket);
    return 0;
}
