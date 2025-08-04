#include "uds/uds_tcp_server.hpp"
#include <iostream>
#include <unistd.h>
#include <cstring>
#include <arpa/inet.h>

UDSTCPServer::UDSTCPServer(int port) : port(port), running(false) {}

UDSTCPServer::~UDSTCPServer() {
    stop();
}

void UDSTCPServer::start() {
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        perror("Socket creation failed");
        return;
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);

    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("Bind failed");
        return;
    }

    if (listen(serverSocket, 5) < 0) {
        perror("Listen failed");
        return;
    }

    running = true;
    std::thread acceptThread(&UDSTCPServer::acceptClients, this);
    acceptThread.detach();
    std::cout << "[Server] Listening on port " << port << "...\n";
}

void UDSTCPServer::stop() {
    running = false;
    close(serverSocket);
    for (auto& t : clientThreads) {
        if (t.joinable()) t.join();
    }
}

void UDSTCPServer::acceptClients() {
    while (running) {
        sockaddr_in clientAddr{};
        socklen_t clientLen = sizeof(clientAddr);
        int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientLen);
        if (clientSocket < 0) {
            if (running) perror("Accept failed");
            continue;
        }

        std::cout << "[Server] Client connected.\n";
        clientThreads.emplace_back(&UDSTCPServer::handleClient, this, clientSocket);
    }
}

void UDSTCPServer::handleClient(int clientSocket) {
    char buffer[1024];
    while (running) {
        ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
        if (bytesRead <= 0) break;
        buffer[bytesRead] = '\0';

        std::cout << "[Client] Received: " << buffer << std::endl;
        // Future: decode UDS here
    }
    close(clientSocket);
    std::cout << "[Server] Client disconnected.\n";
}
