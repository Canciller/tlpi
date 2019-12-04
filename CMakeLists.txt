cmake_minimum_required(VERSION 3.1...3.16)

project(linux-programming-interface
    VERSION 0.1
    LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

function(executable name)
    add_executable(${name} ${name}/main.cpp)
    target_link_libraries(${name} tlpi)
endfunction()

add_subdirectory(lib)
add_subdirectory(chapter4)
