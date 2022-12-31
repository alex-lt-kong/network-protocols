cmake_minimum_required(VERSION 3.18)


find_library(KAFKAA_LIB rdkafka)
if(NOT KAFKAA_LIB)
  message(FATAL_ERROR "rdkafka library not found: apt install librdkafka-dev")
endif()

add_executable(producer producer.c)
add_executable(consumer consumer.c)
target_link_libraries(producer rdkafka)
target_link_libraries(consumer rdkafka)
