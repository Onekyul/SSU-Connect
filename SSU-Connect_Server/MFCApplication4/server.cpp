#include "pch.h"
#include "server.h"
#include <winsock2.h>
#include <iostream>
#include <vector>
#include <thread>

#pragma comment(lib, "ws2_32.lib")

#define PORT 5000
#define BUFFER_SIZE 256

std::vector<SOCKET> clients; // 클라이언트 소켓 저장


// 클라이언트 메시지를 브로드캐스트
void broadcastMessage(const char* message, SOCKET sender) {

    for (SOCKET client : clients) {
        if (client != sender) { // 자기 자신에게는 메시지 전송 안 함
            send(client, message, strlen(message), 0);
        }
    }
}

// 클라이언트 처리 함수
void handleClient(SOCKET clientSocket) {
    char buffer[BUFFER_SIZE];
    while (true) {
        memset(buffer, 0, BUFFER_SIZE);
        int bytesReceived = recv(clientSocket, buffer, BUFFER_SIZE - 1, 0);

        if (bytesReceived <= 0) {
            std::cerr << "클라이언트 연결 종료.\n";
            closesocket(clientSocket);

            // 클라이언트 목록에서 제거

            clients.erase(std::remove(clients.begin(), clients.end(), clientSocket), clients.end());
            break;
        }

        std::cout << "받은 메시지: " << buffer << std::endl;

        // 받은 메시지를 다른 클라이언트에게 브로드캐스트
        broadcastMessage(buffer, clientSocket);
    }
}

// 메인 서버 함수
int main() {
    WSADATA wsaData;
    SOCKET serverSocket;
    SOCKADDR_IN serverAddr;

    // WinSock 초기화
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup 실패\n";
        return 1;
    }

    // 서버 소켓 생성
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        std::cerr << "소켓 생성 실패\n";
        WSACleanup();
        return 1;
    }

    // 서버 주소 설정
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);

    // 소켓 바인딩
    if (bind(serverSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "바인딩 실패\n";
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    // 리스닝 시작
    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "리스닝 실패\n";
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "서버가 포트 " << PORT << "에서 시작되었습니다.\n";

    while (true) {
        SOCKADDR_IN clientAddr;
        int clientAddrSize = sizeof(clientAddr);
        SOCKET clientSocket = accept(serverSocket, (SOCKADDR*)&clientAddr, &clientAddrSize);

        if (clientSocket == INVALID_SOCKET) {
            std::cerr << "클라이언트 연결 실패\n";
            continue;
        }

        // 클라이언트 목록에 추가
        {

            clients.push_back(clientSocket);
        }

        // 클라이언트 처리 스레드 시작
        std::thread clientThread(handleClient, clientSocket);
        clientThread.detach();
    }

    // 서버 소켓 닫기
    closesocket(serverSocket);
    WSACleanup();
    return 0;
}
