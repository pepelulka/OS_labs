set(CMAKE_C_STANDARD 11)

add_executable(main src/utils.c src/parent.c main.c)
add_executable(child src/utils.c src/child.c)

target_include_directories(main PRIVATE include)
target_include_directories(child PRIVATE include)

