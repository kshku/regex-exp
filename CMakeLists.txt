cmake_minimum_required(VERSION 3.15...3.31)

project(regex_exp VERSION 0.0.4 DESCRIPTION "Building a regex engine for fun and learn" LANGUAGES C)

set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 17)

add_executable(regexer)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(regexer PRIVATE RE_DEBUG)
endif()

add_subdirectory(src)
add_subdirectory(docs)

set(gcc_clang_comp "$<COMPILE_LANG_AND_ID:C,Clang,GNU>")

# target_compile_features(regexer PRIVATE c_std_17)

target_compile_options(regexer PRIVATE "$<${gcc_clang_comp}:-Wall;-Wextra;-Wformat;-Wuninitialized>")

set(SRCS
    test.c
)

target_sources(regexer PRIVATE ${SRCS})
