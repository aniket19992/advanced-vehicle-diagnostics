cmake_minimum_required(VERSION 3.12)  
project(AdvancedVehicleDiagnostics VERSION 1.0.0 LANGUAGES CXX)  

# C++17 required  
set(CMAKE_CXX_STANDARD 17)  
set(CMAKE_CXX_STANDARD_REQUIRED ON)  

# Include directories  
include_directories(include)  

# Add subdirectories  
add_subdirectory(src)  
add_subdirectory(tests)  