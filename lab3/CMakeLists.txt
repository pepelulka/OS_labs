add_library(lab3_lib
                src/parent.cpp
                src/util.cpp)

add_executable(lab3_child src/child.cpp src/util.cpp)
add_executable(lab3_main main.cpp)

target_link_libraries(lab3_main lab3_lib)

include_directories(${CMAKE_SOURCE_DIR}/lab3/include)

