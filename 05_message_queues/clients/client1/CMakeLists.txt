cmake_minimum_required(VERSION 3.16)

add_executable(Client1 main.cpp)
target_link_libraries(Client1 PRIVATE ClientBase)
