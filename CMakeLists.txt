cmake_minimum_required(VERSION 3.3)
project(telekinesis)

find_package(Qt5 REQUIRED COMPONENTS Core Widgets)
find_package(PkgConfig REQUIRED)

pkg_search_module(GUPNP REQUIRED gupnp-1.0)
pkg_search_module(GOBJECT REQUIRED gobject-2.0)
pkg_search_module(GSSDP REQUIRED gssdp-1.0)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_subdirectory(src)
