cmake_minimum_required(VERSION 3.3)
project(Server_UDP)

link_libraries(wsock32)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES server.c)
add_executable(Server_UDP ${SOURCE_FILES})