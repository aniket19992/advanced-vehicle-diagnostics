#include <gtest/gtest.h>
#include "uds/uds_server.hpp"

// --------------------------------------------------
// 🧰 Session Helpers
// --------------------------------------------------
void switchToExtendedSession(UDSServer& server) {
    server.processRequest({0x10, 0x03});
}

void switchToProgrammingSession(UDSServer& server) {
    server.processRequest({0x10, 0x02});
}

// --------------------------------------------------
// ✅ Stub Test for Compilation Validation
// --------------------------------------------------
TEST(UDSServerTest, StubTest) {
    UDSServer server;
    EXPECT_NO_THROW(server.start());
}

// --------------------------------------------------
// ✅ VALID REQUEST TEST CASES
// --------------------------------------------------
TEST(UDSServerTest_ReadDataByIdentifier_ValidDID, ReturnsPositiveResponse) {
    UDSServer server;
    switchToExtendedSession(server);
    std::vector<uint8_t> response = server.processRequest({0x22, 0xF1, 0x90});
    EXPECT_EQ(response[0], 0x62);
}

TEST(UDSServerTest_WriteDataByIdentifier_ValidWrite, ReturnsPositiveResponse) {
    UDSServer server;
    switchToExtendedSession(server);
    std::vector<uint8_t> response = server.processRequest({0x2E, 0xF1, 0x90, 0xAA});
    EXPECT_EQ(response[0], 0x6E);
}

TEST(UDSServerTest_CommunicationControl_EnableRxAndTx, ReturnsPositiveResponse) {
    UDSServer server;
    switchToExtendedSession(server);
    std::vector<uint8_t> response = server.processRequest({0x29, 0x00});
    EXPECT_EQ(response[0], 0x69);
}

TEST(UDSServerTest_RoutineControl_StartRoutineSuccess, ReturnsPositiveResponse) {
    UDSServer server;
    switchToExtendedSession(server);
    std::vector<uint8_t> response = server.processRequest({0x31, 0x01, 0x01, 0x02});
    EXPECT_EQ(response[0], 0x71);
}

// --------------------------------------------------
// ❌ MALFORMED REQUESTS & INVALID DIDs
// --------------------------------------------------
TEST(UDSServerTest_ReadDataByIdentifier_InvalidDID, ReturnsRequestOutOfRange) {
    UDSServer server;
    switchToExtendedSession(server);
    std::vector<uint8_t> response = server.processRequest({0x22, 0x00, 0x00});
    EXPECT_EQ(response, std::vector<uint8_t>({0x7F, 0x22, 0x31}));
}

TEST(UDSServerTest_ReadDataByIdentifier_MalformedRequest, ReturnsIncorrectMessageLength) {
    UDSServer server;
    std::vector<uint8_t> response = server.processRequest({0x22});
    EXPECT_EQ(response, std::vector<uint8_t>({0x7F, 0x22, 0x13}));
}

TEST(UDSServerTest_WriteDataByIdentifier_MalformedRequest, ReturnsIncorrectMessageLength) {
    UDSServer server;
    std::vector<uint8_t> response = server.processRequest({0x2E, 0xF1});
    EXPECT_EQ(response, std::vector<uint8_t>({0x7F, 0x2E, 0x13}));
}

TEST(UDSServerTest_CommunicationControl_MalformedRequest, ReturnsIncorrectMessageLength) {
    UDSServer server;
    std::vector<uint8_t> response = server.processRequest({0x29});
    EXPECT_EQ(response, std::vector<uint8_t>({0x7F, 0x29, 0x13}));
}

TEST(UDSServerTest_RoutineControl_MalformedRequest, ReturnsIncorrectMessageLength) {
    UDSServer server;
    std::vector<uint8_t> response = server.processRequest({0x31, 0x01});
    EXPECT_EQ(response, std::vector<uint8_t>({0x7F, 0x31, 0x13}));
}

// --------------------------------------------------
// ⛔ SEQUENCE VIOLATION TESTS
// --------------------------------------------------
TEST(UDSServerTest_TransferData_WithoutDownload, ReturnsRequestSequenceError) {
    UDSServer server;
    switchToProgrammingSession(server);
    std::vector<uint8_t> response = server.processRequest({0x36, 0x01, 0xAA, 0xBB});
    EXPECT_EQ(response, std::vector<uint8_t>({0x7F, 0x36, 0x24}));
}

TEST(UDSServerTest_TransferExit_WithoutDownload, ReturnsRequestSequenceError) {
    UDSServer server;
    switchToProgrammingSession(server);
    std::vector<uint8_t> response = server.processRequest({0x37});
    EXPECT_EQ(response, std::vector<uint8_t>({0x7F, 0x37, 0x24}));
}

// --------------------------------------------------
// 🔐 SESSION ENFORCEMENT TESTS
// --------------------------------------------------
TEST(UDSServerTest_RestrictedInDefaultSession, ReadDIDFails) {
    UDSServer server;
    std::vector<uint8_t> response = server.processRequest({0x22, 0xF1, 0x90});
    EXPECT_EQ(response, std::vector<uint8_t>({0x7F, 0x22, 0x7F}));
}

TEST(UDSServerTest_RequestDownload_NotAllowedInExtended, ReturnsNRC) {
    UDSServer server;
    switchToExtendedSession(server);
    std::vector<uint8_t> response = server.processRequest({0x34, 0x00, 0x44, 0x00, 0x10, 0x00});
    EXPECT_EQ(response, std::vector<uint8_t>({0x7F, 0x34, 0x7F}));
}

TEST(UDSServerTest_RequestDownload_AllowedInProgramming, ReturnsPositiveResponse) {
    UDSServer server;
    switchToProgrammingSession(server);
    std::vector<uint8_t> response = server.processRequest({0x34, 0x00, 0x44, 0x00, 0x10, 0x00});
    EXPECT_EQ(response[0], 0x74);
}
