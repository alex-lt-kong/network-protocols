cmake_minimum_required(VERSION 3.10)
project(tcp)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")

set(TARGETS 1_client_1-send 1_client_n-sends 1_server_1-connection
        2_server_n-connections
        3_server_epoll)

foreach (TARGET ${TARGETS})
    add_executable(${TARGET} ${TARGET}.c)
    target_compile_options(${TARGET} PRIVATE ${CMAKE_C_FLAGS})
endforeach ()

