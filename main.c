#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

struct sockaddr_in client_hint;
char file_name[100];

void error(const char*msg){
    perror(msg);
    exit(1);
}
/* Parameter: args - client connection id
   Opens and reads file and writes to client
*/
void* ServClient(void *args){
    int sockfd = *((int*)args);
    printf("Connection accepted and id: %d\n", sockfd);
    printf("Connected to Client: %s:%d",inet_ntoa(client_hint.sin_addr),ntohs(client_hint.sin_port));
    write(sockfd, file_name, 256);
    FILE *fp = fopen(file_name, "rb");
    if(fp == NULL){
        error("Could not open file");
    }
    while(1){
        unsigned char buff[1024] = {0};
        int nread = fread(buff,1,1024,fp);
        if(nread > 0){
            write(sockfd, buff, nread);
        }
        if(nread < 1024){
            if(feof(fp)){
                printf("End of file\n");
                printf("File transfer completed for id: %d\n", sockfd);
            }
            if(ferror(fp)){
                printf("Error reading");
            }
            break;
        }
    }
    printf("Closing Connection for id: %d\n", sockfd);
    close(sockfd);
    shutdown(sockfd, SHUT_WR);
    sleep(2);
}

int main(int argc, char** argv){
    if(argc < 2){
        error("No port provided, compile like './main [portnumber]' where portnumber > 2000");
        exit(1);
    }
    // initialize variablies
    int sockfd, portno, confd = 0;
    pthread_t t;
    struct sockaddr_in hint;
    socklen_t client_len = 0;
    portno = atoi(argv[1]);
    memset((char *)&hint, '0', sizeof(hint));

    // create socket for listening
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0){
        error("Error opening socket");
    }

    /* Bind protocol type,
       ip address, port number
    */ 
    hint.sin_family = AF_INET;
    hint.sin_addr.s_addr = htonl(INADDR_ANY);
    hint.sin_port = htons(portno);

    if(bind(sockfd, (struct sockaddr*)&hint, sizeof(hint)) < 0){
        error("Could not bind");
    }

    // Wait for a connection
    int err;
    listen(sockfd, SOMAXCONN);

    // Ask/Copy (for) file name to file_name to be read
    if(argc < 3){
        printf("Enter file name to send: ");
        fgets(file_name,255,stdin);
        size_t len;
        len = strlen(file_name);
        file_name[len-1] = '\0';
    }else{
        strcpy(file_name,argv[2]);
    }

    /* Accept connection from client
       then create thread and call
       ServClient to send file
    */
    while(1){
        client_len = sizeof(client_hint);
        confd = accept(sockfd,(struct sockaddr*)&client_hint,&client_len);
        if(confd < 0){
            error("Error could not accept");
            continue;
        }
        printf("Connected to client %dn", confd);
        err = pthread_create(&t, NULL, &ServClient, &confd);
        if(err != 0){
            printf("Can't create thread");
        }
    }

    // CLOSE SOCKET!!!
    close(sockfd);
    return 0;
}

    ///////////////////////////////////
    //For one threaded server/client
    ///////////////////////////////////
    // listen(sockfd, SOMAXCONN);
    // client_len = sizeof(client_hint);

    // newsockfd = accept(sockfd, (struct sockaddr *)&client_hint, &client_len);
    // if(newsockfd < 0){
    //     error("Could not accept socket");
    // }

    // while(1){
    //     memset(buffer,'0',255);
    //     n = read(newsockfd, buffer, 255);
    //     if(n < 0){
    //         error("could not read");
    //     }
    //     printf("Client: %s\n", buffer);
    //     memset(buffer, '0',255);
    //     fgets(buffer, 255, stdin);
    //     n = write(newsockfd, buffer, strlen(buffer));
    //     if(n<0){
    //         error("Could not write");
    //     }

    //     int i = strncmp("Quit", buffer, 4);
    //     if(i == 0){
    //         break;
    //     }
    // }
    // close(newsockfd);
    // close(sockfd);
    /////////////////////////////////////////////////
