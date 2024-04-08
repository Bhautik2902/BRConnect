#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define SERVER_PORT 9009
#define LOCAL_IP "127.0.0.1"

int connectToMirror(const char *serverIP, int serverPort) {
    struct sockaddr_in serverAddr;
    int sock;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation error");
        return -1;
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(serverPort);

    if(inet_pton(AF_INET, serverIP, &serverAddr.sin_addr) <= 0) {
        printf("\nInvalid address / Address not supported \n");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }

    // You can now use `sock` to communicate with the new server
    // Remember to close the socket when done: close(sock);

    return sock; // Return the new socket descriptor
}

void letClientChat(int sckt) {
    // loop to let client send multiple command to server
    char *message = malloc(256 * sizeof(char));
    if (message == NULL) printf("Memory allocation failed\n");

    do {
        printf("Enter command: ");
        scanf("%s", message);

        // send message.
        send(sckt, message, strlen(message), 0);
        sleep(5);

    } while(strcmp(message, "quitc") == 0);

    // sending quit command to server.
    send(sckt, message, strlen(message), 0);

}

int main() {
    int sckt;
    //struct sockaddr_in server_addr;
    char buffer[1024] = {0};

    // making connection with main server
    sckt = connectToMirror(LOCAL_IP, SERVER_PORT);
    if (sckt == -1) {  // if fails to create success.
        return 1;
    }
    
    // Receive initial(connection) response from the server
    int charread = read(sckt, buffer, sizeof(buffer));
    if(charread > 0) {
        if(strstr(buffer, "REDIRECT") != NULL) {  // response has redirection information for mirrors.
            char mirr_ip[16];
            int mirr_port;

            // reading ip and port of respective mirror from message
            sscanf(buffer, "REDIRECT %s %d", mirr_ip, &mirr_port);

            // connecting to the respective server
            int mirr_sckt = connectToMirror(mirr_ip, mirr_port);
            if (mirr_sckt == -1) return 1;

            close(sckt);  // closing old socket
            printf("Connected to mirror server: %s:%d\n", mirr_ip, mirr_port);
            letClientChat(mirr_sckt);
        }
        else {  // normal execuation at server            
            printf("Connected to main server: %s:%d\n", LOCAL_IP, SERVER_PORT);
            
            letClientChat(sckt);
        }
    }
    else {
        printf("Failed to receive a response from the server\n");
    }

    // closing the socket
    close(sckt);

    return 0;
}
