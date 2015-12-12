//
// Created by lateg on 11/12/2015.
//

#if defined WIN32
#include <winsock.h>
#else
#define closesocket close
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#endif

#include <stdio.h>
#include <string.h> //for memset()

#define ECHOMAX 255
#define PORT 18000

void errorHandler(char *msg) {
    printf(msg);
}

void clearWinSock() {
#if defined WIN32
    WSACleanup();
#endif
}

int main() {
#if defined WIN32
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if(iResult != 0) {
        printf("Error at WSAStartup!\n");
        return 0;
    }
#endif

    int sock;
    struct sockaddr_in serverAddr;
    struct sockaddr_in clientAddr;
    unsigned int clientAddrLen;  //lunghezza dell'indirizzo
    char buffer[ECHOMAX];
    int recvMsgSize;  //dimensione del messaggio

    //CREAZIONE DELLA SOCKET
    if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
        errorHandler("socket() failed!");
    }

    // COSTRUZIONE DELL'INDIRIZZO DEL SERVER
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // BIND DELLA SOCKET
    if ((bind(sock, (struct sockaddr *) &serverAddr, sizeof(serverAddr))) < 0) {
        errorHandler("bind() failed!");
    }

    // RICEZIONE DELLA STRINGA ECHO DAL CLIENT
    while (1) {
        clientAddrLen = sizeof(clientAddr);
        recvMsgSize = recvfrom(sock, buffer, ECHOMAX, 0, (struct sockaddr *) &clientAddr, &clientAddrLen);
        printf("Handling client %s\n", inet_ntoa(clientAddr.sin_addr));
        printf("Received: %s \n", buffer);

        // RINVIA LA STRINGA ECHO AL CLIENT
        if (sendto(sock, buffer, recvMsgSize, 0, (struct sockaddr *) &clientAddr, sizeof(clientAddr)) != recvMsgSize) {
            errorHandler("sendto() sent different number of bytes than expected!");
        }
    }
}











