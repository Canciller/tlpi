cmake_minimum_required(VERSION 3.1...3.16)

project(linux-programming-interface
    VERSION 0.1
    LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

function(tlpi_executable name)
    add_executable(${name} ${name}.cpp)
    target_link_libraries(${name} tlpi)
    target_compile_options(${name} PRIVATE -g)
    #target_compile_definitions(${name} PUBLIC _FILE_OFFSET_BITS=64)
endfunction()

add_subdirectory(lib)
add_subdirectory(chapter4)
add_subdirectory(chapter5)
add_subdirectory(chapter6)
add_subdirectory(chapter7)
