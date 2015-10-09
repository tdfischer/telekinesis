cmake_minimum_required(VERSION 3.3)
project(telekinesis)

find_package(Qt5 REQUIRED COMPONENTS Core Widgets)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_subdirectory(src)
