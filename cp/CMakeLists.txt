add_library(cp_lib src/job_dag.cpp src/job_exec.cpp)

add_executable(cp_main main.cpp)

target_include_directories(cp_lib PUBLIC include)
target_include_directories(cp_lib PUBLIC lib)

target_link_libraries(cp_main PUBLIC cp_lib)