add_library(lab2_lib
                src/thread_pool.cpp
                src/merge_sort.cpp
                src/timer.cpp
                src/generator.cpp)

add_executable(main main.cpp)
target_include_directories(lab2_lib PUBLIC include)

target_link_libraries(main lab2_lib)
