cmake_minimum_required(VERSION 3.20)

project(OS_LABS C CXX)

enable_testing()

if (MSVC)
    add_compile_options(/W4 /WX -g3)
else()
    add_compile_options(-Wall -Wextra -pedantic -Werror -g3)
endif()

include(FindThreads)

add_subdirectory(lab2)

add_subdirectory(tests)
