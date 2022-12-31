cmake_minimum_required(VERSION 3.18)
add_executable(producer producer.c)

find_library(KAFKAA_LIB rdkafka)
if(NOT KAFKAA_LIB)
  message(FATAL_ERROR "rdkafka library not found: apt install librdkafka-dev")
endif()


target_link_libraries(producer rdkafka)