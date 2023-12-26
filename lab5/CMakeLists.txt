add_library(lab5_lib src/base.cpp src/computer_base.cpp src/manager_base.cpp)

target_include_directories(lab5_lib PUBLIC ${CMAKE_SOURCE_DIR}/lib/include ${CMAKE_SOURCE_DIR}/lab5/include)

if (DEFINED ENV{ZMQ_LIB_PATH})
    set(ZMQ_LIB_PATH $ENV{ZMQ_LIB_PATH})
else()
    message("WARNING : ZMQ_LIB_PATH is not specified!")
    set(ZMQ_LIB_PATH ${CMAKE_SOURCE_DIR}/../zmq_build/lib/libzmq.a)
endif()

target_link_libraries(lab5_lib PUBLIC ${ZMQ_LIB_PATH})

add_executable(manager src/manager.cpp)
add_executable(computer src/computer.cpp)

target_link_libraries(manager PUBLIC lab5_lib)
target_link_libraries(computer PUBLIC lab5_lib)
