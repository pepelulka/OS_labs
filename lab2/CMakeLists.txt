add_library(lab2_lib
                src/task_queue.cpp
                src/merge_sort.cpp
                src/timer.cpp)

add_executable(main main.cpp)
target_include_directories(lab2_lib PUBLIC include)

target_link_libraries(main lab2_lib)
