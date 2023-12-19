set(CMAKE_C_STANDARD 11)

add_library(lab1_lib src/utils.c src/parent.c)

add_executable(lab1_main main.c)
add_executable(lab1_child src/child.c)

target_link_libraries(lab1_main PRIVATE lab1_lib)
target_link_libraries(lab1_child PRIVATE lab1_lib)

target_include_directories(lab1_lib PRIVATE include)
target_include_directories(lab1_main PRIVATE include)
target_include_directories(lab1_child PRIVATE include)
