#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "usage %s ip_address port\n", argv[0]);
        exit(0);
    }

    char *ip_address = argv[1];
    int portno = atoi(argv[2]);

    int sockfd;
    struct sockaddr_in serv_addr;
    char buffer[256];
    int bytes_received;

    // Krijo socket-in
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Error creating socket");
        exit(1);
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(portno);
    inet_pton(AF_INET, ip_address, &serv_addr.sin_addr);

    // Përdorni një cikël për të lidhur klientin me serverin
    while (1) {
        if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
            printf("Serveri është në kapacitet të plotë, duke pritur...\n");
            sleep(5); // Prit 5 sekonda dhe përpiqu përsëri
        } else {
            printf("Lidhja me serverin u realizua me sukses.\n");
            break;  // Lidhja u realizua, doli nga cikli
        }
    }

    // Cikli i komunikimit me serverin
    while (1) {
        printf("client: ");
        fgets(buffer, sizeof(buffer), stdin);

        // Hiq newline nga fgets()
        buffer[strcspn(buffer, "\n")] = 0;

        send(sockfd, buffer, strlen(buffer) + 1, 0);

        if (strcmp(buffer, "exit") == 0) {
            break;
        }

        // Prit përgjigjen nga serveri
        bytes_received = recv(sockfd, buffer, sizeof(buffer), 0);
        if (bytes_received > 0) {
            buffer[bytes_received] = '\0';  // Siguro që buffer-i përfundon me '\0'
            printf("Serveri: %s\n", buffer);

            // Kontrollo për mesazhe specifike nga serveri
            if (strstr(buffer, "Serveri është në kapacitet të plotë") != NULL) {
                printf("Ju jeni në radhë dhe do të lidheni kur të lirohet një vend.\n");
            } else if (strstr(buffer, "Lidhja është liruar") != NULL) {
                printf("Lidhja është e lirë. Po lidheni...\n");
            } else if (strstr(buffer, "Ti je lidhur pasi që është liruar një vend") != NULL) {
                printf("Ti je lidhur pasi që është liruar një vend. Mirësevini!\n");
            }
        } else if (bytes_received == 0) {
            printf("Lidhja me serverin u mbyll.\n");
            break;
        } else {
            perror("Error receiving data");
            break;
        }
    }

    close(sockfd);
    return 0;
}
