// librarite e nevojshme
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

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
    }

    return 0;
}