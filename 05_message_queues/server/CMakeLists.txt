cmake_minimum_required(VERSION 3.16)

add_executable(Server
    mainWindow.cpp
    ../mq_wrapper.cpp
)

target_link_libraries(Server PRIVATE Qt6::Widgets rt)