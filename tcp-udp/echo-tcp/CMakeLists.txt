cmake_minimum_required(VERSION 3.31)
project(echo_tcp)

set(CMAKE_CXX_STANDARD 20)

find_package(spdlog CONFIG REQUIRED)

add_executable(echo_tcp main.cpp)
target_link_libraries(echo_tcp PRIVATE spdlog::spdlog)
