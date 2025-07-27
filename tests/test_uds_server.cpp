#include <gtest/gtest.h>
#include "uds/uds_server.hpp"

TEST(UDSServerTest, StubTest) {
    UDSServer server;
    EXPECT_NO_THROW(server.start());
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
