cmake_minimum_required(VERSION 3.22)
project(bigint VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE Debug)

include_directories(src)
include_directories(examples)

add_executable(repl
    src/main.cpp
)

add_executable(bigint_test
    examples/main.cpp
)

target_compile_options(repl PRIVATE -Wall -Wextra -pedantic -Werror -Wfatal-errors)
target_compile_options(bigint_test PRIVATE -Wall -Wextra -pedantic -Werror -Wfatal-errors)

