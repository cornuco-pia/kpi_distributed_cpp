cmake_minimum_required(VERSION 3.16)

# need to list .h because qt's automoc doesn't see it otherwise
add_executable(Client3 main.cpp client3Window.h)
target_link_libraries(Client3 PRIVATE ClientBase)
