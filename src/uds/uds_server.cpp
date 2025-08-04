#include "uds/uds_server.hpp"
#include <iostream>

std::vector<uint8_t> flashBuffer;
bool downloadRequested = false;

void UDSServer::start()
{
    std::cout << "UDS Server started (stub).\n";
}

UDSServer::UDSServer()
{
    initializeHandlers();
}
void UDSServer::initializeHandlers()
{
    serviceHandlers[0x22] = std::bind(&UDSServer::handle0x22, this, std::placeholders::_1, std::placeholders::_2);
    serviceHandlers[0x2E] = std::bind(&UDSServer::handle0x2E, this, std::placeholders::_1, std::placeholders::_2);
    serviceHandlers[0x29] = std::bind(&UDSServer::handle0x29, this, std::placeholders::_1, std::placeholders::_2);
    serviceHandlers[0x31] = std::bind(&UDSServer::handle0x31, this, std::placeholders::_1, std::placeholders::_2);
}

void UDSServer::handleUDSRequest(const std::vector<uint8_t> &req, std::vector<uint8_t> &res)
{
    if (req.empty())
    {
        res = {0x7F, 0x00, 0x13}; // Invalid format NRC
        return;
    }

    uint8_t sid = req[0];
    auto handler = serviceHandlers.find(sid);
    if (handler != serviceHandlers.end())
    {
        handler->second(req, res);
    }
    else
    {
        res = {0x7F, sid, 0x11}; // Service Not Supported
    }
}

void UDSServer::handle0x22(const std::vector<uint8_t> &req, std::vector<uint8_t> &res)
{
    if (req.size() < 3)
    {
        res = {0x7F, 0x22, 0x13}; // Incorrect message length
        return;
    }

    uint16_t did = (req[1] << 8) | req[2];
    std::lock_guard<std::mutex> lock(dataMutex);

    if (didStore.find(did) != didStore.end())
    {
        res = {0x62, req[1], req[2]};
        res.insert(res.end(), didStore[did].begin(), didStore[did].end());
    }
    else
    {
        res = {0x7F, 0x22, 0x31}; // Request out of range
    }
}
void UDSServer::handle0x2E(const std::vector<uint8_t> &req, std::vector<uint8_t> &res)
{
    if (req.size() < 4)
    {
        res = {0x7F, 0x2E, 0x13}; // Incorrect length
        return;
    }

    uint16_t did = (req[1] << 8) | req[2];
    std::vector<uint8_t> data(req.begin() + 3, req.end());

    std::lock_guard<std::mutex> lock(dataMutex);
    didStore[did] = data;

    res = {0x6E, req[1], req[2]};
}
void UDSServer::handle0x29(const std::vector<uint8_t> &req, std::vector<uint8_t> &res)
{
    if (req.size() < 2)
    {
        res = {0x7F, 0x29, 0x13};
        return;
    }

    uint8_t subfunction = req[1];
    if (subfunction == 0x05 || subfunction == 0x06)
    {
        res = {0x69, subfunction};
    }
    else
    {
        res = {0x7F, 0x29, 0x12}; // Subfunction not supported
    }
}
void UDSServer::handle0x31(const std::vector<uint8_t> &req, std::vector<uint8_t> &res)
{
    if (req.size() < 4)
    {
        res = {0x7F, 0x31, 0x13};
        return;
    }

    uint8_t subfunction = req[1];
    uint16_t routineId = (req[2] << 8) | req[3];

    // Simulated routine: Always successful
    res = {0x71, subfunction, req[2], req[3]};
}

std::vector<uint8_t> UDSServer::processRequest(const std::vector<uint8_t> &request)
{
    if (request.empty())
        return {0x7F, 0x00, 0x13}; // invalid format

    
    uint8_t sid = request[0];

      if (sid == 0x10) {
        if (request.size() < 2) {
            // Missing session type byte
            return {0x7F, 0x10, 0x13}; // Incorrect message length
        }

        uint8_t sessionType = request[1];
        switch (sessionType) {
            case 0x01: currentSession = DEFAULT_SESSION; break;
            case 0x02: currentSession = PROGRAMMING_SESSION; break;
            case 0x03: currentSession = EXTENDED_SESSION; break;
            default:
                return {0x7F, 0x10, 0x12}; // Sub-function not supported
        }

        // Positive response: 0x50 is positive response to 0x10
        return {0x50, sessionType, 0x00, 0x32};  // Includes timing info
    }

     // Only allow some services in specific sessions
    if ((sid == 0x22 || sid == 0x2E || sid == 0x29 || sid == 0x31) && currentSession != EXTENDED_SESSION) {
        return {0x7F, sid, 0x7E};  // NRC 0x7E = "Service not supported in active session"
    }

    if ((sid == 0x34 || sid == 0x36 || sid == 0x37) && currentSession != PROGRAMMING_SESSION) {
        return {0x7F, sid, 0x7E};
    }

    switch (sid)
    {
    case 0x22:
    { // ReadDataByIdentifier
        if (request.size() != 3)
            return {0x7F, 0x22, 0x13};
        uint8_t did1 = request[1], did2 = request[2];
        if (did1 == 0xF1 && did2 == 0x90)
            return {0x62, did1, did2, 0x12, 0x34}; // mock data
        else
            return {0x7F, 0x22, 0x31}; // Request out of range
    }

    case 0x2E:
    { // WriteDataByIdentifier
        if (request.size() < 4)
            return {0x7F, 0x2E, 0x13};
        return {0x6E, request[1], request[2]}; // echo DID
    }

    case 0x29:
    { // CommunicationControl
        if (request.size() < 2)
            return {0x7F, 0x29, 0x13};
        return {0x69, request[1]};
    }

    case 0x31:
    { // RoutineControl
        if (request.size() < 4)
            return {0x7F, 0x31, 0x13};
        return {0x71, request[1], request[2], request[3]};
    }
    case 0x34:
    { // RequestDownload
        if (request.size() < 4)
        {
            return {0x7F, 0x34, 0x13}; // Incorrect Message Length
        }
        downloadRequested = true;
        flashBuffer.clear();

        // Positive response: SID + length format identifier
        return {0x74, 0x20, 0x00, 0x40}; // Example: max 0x4000 bytes
    }

    case 0x36:
    { // TransferData
        if (!downloadRequested){
            return {0x7F, 0x36, 0x24}; // Request Sequence Error
        }
        if (request.size() < 2){
            return {0x7F, 0x36, 0x13}; // Incorrect length
        }

        uint8_t blockSequenceCounter = request[1];
        flashBuffer.insert(flashBuffer.end(), request.begin() + 2, request.end());

        // Acknowledge with blockSequenceCounter
        return {0x76, blockSequenceCounter};
    }

    case 0x37:
    { // RequestTransferExit
        if (!downloadRequested)
        {
            return {0x7F, 0x37, 0x24}; // Request Sequence Error
        }

        downloadRequested = false;
        // ✅ Placeholder: Later persist flashBuffer to file or use n8n

        return {0x77}; // Positive Response
    }

    case 0x10:
    { // DiagnosticSessionControl
        if (request.size() < 2)
        {
            return {0x7F, 0x10, 0x13}; // Incorrect Message Length
        }

        uint8_t sessionType = request[1];

        switch (sessionType)
        {
        case 0x01:
            currentSession = DEFAULT_SESSION;
            break;
        case 0x02:
            currentSession = PROGRAMMING_SESSION;
            break;
        case 0x03:
            currentSession = EXTENDED_SESSION;
            break;
        default:
            return {0x7F, 0x10, 0x12}; // Sub-function not supported
        }

        return {0x50, sessionType, 0x00, 0x32}; // Positive response + timing (dummy)
    }

    default:
        return {0x7F, sid, 0x11}; // ServiceNotSupported
    }
}
