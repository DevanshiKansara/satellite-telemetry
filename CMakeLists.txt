cmake_minimum_required(VERSION 3.20)
project(satellite_telemetry CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_compile_options(-Wall -Wextra -Wpedantic)

add_executable(onboard onboard/main.cpp onboard/packet.cpp)
target_include_directories(onboard PRIVATE onboard)
