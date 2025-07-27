# GoogleTest setup  
find_package(GTest REQUIRED)  

# Test executable  
add_executable(test_uds_server test_uds_server.cpp)  
target_link_libraries(test_uds_server uds_server GTest::GTest)  

# Add test  
enable_testing()  
add_test(NAME UDS_Server_Tests COMMAND test_uds_server)  