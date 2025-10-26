cmake_minimum_required(VERSION 3.16)

# Shared logic compiled once
add_library(ClientBase STATIC
    ../mq_wrapper.cpp
    mainWindow.cpp
)

target_link_libraries(ClientBase PUBLIC Qt6::Widgets rt)
target_include_directories(ClientBase PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(client1)
add_subdirectory(client2)
add_subdirectory(client3)