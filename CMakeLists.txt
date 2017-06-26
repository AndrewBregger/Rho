cmake_minimum_required (VERSION 2.6)
project(Rho)
include_directories("$(PROJECT_SOURCE_DIR)/src")
add_subdirectory(src)

add_executable(rhoc main.cpp)
