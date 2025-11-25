set(CMAKE_PREFIX_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../external/Qt/6.9.2/gcc_64")
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOMOC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
cmake_minimum_required(VERSION 3.16)


project(FileMapping LANGUAGES CXX)

find_package(Qt6 REQUIRED COMPONENTS Widgets)
find_package(Boost REQUIRED)

cmake_policy(SET CMP0167 NEW)

# --- Server ---
add_executable(server server.cpp)
target_link_libraries(server PRIVATE 
    Qt6::Widgets 
    pthread 
    rt
)

# --- Client 1 ---
add_executable(client1 client1.cpp)
target_link_libraries(client1 PRIVATE 
    Qt6::Widgets 
    pthread 
    rt
)

# --- Client 2 ---
add_executable(client2 client2.cpp)
target_link_libraries(client2 PRIVATE 
    Qt6::Widgets 
    pthread 
    rt
)

target_include_directories(server PRIVATE ${Boost_INCLUDE_DIRS})
target_include_directories(client1 PRIVATE ${Boost_INCLUDE_DIRS})
target_include_directories(client2 PRIVATE ${Boost_INCLUDE_DIRS})