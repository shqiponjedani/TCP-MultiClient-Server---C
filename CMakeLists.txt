cmake_minimum_required(VERSION 3.29)
project(TCP-MultiClient-Server---C C)

set(CMAKE_C_STANDARD 11)

add_executable(TCPServer TCPServer.c)
add_executable(TCPClient TCPClient.c)
