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
#include <sys/stat.h>
#include <windows.h>
#include "ticTacToe.c"
#define BUFFLEN 1024

int main(int argc, char *argv[]){

#ifdef _WIN32
    WSADATA data;
#endif
    unsigned int port;
    struct sockaddr_in servaddr;
    fd_set read_set;

    char recvbuffer[BUFFLEN];
    char sendbuffer[BUFFLEN];

    int client_id;
    int s_socket;
//tic tac toe--------------------

    char board[SIDE][SIDE];
    int moves[SIDE*SIDE];
    int moveIndex = 0;
    int selection = 0;
    int opSelection = 0;

//-------------------------------

    if (argc != 3){
        fprintf(stderr,"USAGE: %s <ip> <port>\n",argv[0]);
        exit(1);
    }

    port = atoi(argv[2]);

    if ((port < 1) || (port > 65535)){
        printf("ERROR #1: invalid port specified.\n");
        exit(1);
    }

#ifdef _WIN32
    WSAStartup(MAKEWORD(2,2),&data);
#endif

    if ((s_socket = socket(AF_INET, SOCK_STREAM,0))< 0){
        fprintf(stderr,"ERROR #2: cannot create socket.\n");
        exit(1);
    }

    memset(&servaddr,0,sizeof(servaddr));
    servaddr.sin_family = AF_INET; // protokolas (IP)
    servaddr.sin_port = htons(port); // portas

#ifdef _WIN32
    servaddr.sin_addr.s_addr = inet_addr(argv[1]);
#else
    if ( inet_aton(argv[1], &servaddr.sin_addr) <= 0 ) {
        fprintf(stderr,"ERROR #3: Invalid remote IP address.\n");
        exit(1);
    }
#endif


    if (connect(s_socket,(struct sockaddr*)&servaddr,sizeof(servaddr))<0){
        fprintf(stderr,"ERROR #4: error in connect().\n");
        exit(1);

    }
    else {
        if(recv(s_socket,recvbuffer,BUFFLEN,0))
        {
            initialise(board, moves);
            printf("You are: %s\n", recvbuffer);
            client_id = 1;

            showInstructions();
            if(recvbuffer[0] == '2')
            {
                client_id = 2;
                memset(&recvbuffer,0,BUFFLEN);
                recv(s_socket,recvbuffer,BUFFLEN,0);
                printf("Opponent's move: %s\n", recvbuffer);
                opSelection = atoi(recvbuffer);
                PositionCalculation(board, opSelection, 'x');
                DrawBoard(board);
                moveIndex ++;
                if (moveIndex > 3)
                {
                    gameOver(board);
                }
            }
            memset(&recvbuffer,0,BUFFLEN);
        }
    }

    memset(&sendbuffer,0,BUFFLEN);
    u_long nonblocking_enabled = TRUE;
    ioctlsocket(s_socket, FIONBIO, &nonblocking_enabled );

    while (gameOver(board) == false && moveIndex != SIDE*SIDE) {
        FD_ZERO(&read_set);
        FD_SET(s_socket, &read_set);
        FD_SET(0, &read_set);
        select(s_socket + 1, &read_set, NULL, NULL, NULL);

        if (client_id == 1)
        {
            printf("\nPlease input where you want to place x: ");
        }
        else
        {
            printf("\nPlease input where you want to place o: ");
        }
        fgets(sendbuffer, BUFFLEN, stdin);

        selection = atoi(sendbuffer);
        while (selection < 0 || selection > 10 || !cellEmpty(board, selection))
        {
            printf("Invalid input, please try again: ");
            fgets(sendbuffer, BUFFLEN, stdin);
            selection = atoi(sendbuffer);
        }

        if (client_id == 1) {
            PositionCalculation(board, selection, 'x');
        }
        else{
            PositionCalculation(board, selection, 'o');
        }

        DrawBoard(board);
        moveIndex ++;

        if (moveIndex > 3 && gameOver(board) || moveIndex == 9)
        {
            send(s_socket, sendbuffer, strlen(sendbuffer), 0);
            memset(&sendbuffer, 0, BUFFLEN);
            strcpy(sendbuffer, "/x");
            send(s_socket, sendbuffer, strlen(sendbuffer), 0);
            break;
        }

        send(s_socket, sendbuffer, strlen(sendbuffer), 0);
        memset(&sendbuffer, 0, BUFFLEN);
        memset(&recvbuffer, 0, BUFFLEN);

        while (!recv(s_socket, recvbuffer, BUFFLEN, 0) || strcmp(recvbuffer, "") == 0) {
            sleep(1);
        }

        if (recv(s_socket, recvbuffer, BUFFLEN, 0) && strcmp(recvbuffer, "") != 0) {

            printf("Opponent's move: %s\n", recvbuffer);
            opSelection = atoi(recvbuffer);
            if(client_id == 1)
                PositionCalculation(board, opSelection, 'o');
            if(client_id == 2)
                PositionCalculation(board, opSelection, 'x');
            DrawBoard(board);
            moveIndex ++;

            memset(&recvbuffer, 0, BUFFLEN);
            if (recv(s_socket, recvbuffer, BUFFLEN, 0) && recvbuffer[0] == '/' && recvbuffer[1] == 'x')
            {
                gameOver(board);
                break;
            }
            if (moveIndex > 3)
            {
                gameOver(board);
            }
        }
    }

    gameEnd(moveIndex, board);
    close(s_socket);
    return 0;
}