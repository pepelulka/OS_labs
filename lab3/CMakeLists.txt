add_library(lab3_lib 
                src/parent.cpp
                src/util.cpp)

add_executable(child src/child.cpp src/util.cpp)
add_executable(main main.cpp)

target_link_libraries(main lab3_lib)

include_directories(${CMAKE_SOURCE_DIR}/lab3/include)

