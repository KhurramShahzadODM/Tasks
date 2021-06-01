cmake_minimum_required(VERSION 3.19)
project(Task1)

set(CMAKE_CXX_STANDARD 14)

add_executable(Task1 main.cpp)
target_link_libraries(Task1
        -lpthread
        -lcurl)