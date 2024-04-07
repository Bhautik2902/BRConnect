#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define SERVER_PORT 9009
#define LOCAL_IP 127.0.0.1

int main() {
    int sckt;
    struct sockaddr_in server_addr;
    char buffer[1024] = {0};
    char *message = "Hello, server!";

    // Creating the socket
    if ((sckt = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);

    // // Convert IPv4 or IPv6 addresses from text to binary form
    // if(inet_pton(AF_INET, LOCAL_IP, &server_addr.sin_addr) <= 0) {
    //     printf("\nInvalid address / Address not supported \n");
    //     return -1;
    // }

    // Making connection with server
    if (connect(sckt, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        printf("\nConnection Failed \n");
        return -1;
    }

    // Send a message to the server
    send(sckt, message, strlen(message), 0);
    printf("Message sent: %s\n", message);

    // Receive a response from the server
    int charread = read(sckt, buffer, sizeof(buffer));
    if(charread > 0) {
        printf("Server response: %s\n", buffer);
    }
    else {
        printf("Failed to receive a response from the server\n");
    }

    // closing the socket
    close(sckt);

    return 0;
}
