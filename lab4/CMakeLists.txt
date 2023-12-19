add_library(lib1 SHARED src/lib1.c)
add_library(lib2 SHARED src/lib2.c)

set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/lab4/include)

target_include_directories(lib1 PUBLIC ${INCLUDE_DIR})
target_include_directories(lib2 PUBLIC ${INCLUDE_DIR})

add_executable(lab4_main1 main1.cpp)
add_executable(lab4_main2 main2.cpp)

target_link_libraries(lab4_main1 PRIVATE lib1)

target_include_directories(lab4_main2 PRIVATE ${INCLUDE_DIR})
