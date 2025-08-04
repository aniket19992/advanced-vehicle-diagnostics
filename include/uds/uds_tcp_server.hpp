#ifndef UDS_TCP_SERVER_HPP
#define UDS_TCP_SERVER_HPP

#include <thread>
#include <vector>
#include <atomic>
#include <netinet/in.h>

class UDSTCPServer {
public:
    UDSTCPServer(int port);
    ~UDSTCPServer();

    void start();
    void stop();

private:
    void acceptClients();
    void handleClient(int clientSocket);

    int serverSocket;
    int port;
    std::atomic<bool> running;
    std::vector<std::thread> clientThreads;
};

#endif // UDS_TCP_SERVER_HPP
