#ifdef _WIN32
#include <winsock2.h>
#endif

#include <fcntl.h>
#include <stdbool.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ws2tcpip.h>
#define BUFFLEN 1024
#include <sys/stat.h>

int main(int argc, char *argv[]){
#ifdef _WIN32
    WSADATA data;
#endif

    unsigned int port;
    int s_socket;
    int o_socket; //oponento;
    struct sockaddr_in servaddr; // Serverio adreso struktūra
    fd_set read_set;

    char recvbuffer[BUFFLEN];
    char sendbuffer[BUFFLEN];

    int i;
    int client_id = -1;

    if (argc != 3){
        fprintf(stderr,"USAGE: %s <ip> <port>\n",argv[0]);
        exit(1);
    }

    port = atoi(argv[2]);

    if ((port < 1) || (port > 65535)){
        printf("ERROR #1: invalid port specified.\n");
        exit(1);
    }

    /*
     * Sukuriamas socket'as
     */
#ifdef _WIN32
    WSAStartup(MAKEWORD(2,2),&data);
#endif

    if ((s_socket = socket(AF_INET, SOCK_STREAM,0))< 0){
        fprintf(stderr,"ERROR #2: cannot create socket.\n");
        exit(1);
    }

    /*
     * Išvaloma ir užpildoma serverio struktūra
     */
    memset(&servaddr,0,sizeof(servaddr));
    servaddr.sin_family = AF_INET; // nurodomas protokolas (IP)
    servaddr.sin_port = htons(port); // nurodomas portas

#ifdef _WIN32
    servaddr.sin_addr.s_addr = inet_addr(argv[1]);
#else
    if ( inet_aton(argv[1], &servaddr.sin_addr) <= 0 ) {
        fprintf(stderr,"ERROR #3: Invalid remote IP address.\n");
        exit(1);
    }
#endif
    /*
     * Prisijungiama prie serverio
     */
    if (connect(s_socket,(struct sockaddr*)&servaddr,sizeof(servaddr))<0){
        fprintf(stderr,"ERROR #4: error in connect().\n");
        exit(1);

    }
    else {
        if(recv(s_socket,recvbuffer,BUFFLEN,0))
        {
            printf("You are: %s\n", recvbuffer);
            client_id= 1;
            if(recvbuffer[0] == '2')
            {
                client_id = 2;
                memset(&recvbuffer,0,BUFFLEN);
                recv(s_socket,recvbuffer,BUFFLEN,0);
                printf("Opponent's move: %s\n", recvbuffer);
            }
            memset(&recvbuffer,0,BUFFLEN);
        }
        if(client_id == 1 ) {
            o_socket = recv(s_socket,recvbuffer,BUFFLEN,0);
            printf("kito socketas: %s\n", o_socket);
        }

    }
    memset(&sendbuffer,0,BUFFLEN);
    u_long nonblocking_enabled = TRUE;
    ioctlsocket(s_socket, FIONBIO, &nonblocking_enabled );
    bool running = true;

    while (running){
        FD_ZERO(&read_set);
        FD_SET(s_socket,&read_set);
        FD_SET(0,&read_set);

        select(s_socket+1,&read_set,NULL,NULL,NULL);
        if(client_id == 1)
        {

        }
        if(client_id == 2)
        {

        }
        printf("Your move: ");
        fgets(sendbuffer, BUFFLEN, stdin);
        /*
            * Išsiunčiamas pranešimas serveriui
        */
        send(s_socket,sendbuffer,strlen(sendbuffer),0);
        if(sendbuffer[0] == '/' && sendbuffer[1] == 'x')
        {
            running = false;
            close(s_socket);
        }
        memset(&sendbuffer,0,BUFFLEN);
        /*
        * Pranešimas gaunamas iš serverio
        */
        if (FD_ISSET(s_socket, &read_set)){
            memset(&recvbuffer,0,BUFFLEN);
            read(s_socket, &recvbuffer, BUFFLEN);
            printf("%s\n",recvbuffer);
        }
        else if (FD_ISSET(0,&read_set)) {
            i = read(0,&sendbuffer,1);
            write(s_socket, sendbuffer,i);
        }

        recv(s_socket,recvbuffer,BUFFLEN,0);
        printf("Server sent: %s\n", recvbuffer);

    }

//

    return 0;
}