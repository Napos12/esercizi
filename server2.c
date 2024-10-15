#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <pthread.h>

#define LEN_MAX 100
#define BUFFER_SIZE 1024

typedef struct _MSG {
    char path[BUFFER_SIZE];
    int value;
    char tipo[LEN_MAX];
} MSG;

typedef struct client_info {
    int sock;
    struct sockaddr_in client_addr;
} client_info;


char server_port[100];
char server_address[100];
char root_directory[100];

void* client_handler(void* arg) {
    client_info* client = (client_info*) arg;
    MSG messaggio;
    memset(&messaggio, 0, sizeof(messaggio));
    
    if (read(client->sock, &messaggio, sizeof(messaggio)) < 0) {
        perror("Read error");
        close(client->sock);
        free(client);
        pthread_exit(NULL);
    }
    
    printf("Ricevuto messaggio da client: %s\n", messaggio.tipo);
    
    close(client->sock);
    free(client);
    pthread_exit(NULL);
}

void check_argument(int argc, char* argv[])
{
    if(argc<7)
    {
        printf("argomenti mancanti\n");
        exit(-1);    
    }
    else{
        if(strcmp(argv[1],"-a")!=0 || strcmp(argv[3],"-p")!=0 || strcmp(argv[5],"-d")!=0 )
        {
            printf("Utilizzare formato: MyFTserve -a server_address -p server_port -d ft_root_directory\n");
            exit(-1);
        }
        else
        {
            strcpy(server_address,argv[2]);
            strcpy(server_address,argv[4]);
            strcpy(server_address,argv[6]);
        }

        

    }



}

int main(int argc, char* argv[]) {
    int server_sock;
    struct sockaddr_in server_addr, client_addr;
    int porta;
    socklen_t client_len;

    //funzione controllo argomenti.. 6 arg +chiamata funzione(7)
    check_argument();

    // Creazione socket
    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock == -1) {
        perror("Socket error");
        exit(-1);
    }

    // Impostazione dei parametri del server
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(porta);

    // Binding
    if (bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Binding error");
        close(server_sock);
        exit(-1);
    }

    // Ascolto
    if (listen(server_sock, 4) == -1) {
        perror("Listen error");
        close(server_sock);
        exit(-1);
    }

    printf("Server attivo alla porta %d, attendo connessioni...\n", ntohs(server_addr.sin_port));

    while (1) {
        client_len = sizeof(client_addr);
        int client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &client_len);
        
        if (client_sock < 0) {
            perror("Accept error");
            continue;
        }

        printf("Connessione ricevuta...\n");

        // Allocare memoria per le informazioni del client e passarlo al thread
        client_info* client = (client_info*) malloc(sizeof(client_info));
        client->sock = client_sock;
        memcpy(&client->client_addr, &client_addr, sizeof(client_addr));

        pthread_t client_thread;
        if (pthread_create(&client_thread, NULL, client_handler, (void*) client) != 0) {
            perror("Thread creation error");
            close(client_sock);
            free(client);
            continue;
        }

        // Detach del thread per gestire automaticamente la terminazione
        pthread_detach(client_thread);
    }

    close(server_sock);
    return 0;
}
