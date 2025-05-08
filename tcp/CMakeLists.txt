cmake_minimum_required(VERSION 3.10)
project(tcp)

# Set the compiler
#set(CMAKE_C_COMPILER clang)

set(TARGETS 1_tcp-client 1_tcp-client-multiple-sends 1_tcp-server_single-connection
        2_tcp-server_n-connections
        3_tcp-server_epoll)

foreach (TARGET ${TARGETS})
    add_executable(${TARGET} ${TARGET}.c)
    target_compile_options(${TARGET} PRIVATE ${CMAKE_C_FLAGS})
endforeach ()

