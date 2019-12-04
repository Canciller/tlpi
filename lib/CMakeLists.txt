set(SOURCE_FILES
    error_functions.cpp
    alt_functions.cpp
    get_num.cpp
    )

add_library(tlpi ${SOURCE_FILES})

target_include_directories(tlpi PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
