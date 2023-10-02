set(CMAKE_C_STANDARD 11)

add_library(lab1_lib src/utils.c src/parent.c)

add_executable(main main.c)
add_executable(child src/child.c)

target_link_libraries(main PRIVATE lab1_lib)
target_link_libraries(child PRIVATE lab1_lib)

target_include_directories(lab1_lib PRIVATE include)
target_include_directories(main PRIVATE include)
target_include_directories(child PRIVATE include)
