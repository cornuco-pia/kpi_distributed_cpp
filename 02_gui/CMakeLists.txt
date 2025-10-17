set(CMAKE_PREFIX_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../external/Qt/6.9.2/gcc_64")
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOMOC ON)

cmake_minimum_required(VERSION 3.16)
project(ui LANGUAGES CXX)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

qt_add_executable(ui
    ui.cpp
)

target_link_libraries(ui PRIVATE Qt6::Widgets)
