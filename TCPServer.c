// librarite e nevojshme
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <stdbool.h>
#include <time.h>
#include <fcntl.h>
#include <sys/stat.h>
#define MAX_CLIENTS 4
int num_clients = 0;
#define TIMEOUT_SECONDS 5
#define LOG_FILE "/home/shpati/CLionProjects/TCP-MultiClient-Server---C/log.txt"
// Struktura për të kaluar argumente te thread
struct thread_args {
    int sockfd ;
    struct sockaddr_in addr;

};
// Strukturë për lidhjet në pritje
struct queue_node {
    int sockfd;
    struct sockaddr_in addr;
    struct queue_node *next;
};

// Struktura e radhës
struct queue {
    struct queue_node *front, *rear;
    pthread_mutex_t lock;
} client_queue = {NULL, NULL, PTHREAD_MUTEX_INITIALIZER};

// Funksion për shtimin e lidhjeve në radhë
void enqueue(int sockfd, struct sockaddr_in addr) {
    struct queue_node *new_node = (struct queue_node *)malloc(sizeof(struct queue_node));
    new_node->sockfd = sockfd;
    new_node->addr = addr;
    new_node->next = NULL;

    pthread_mutex_lock(&client_queue.lock);
    if (client_queue.rear == NULL) {
        client_queue.front = client_queue.rear = new_node;
    } else {
        client_queue.rear->next = new_node;
        client_queue.rear = new_node;
    }
    pthread_mutex_unlock(&client_queue.lock);
}

// Funksion për heqjen e lidhjeve nga radhë
struct queue_node *dequeue() {
    pthread_mutex_lock(&client_queue.lock);
    struct queue_node *temp = client_queue.front;
    if (client_queue.front != NULL) {
        client_queue.front = client_queue.front->next;
        if (client_queue.front == NULL) {
            client_queue.rear = NULL;
        }
    }
    pthread_mutex_unlock(&client_queue.lock);
    return temp;
}
// Funksioni për të ekzekutuar komandën në shell
void execute_command(int newsockfd, char *command) {
    FILE *fp = popen(command, "r");
    if (fp == NULL) {
        perror("ERROR executing command");
        return;
    }

    char buffer[1024];
    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        send(newsockfd, buffer, strlen(buffer), 0);
    }

    pclose(fp);
}



// Funksioni që do të ekzekutohet nga çdo thread qe krijohet për menaxhimin e klientave
void *handle_client(void *arg) {
    // Hap skedarin e log-ut
    FILE *logfile = fopen(LOG_FILE, "a");
    if (logfile == NULL) {
        perror("ERROR opening log file");
        pthread_exit(NULL);
    }
    struct thread_args *args = (struct thread_args *)arg;
    int newsockfd = args->sockfd;
    struct sockaddr_in cli_addr = args->addr;
    free(args);



    char client_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(cli_addr.sin_addr), client_ip, INET_ADDRSTRLEN);
    printf("Serveri u lidh me klientin %s\n", client_ip);
    // Cakto nëse klienti është admin (fillimisht asnjë)
    static bool admin_connected = false;
    bool isAdmin = false;

    // Komunikimi i mesazheve, logimin @Shqiponja
    char buffer[256];
    while (1) {
        // Përdorim select() për të monitoruar aktivitetin e socket-it
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(newsockfd, &readfds);

        struct timeval timeout;
        timeout.tv_sec = 200; // Timeout prej 200 sekondash
        timeout.tv_usec = 0;

        int activity = select(newsockfd + 1, &readfds, NULL, NULL, &timeout);
        if (activity < 0) {
            perror("ERROR");
            break;
        }
        if (activity == 0) {
            printf("Klienti %s u shkëput për shkak të timeout-it.\n", client_ip);
            break; // Mbyll lidhjen për shkak të timeout-it
        }


        int bytes_received = recv(newsockfd, buffer, 255, 0);
        if (bytes_received <= 0) {
            break;
        }

        time_t now = time(NULL);
        char *time_str = ctime(&now);
        time_str[strcspn(time_str, "\n")] = 0; // Remove the newline from ctime()
        fprintf(logfile, "[%s] %s: %s\n", time_str, client_ip, buffer);

        if (strncmp(buffer, "admin enable", 12) == 0 && !admin_connected) {
            isAdmin = true;
            admin_connected = true;
            printf("Klienti %s është tani admin.\n", client_ip);
            send(newsockfd, "Privilegjet e admin-it u aktivizuan!", 37, 0);
        } else if (strncmp(buffer, "execute", 7) == 0 && isAdmin) {
            execute_command(newsockfd, buffer + 8); // Ekzekuto çdo komandë shell
        } else if (strncmp(buffer, "exit", 4) == 0) {
            if (isAdmin) {
                admin_connected = false; // Rivendos admin_connected nëse admin shkëputet
            }
            break;
        } else if (strncmp(buffer, "read", 4) == 0) {
            char *filename = buffer + 5;

            int fd = open(filename, O_RDONLY);
            if (fd == -1) {
                perror("Gabim gjatë hapjes së skedarit");
                send(newsockfd, "Gabim gjatë hapjes së skedarit.\n", 30, 0);
            } else {
                char file_content[1024];
                ssize_t bytes_read;
                while ((bytes_read = read(fd, file_content, sizeof(file_content) - 1)) > 0) {
                    file_content[bytes_read] = '\0'; // Vendos null terminator në fund të stringut
                    send(newsockfd, file_content, bytes_read, 0);
                }
                close(fd);
            }
        } else {

            if (!isAdmin) {
                usleep(500000); // 500 ms
            }


            printf("Klienti %s: %s\n", client_ip, buffer);
            char *server_message = "Kërkesa u pranua.";
            send(newsockfd, server_message, strlen(server_message) + 1, 0);
        }

        // Vonimi për klientët
        if (!isAdmin) {
        usleep(500000); // 500 ms
            }


        // Dërgo një mesazh te klienti
        char *server_message = "Kërkesa u pranua.";
        send(newsockfd, server_message, strlen(server_message) + 1, 0);
    }
    // Mbyllja e lidhjes
    fclose(logfile);
    close(newsockfd);
    printf("Klienti %s disconnected.\n", client_ip);
    num_clients--;
    pthread_exit(NULL);
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
    listen(sockfd, MAX_CLIENTS);

    // 5. Dëgjimi për lidhje
    int backlog = 5;
    listen(sockfd, backlog);

    // 6. Server loop
    while (1) {
        // 7. Pranimi lidhjes me klient
        struct sockaddr_in cli_addr;
        socklen_t clilen = sizeof(cli_addr);
        int newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);

        if (newsockfd < 0) {
            perror("ERROR duke pranuar lidhjen");
            continue;
        }
        pthread_mutex_lock(&client_queue.lock);
        if (num_clients < MAX_CLIENTS) {
            // Krijimi thread
            pthread_t thread_id;
            struct thread_args *args = malloc(sizeof(struct thread_args));
            args->sockfd = newsockfd;
            args->addr = cli_addr;
            pthread_create(&thread_id, NULL, handle_client, (void *)args);
            pthread_detach(thread_id);
            num_clients++;

        } else {
            enqueue(newsockfd, cli_addr);
            printf("Klienti është vendosur në pritje për shkak të kufizimit të lidhjeve.\n");


        }
        pthread_mutex_unlock(&client_queue.lock);

        // Kontrollo nëse ka vend të lirë dhe ka klientë në pritje
        while (num_clients < MAX_CLIENTS) {
            struct queue_node *queued_client = dequeue();
            if (!queued_client) {
                break;
            }

            pthread_t thread_id;
            struct thread_args *args = malloc(sizeof(struct thread_args));
            args->sockfd = queued_client->sockfd;
            args->addr = queued_client->addr;
            pthread_create(&thread_id, NULL, handle_client, (void *)args);
            pthread_detach(thread_id);
            num_clients++;
            free(queued_client);
        }
    }

    close(sockfd);

    return 0;
}