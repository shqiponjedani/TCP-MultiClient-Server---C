// librarite e nevojshme
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>



// Funksioni që do të ekzekutohet nga çdo thread qe krijohet për menaxhimin e klientave
void handle_client(int newsockfd, struct sockaddr_in cli_addr) {
    char client_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(cli_addr.sin_addr), client_ip, INET_ADDRSTRLEN);
    printf("Serveri u lidh me klientin %s\n", client_ip);

    // Komunikimi dhe logimi i mesazheve
    char buffer[256];
    while (1) {
        int bytes_received = recv(newsockfd, buffer, 255, 0);
        if (bytes_received <= 0) {
            break;
        }

        printf("Klienti %s: %s\n", client_ip, buffer);

        // Dërgo një mesazh te klienti
        char *server_message = "Ky është një mesazh nga serveri.";
        send(newsockfd, server_message, strlen(server_message) + 1, 0);
    }

    close(newsockfd);
    printf("Klienti %s disconnected.\n", client_ip);
}

int main() {

    // 1. Deklarimi i variablave për portin dhe IP adresën
    int port = 3490;
    char *server_ip_address = "127.0.0.1";

    // 2. Krijimi i socket-it
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    // 3. Konfigurimi i adresës së serverit
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    inet_pton(AF_INET, server_ip_address, &serv_addr.sin_addr);
    serv_addr.sin_port = htons(port);

    // 4. Lidhja e socket-it me adresën
    bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));

    // 5. Dëgjimi për lidhje
    int backlog = 5;
    listen(sockfd, backlog);

    // 6. Server loop
    while (1) {
        // 7. Pranimi lidhjes me klient
        struct sockaddr_in cli_addr;
        socklen_t clilen = sizeof(cli_addr);
        int newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);

        handle_client(newsockfd, cli_addr);
    }

    return 0;
}