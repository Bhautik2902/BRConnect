#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>

#define PORT 9009
#define MIRR1_IP "127.0.0.1"
#define MIRR2_IP "127.0.0.1"
#define MIRR1_PORT 9010
#define MIRR2_PORT 9011

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

        char response[1024];
        snprintf(response, sizeof(response), "Server received: %s", buffer);
        sleep(60);

        // Returning result back to client
        if(write(client_sock, response, strlen(response)) > 0) {
            printf("Response sent successfully\n");
        }
    }

    close(client_sock); // Close the client socket
}

int main() {
    int conn_sckt, listening_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    int conn_count = 0;
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
    
    printf("Server accepting requests on port %d...\n", PORT);

    // setting messages for to send to the client.
    char *conn_info = "Connected with main server successfully"; // when connected with main server
    char Mirr1_info[64];
    sprintf(Mirr1_info, "REDIRECT %s %d", MIRR1_IP, MIRR1_PORT);  // sending mirror1 info to client
    char Mirr2_info[64];
    sprintf(Mirr2_info, "REDIRECT %s %d", MIRR2_IP, MIRR2_PORT);  // sending mirror1 info to client

    while(1) {
        if ((listening_socket = accept(conn_sckt, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0) {
            perror("accept_main");
            continue; // Continue to the next iteration if accept fails
        }
        if (conn_count <= 3) {  // requests to handle at main server
            send(listening_socket, conn_info, strlen(conn_info), 0);
            conn_count++;
        }
        else if (conn_count <= 6) { // requests to handle at mirror1
            send(listening_socket, Mirr1_info, strlen(Mirr1_info), 0);
            conn_count++;
        }
        else if (conn_count <= 9) {  // requests to handle at mirror2
            send(listening_socket, Mirr2_info, strlen(Mirr2_info), 0);
            conn_count++;
        }
        else {  // setting alternative server.
            switch ((conn_count - 10) % 3) {
            case 0: // main server
                send(listening_socket, conn_info, strlen(conn_info), 0);
                conn_count++;
                break;
            case 1: // mirror1 
                send(listening_socket, Mirr1_info, strlen(Mirr1_info), 0);
                conn_count++;
                break;
            case 2: // mirror2
                send(listening_socket, Mirr2_info, strlen(Mirr2_info), 0);
                conn_count++;
                break;
            }
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
