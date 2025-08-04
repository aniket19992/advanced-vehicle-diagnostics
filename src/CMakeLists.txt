# add_library(uds_server STATIC uds/uds_server.cpp)
# target_include_directories(uds_server PUBLIC ${CMAKE_SOURCE_DIR}/include)
# add_library(uds_server
#     uds/uds_server.cpp
#     uds/uds_tcp_server.cpp
# )

# target_include_directories(uds_server PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/uds)

add_library(uds_server
    uds/uds_server.cpp
    uds/uds_tcp_server.cpp
)

target_include_directories(uds_server PUBLIC ${CMAKE_SOURCE_DIR}/include)
