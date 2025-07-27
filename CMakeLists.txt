cmake_minimum_required(VERSION 3.10)
project(AdvancedVehicleDiagnostics)

set(CMAKE_CXX_STANDARD 17)

# 🧪 GoogleTest
find_package(GTest REQUIRED)

enable_testing()

add_subdirectory(src)
add_subdirectory(tests)
