# UDS Server Library  
add_library(uds_server STATIC uds/uds_server.cpp)  
target_include_directories(uds_server PUBLIC ${CMAKE_SOURCE_DIR}/include)  