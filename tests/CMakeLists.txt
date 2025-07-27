add_executable(test_uds_server test_uds_server.cpp)
target_link_libraries(test_uds_server PRIVATE uds_server GTest::gtest_main)

# ✅ Register the test with CTest
include(GoogleTest)
gtest_discover_tests(test_uds_server)
