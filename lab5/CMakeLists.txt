add_executable(manager src/manager.cpp)
add_executable(computer src/computer.cpp)

target_include_directories(manager PRIVATE ${CMAKE_SOURCE_DIR}/lib/include ${CMAKE_SOURCE_DIR}/lab5/include)
target_include_directories(computer PRIVATE ${CMAKE_SOURCE_DIR}/lib/include ${CMAKE_SOURCE_DIR}/lab5/include)

if (DEFINED ENV{ZMQ_LIB_PATH})
    set(ZMQ_LIB_PATH $ENV{ZMQ_LIB_PATH})
else()
    message("WARNING : ZMQ_LIB_PATH is not specified!")
    set(ZMQ_LIB_PATH ${CMAKE_SOURCE_DIR}/../zmq_build/lib/libzmq.a)
endif()

target_link_libraries(manager PRIVATE ${ZMQ_LIB_PATH})
target_link_libraries(computer PRIVATE ${ZMQ_LIB_PATH})
