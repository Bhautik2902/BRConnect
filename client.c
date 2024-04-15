#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <time.h>

#define SERVER_PORT 9009
#define LOCAL_IP "127.0.0.1"
#define DEF_BUFF_SIZE 1024

#define MAX_SIZE 7

char* _commandList[MAX_SIZE]={"dirlist -a","dirlist -t","w24fn","w24fz","w24ft","w24fdb","w24fda"};

int CommandParser(char *message)
{
  for(int i=0;i<MAX_SIZE;i++)
    {
      if(strncmp(message,_commandList[i],strlen(_commandList[i]))==0)
      {
        if(strcmp(_commandList[i],"dirlist -a")==0)
          {
            printf("1. %s\n",_commandList[i]);
            return 1;
          }
        else if(strcmp(_commandList[i],"dirlist -t")==0)
          {
            printf("2. %s\n",_commandList[i]);
            return 1;
          }
        else if(strcmp(_commandList[i],"w24fn")==0)
          {
            char *token=strtok(message," ");
            token=strtok(NULL," ");
            if(sizeof(token)>=1)
            {
              return 1;
            }
            else
            {
              printf("Incorrect FileName \n");
              return 0;
            }
          }
        else if(strcmp(_commandList[i],"w24fz")==0)
            {
              char *token=strtok(message," ");
              for(int j=0;j<2;j++)
                {
                  token=strtok(NULL," ");
                  if(sizeof(token)>=1 && token!=NULL)
                  {
                    char *endptr;
                    int num=strtol(token,&endptr,10);
                    if(*endptr!='\0')
                    {
                        printf("Enter Size in Numeric Only \n");
                        return 0;
                    }
                  }
                  else
                  {
                    printf("Incorrect Size \n");
                    return 0;
                  }
                }
                printf("conversion worked");
                return 1;
            }
        else if(strcmp(_commandList[i],"w24ft")==0)
            {
              int counter=0;
              char *token=strtok(message," ");
              counter++;
              while(token!=NULL && sizeof(token)>1){
                token=strtok(NULL," ");
                counter++;
              }
              if(counter>5){
                printf("too many parameters \n");
                return 0;
              }
              else if(counter<3){
                printf("Atleast give one extension \n");
                return 0;
              }
              return 1;
            }
        else if(strcmp(_commandList[i],"w24fdb")==0)
            {
              char *token=strtok(message," ");
              token=strtok(NULL," ");
              if(sizeof(token)>=1 && token!=NULL){
                struct tm tm;
                if (strptime(token, "%Y-%m-%d", &tm) == NULL)
                {
                  printf("Invalid date format.\n");
                  return 0;
                }
                else
                {
                  printf("Valid date format: %s\n", token);
                  return 1;
                }
              }
              return 0;  
            }
        else if(strcmp(_commandList[i],"w24fa")==0)
            {
              char *token=strtok(message," ");
              token=strtok(NULL," ");
              if(sizeof(token)>=1 && token!=NULL){
                struct tm tm;
                if (strptime(token, "%Y-%m-%d", &tm) == NULL)
                {
                  printf("Invalid date format.\n");
                  return 0;
                }
                else
                {
                  printf("Valid date format: %s\n", token);
                  return 1;
                }
              }
              return 0;
            }
            else{
              return 0;
            }
        return 1;
      }
    }
    return 0;
}

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

    return sock; // Return the new socket descriptor
}

void letClientChat(int sckt) {
    // loop to let client send multiple command to server
    char *message = malloc(256 * sizeof(char));
    char *serResponse;
    if (message == NULL) printf("Memory allocation failed\n");

    do {
        printf("\n\nEnter command: ");
        // Read user command from standard cmd
        if (scanf(" %[^\n]", message) == 0) {
            perror("scanf");
            exit(EXIT_FAILURE);
        }

        char *temp = strdup(message); // making temp copy of usercmd to perform validation
        int isValid = CommandParser(temp);

        if(isValid == 1) {
          // send command.
          send(sckt, message, strlen(message), 0);

          // getting response size.
          int int_received;
          read(sckt, &int_received, sizeof(int_received));

          // Convert from network byte order to host byte order
          int response_size = ntohl(int_received);

          //creating buffer of required size
          serResponse = malloc((response_size * sizeof(char)));

          printf("\nResponse from server:\n\n");
          if (recv(sckt, serResponse, response_size, 0) > 0) {
              printf("%s", serResponse);
          }
            // free(serResponse);
          memset(serResponse, 0, sizeof(serResponse)); // resetting the buffer content.

        }
        else {
            printf("Given command is not valid\n");
        }
    } while(strcmp(message, "quitc") != 0);

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


