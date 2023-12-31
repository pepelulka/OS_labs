include(FetchContent)

FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG        release-1.11.0
)

FetchContent_MakeAvailable(googletest)
add_library(GTest::GTest INTERFACE IMPORTED)
target_link_libraries(GTest::GTest INTERFACE gtest_main)

set(CMAKE_CXX_STANDARD 17)

add_executable(lab1_test lab1_test.cpp)
target_link_libraries(lab1_test PRIVATE GTest::GTest lab1_lib)
target_include_directories(lab1_test PRIVATE ${CMAKE_SOURCE_DIR}/lab1/include)

# add_executable(lab2_test lab2_test.cpp)
# target_link_libraries(lab2_test PRIVATE GTest::GTest lab2_lib)
# target_include_directories(lab2_test PRIVATE ${CMAKE_SOURCE_DIR}/lab2/include)

# add_executable(lab3_test lab3_test.cpp)
# target_link_libraries(lab3_test PRIVATE GTest::GTest lab3_lib)
# target_include_directories(lab3_test PRIVATE ${CMAKE_SOURCE_DIR}/lab3/include)

add_executable(lab4_test1 lab4_test1.cpp)
target_include_directories(lab4_test1 PRIVATE ${CMAKE_SOURCE_DIR}/lab4/include)
target_link_libraries(lab4_test1 PRIVATE lib1 GTest::GTest)

add_executable(lab4_test2 lab4_test2.cpp)
target_include_directories(lab4_test2 PRIVATE ${CMAKE_SOURCE_DIR}/lab4/include)
target_link_libraries(lab4_test2 PRIVATE lib2 GTest::GTest)

add_executable(cp_test cp_test.cpp)
target_include_directories(cp_test PRIVATE ${CMAKE_SOURCE_DIR}/cp/include)
target_include_directories(cp_test PRIVATE ${CMAKE_SOURCE_DIR}/cp/lib)
target_link_libraries(cp_test PRIVATE cp_lib GTest::GTest)
