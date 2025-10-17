set(CMAKE_PREFIX_PATH "/home/admin/Documents/code/distributed_cpp/external/Qt/6.9.2/gcc_64")
set(CMAKE_AUTORCC ON)

cmake_minimum_required(VERSION 3.16)
project(ui LANGUAGES CXX)

# set(CMAKE_CXX_STANDARD 17)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

qt_add_executable(ui
    ui.cpp
    assets.qrc
)

target_link_libraries(ui PRIVATE Qt6::Widgets)
