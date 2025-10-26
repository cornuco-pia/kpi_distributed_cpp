cmake_minimum_required(VERSION 3.16)

add_executable(Client2 main.cpp)
target_link_libraries(Client2 PRIVATE ClientBase)
