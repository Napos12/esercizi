#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <netdb.h>

#define LEN_MAX 100
#define SERVER_PORT 10000

typedef struct _MSG
{
    int value;
    char tipo[LEN_MAX];
}MSG;



int main(int argc, char* argv[])
{
    int client_fd;
    struct sockaddr_in server_address;
    struct hostent *hp;
    MSG messaggio;

    client_fd= socket(AF_INET,SOCK_STREAM,0);
    if(client_fd<0)
    {
        perror("Socket error");
        exit(-1);
    }

    server_address.sin_family = AF_INET;
    server_address.sin_port=htons(SERVER_PORT);

    hp=gethostbyname("localhost");
    if(hp==NULL)
    {
        herror("Error Localhost");
        exit(-1);
    }

    memcpy(&server_address.sin_addr, hp->h_addr, hp->h_length);
    
    if (connect(client_fd,(struct sockaddr *)&server_address,sizeof(server_address))<0)
    {
        perror("Connector error");
        exit(-1);
    }

    printf("Connessione riuscita!\n");

    messaggio.value = 10;
    printf("Inserisci un messaggio: ");
    fgets(messaggio.tipo, sizeof(messaggio.tipo), stdin); 
    //strcpy(messaggio.tipo,"Ciao sono un Client!");

    if(send(client_fd,&messaggio,sizeof(messaggio),0)<0)
    {
        perror("Send error");
        exit(-1);
    }

    printf("Messaggio inviato correttamente!\n");

    close(client_fd);


    return 0;
}