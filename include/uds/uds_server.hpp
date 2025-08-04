#pragma once
#include <unordered_map>
#include <functional>
#include <vector>
#include <mutex>

class UDSServer {
public:
    void start();
    void handleUDSRequest(const std::vector<uint8_t>& request, std::vector<uint8_t>& response);
    UDSServer();
    std::vector<uint8_t> processRequest(const std::vector<uint8_t>& request); 
   enum DiagnosticSessionType {
    DEFAULT_SESSION = 0x01,
    PROGRAMMING_SESSION = 0x02,
    EXTENDED_SESSION = 0x03
};
private:
    std::unordered_map<uint8_t, std::function<void(const std::vector<uint8_t>&, std::vector<uint8_t>&)>> serviceHandlers;
    std::unordered_map<uint16_t, std::vector<uint8_t>> didStore; // DID → Data
    std::mutex dataMutex;

    void handle0x22(const std::vector<uint8_t>& req, std::vector<uint8_t>& res);
    void handle0x2E(const std::vector<uint8_t>& req, std::vector<uint8_t>& res);
    void handle0x29(const std::vector<uint8_t>& req, std::vector<uint8_t>& res);
    void handle0x31(const std::vector<uint8_t>& req, std::vector<uint8_t>& res);
    void initializeHandlers();
    DiagnosticSessionType currentSession = DEFAULT_SESSION;


};
