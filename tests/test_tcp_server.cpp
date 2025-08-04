#include <gtest/gtest.h>
#include "uds/uds_tcp_server.hpp"
#include <thread>
#include <chrono>

TEST(TCPServerTest, StartsAndStops) {
    UDSTCPServer server(13400);
    server.start();
    std::this_thread::sleep_for(std::chrono::seconds(1));
    server.stop();

    SUCCEED(); // No crash = pass
}
