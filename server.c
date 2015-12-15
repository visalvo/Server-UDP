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
#include <ctype.h>

#define DIM_MSG 255
#define PORT 18000

void errorHandler(char *msg) {
    printf(msg);
}

void clearWinSock() {
#if defined WIN32
    WSACleanup();
#endif
}

void closeConnection(int socket) {
    closesocket(socket);
    clearWinSock();
}

int main() {
#if defined WIN32
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        printf("Error at WSAStartup!\n");
        return 0;
    }
#endif

    int sock;
    struct sockaddr_in serverAddr;
    struct sockaddr_in clientAddr;
    unsigned int clientAddrLen;  //lunghezza dell'indirizzo
    int msg_len;  //dimensione del messaggio
    char buffer[DIM_MSG];  //buffer per la ricezione
    int recvMsgSize;  //dimensione del messaggio ricevuto

    //CREAZIONE DELLA SOCKET
    if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
        errorHandler("socket() failed!");
        clearWinSock();
        return 0;
    }

    // COSTRUZIONE DELL'INDIRIZZO DEL SERVER
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // BIND DELLA SOCKET
    if ((bind(sock, (struct sockaddr *) &serverAddr, sizeof(serverAddr))) < 0) {
        errorHandler("bind() failed!");
        closeConnection(sock);
        return 0;
    }


    while (1) {
        // RICEZIONE DEL MESSAGGIO INIZIALE DEL CLIENT CON IL NOME DELL'HOST
        clientAddrLen = sizeof(clientAddr);
        if ((recvMsgSize = recvfrom(sock, buffer, DIM_MSG, 0, (struct sockaddr *) &clientAddr, &clientAddrLen)) <= 0) {
            errorHandler("recvfrom() receive different number of bytes than expected!");
            closeConnection(sock);
            return 0;
        }
        buffer[recvMsgSize] = '\0';
        printf("Message from the client: '%s'\n", buffer);

        struct in_addr addr;
        char *s = inet_ntoa(clientAddr.sin_addr);
        addr.s_addr = inet_addr(s);
        struct hostent *host = gethostbyaddr((char *) &addr, 4, AF_INET);
        printf("Received from client with host name: '%s'\n", host->h_name);

        //INVIO DEL MESSAGGIO OK AL CLIENT
        char *msg = "OK";
        msg_len = strlen(msg);
        if (sendto(sock, msg, msg_len, 0, (struct sockaddr *) &clientAddr, sizeof(clientAddr)) != msg_len) {
            errorHandler("sendto() sent different number of bytes than expected!");
            closeConnection(sock);
            return 0;
        }

        //RICEZIONE VOCALI DAL CLIENT
        printf("Vocali ricevute dal client: ");
        char c;
        int ans = 1;
        while (ans) {
            if ((recvMsgSize = recvfrom(sock, buffer, 1, 0, (struct sockaddr *) &clientAddr, &clientAddrLen)) <= 0) {
                errorHandler("recvfrom() receive different number of bytes than expected!");
                closeConnection(sock);
                return 0;
            }

            if (buffer[0] == '\n') {
                ans = 0;
            }
            else {
                buffer[recvMsgSize] = '\0';
                printf("%s, ", buffer);
                c = toupper(buffer[0]);
                if (sendto(sock, &c, 1, 0, (struct sockaddr *) &clientAddr, sizeof(clientAddr)) != 1) {
                    errorHandler("sendto() sent different number of bytes than expected!");
                    closeConnection(sock);
                    return 0;
                }
            }
        }
    }

}