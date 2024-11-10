#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

int main(int   
 argc, char *argv[]) {
    if (argc < 3) { 
        fprintf(stderr,"usage %s ip_address port\n", argv[0]);
        exit(0);
    }

    char *ip_address = argv[1]; 
    int portno = atoi(argv[2]); 

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port   
 = htons(portno);
    inet_pton(AF_INET, ip_address, &serv_addr.sin_addr);

    connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));


    char buffer[256];
    while (1) {
        printf("client: "); 
        fgets(buffer, 255, stdin); 
     
       // Hiq newline nga fgets()
        buffer[strcspn(buffer, "\n")] = 0; 

        send(sockfd, buffer, strlen(buffer) + 1, 0); 

        if (strcmp(buffer, "exit") == 0) { 
            break;
        }
     recv(sockfd, buffer, 255, 0); 
        printf("Serveri: %s\n", buffer); 
    }
 
    close(sockfd); 
    return 0;
}
