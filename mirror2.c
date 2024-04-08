#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>

#define PORT 9011


void crequest(int client_sock) {
    char buffer[1024];
    int client_readChar;

    // waiting for command from client in infinite loop
    while(1) {
        memset(buffer, 0, sizeof(buffer)); // resetting the buffer content.
        
        // Reading command sent from client
        client_readChar = read(client_sock, buffer, sizeof(buffer));
        if(client_readChar <= 0) {
            perror("read_crequest");
            break; 
        }
        
        printf("Command received and processing....: %s\n", buffer);

        sleep(120);
        char response[1024];
        snprintf(response, sizeof(response), "Server received: %s", buffer);

        // Returning result back to client
        write(client_sock, response, strlen(response));
    }

    close(client_sock); // Close the client socket
}

int main() {
    int conn_sckt, listening_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    pid_t child_pid;

    // Creating socket file descriptor (connection socket)
    if ((conn_sckt = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    
    // setting address attributes.
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    
    // Binding the socket with port number
    if (bind(conn_sckt, (struct sockaddr *)&address, sizeof(address)) == -1) {
        perror("bind_main");
        exit(EXIT_FAILURE);
    }
    
    if (listen(conn_sckt, 3) < 0) {
        perror("listen_main");
        exit(EXIT_FAILURE);
    }
    
    printf("Mirror2 is accepting requests on port %d...\n", PORT);

    while(1) {

        if ((listening_socket = accept(conn_sckt, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0) {
            perror("accept_main");
            continue; // Continue to the next iteration if accept fails
        }
        
        // Fork a child process to handle the client's request
        child_pid = fork();
        
        if(child_pid < 0) {
            perror("fork failed");
            close(listening_socket);
            continue;
        }
        
        if(child_pid == 0) {
            close(conn_sckt); // Close the listening socket in the child process
            crequest(listening_socket); // Handle the client's request
            exit(0);
        } 
        else {
            close(listening_socket); // Close the connected socket in the parent process
        }
    }
    
    // Should never reach here
    close(conn_sckt);
    return 0;
}
