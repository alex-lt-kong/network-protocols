cmake_minimum_required(VERSION 3.30)
project(boost_asio)

set(CMAKE_CXX_STANDARD 23)

find_package(spdlog REQUIRED)
find_package(Protobuf REQUIRED)
#include_directories(${Protobuf_INCLUDE_DIRS})

set(PROTO_FILES src/test_message.proto)
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${PROTO_FILES})

add_executable(sender src/sender.cpp ${PROTO_SRCS} ${PROTO_HDRS})
# generate C++ bindings residing in the build directory
target_include_directories(sender PUBLIC ${CMAKE_BINARY_DIR})
target_link_libraries(sender PRIVATE
        protobuf::libprotobuf
        spdlog::spdlog)

add_executable(receiver src/receiver.cpp ${PROTO_SRCS} ${PROTO_HDRS})
# generate C++ bindings residing in the build directory
target_include_directories(receiver PUBLIC ${CMAKE_BINARY_DIR})
target_link_libraries(receiver PRIVATE
        protobuf::libprotobuf
        spdlog::spdlog)
