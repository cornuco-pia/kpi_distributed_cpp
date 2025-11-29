set(CMAKE_PREFIX_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../external/Qt/6.9.2/gcc_64")
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
cmake_minimum_required(VERSION 3.16)
cmake_policy(SET CMP0167 NEW)

project(Pipes LANGUAGES CXX)

find_package(Qt6 COMPONENTS Widgets Network REQUIRED)

add_executable(lab07
    main.cpp
    common.h
    server.h
    client.h
)

target_link_libraries(lab07 PRIVATE Qt6::Widgets Qt6::Network)