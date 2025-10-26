set(CMAKE_PREFIX_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../external/Qt/6.9.2/gcc_64")
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
cmake_minimum_required(VERSION 3.16)

#thorough debugging
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g -fno-omit-frame-pointer -fno-optimize-sibling-calls")
option(USE_ASAN "Enable AddressSanitizer" ON)
option(USE_TSAN "Enable ThreadSanitizer" OFF)
option(USE_UBSAN "Enable UndefinedBehaviorSanitizer" ON)
if (USE_ASAN)
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address")
endif()

if (USE_TSAN)
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=thread")
endif()

if (USE_UBSAN)
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=undefined")
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../external/asio/asio/include)

project(client_server LANGUAGES CXX)

cmake_policy(SET CMP0167 NEW)
find_package(Qt6 REQUIRED COMPONENTS Widgets)
find_package(Boost REQUIRED COMPONENTS system)
find_package(X11 REQUIRED)

add_executable(server server.cpp)
target_link_libraries(server 
    PRIVATE 
        Qt6::Widgets
        Boost::system
        Threads::Threads
        ${X11_LIBRARIES}
)
add_executable(client_auto client_auto.cpp)
target_link_libraries(client_auto
    PRIVATE 
        Qt6::Widgets
        Boost::system
        Threads::Threads
        ${X11_LIBRARIES}
)

add_executable(client_chat client_chat.cpp)
target_link_libraries(client_chat
    PRIVATE 
        Qt6::Widgets
        Boost::system
        Threads::Threads
)