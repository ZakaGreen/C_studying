#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>

#pragma comment(lib, "Ws2_32.lib")

#define DEFAULT_BUFLEN 512

int veta(char recvbuf[]) {
    if (strstr(recvbuf, "Wake up")) return 1;
    else if (strstr(recvbuf, "Hello")) return 2;
    else if (strstr(recvbuf, "Please disconnect")) return 3;
}

//char print_message() {
//    SetColor(10);
//    printf("Enter message: ");
//    iResult = send(ConnectSocket, sendbuf, (int)strlen(sendbuf), 0);
//    if (iResult == SOCKET_ERROR) {
//        printf("send failed: %d\n", WSAGetLastError());
//    }
//}

void Color(int color){
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
}

int main(int argc, char* argv[])
{
    WSADATA wsaData;
    int iResult;

    // Инициализация Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0)
    {
        printf("WSAStartup failed: %d\n", iResult);
        return 1;
    }

    struct addrinfo* result = NULL, * ptr = NULL;
    struct addrinfo hints;

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // IP и порт сервера
    iResult = getaddrinfo("147.175.115.34", "777", &hints, &result);
    if (iResult != 0)
    {
        printf("getaddrinfo failed: %d\n", iResult);
        WSACleanup();
        return 1;
    }
    else
        printf("getaddrinfo didn’t fail\n");

    SOCKET ConnectSocket = INVALID_SOCKET;
    ptr = result;

    ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
    if (ConnectSocket == INVALID_SOCKET)
    {
        printf("Error at socket(): %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }
    else
        printf("Error at socket DIDN’T occur\n");

    iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
    if (iResult == SOCKET_ERROR)
    {
        printf("Not connected to server\n");
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }

    printf("Connected to server!\n");

    // Цикл общения
    char sendbuf[DEFAULT_BUFLEN] = "?";
    char recvbuf[DEFAULT_BUFLEN];
    int recvbuflen = DEFAULT_BUFLEN;


    Color(10);
    printf("Enter message: ?\n");
    //fgets(sendbuf, DEFAULT_BUFLEN, stdin);

    // Удалить \n
    char* newline = strchr(sendbuf, '\n');
    if (newline != NULL) {
        *newline = '\0';
    }

    //if (strcmp(sendbuf, "quit") == 0) {
    //    break;
    //}

    // Отправка сообщения
    iResult = send(ConnectSocket, sendbuf, (int)strlen(sendbuf), 0);
    if (iResult == SOCKET_ERROR) {
        printf("send failed: %d\n", WSAGetLastError());
    }

    // Прием ответа
    while (1) {
        iResult = recv(ConnectSocket, recvbuf, recvbuflen - 1, 0);
        if (iResult > 0) {
            recvbuf[iResult] = '\0';
            newline = strchr(recvbuf, '\n');
            if (newline != NULL) {
                *newline = '\0';
            }
            Color(9);
            printf("Message received: \"%s\"\n", recvbuf);
            switch (veta(recvbuf)) {
            case 1:
                Color(10);
                printf("Enter message: 135686");
                strcpy(sendbuf, "135686");
                send(ConnectSocket, sendbuf, (int)strlen(sendbuf), 0);
            default: return 0;
            }

        }
        else if (iResult == 0) {
            printf("Connection closed\n");
            break;
        }
        else {
            printf("recv failed with error: %d\n", WSAGetLastError());
            break;
        }
    }

    // Завершение работы
    shutdown(ConnectSocket, SD_SEND);
    closesocket(ConnectSocket);
    freeaddrinfo(result);
    WSACleanup();

    return 0;
}
