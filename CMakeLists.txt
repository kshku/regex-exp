cmake_minimum_required(VERSION 3.15...3.31)

project(regex_exp)

add_executable(regexer)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(regexer PRIVATE RE_DEBUG)
endif()

add_subdirectory(src)

set(SRCS
    test.c
)

target_sources(regexer PRIVATE ${SRCS})
