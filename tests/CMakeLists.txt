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

add_executable(lab4_test1 lab4_test1.cpp)
target_include_directories(lab4_test1 PRIVATE ${CMAKE_SOURCE_DIR}/lab4/include)
target_link_libraries(lab4_test1 PRIVATE lib1 GTest::GTest)

add_executable(lab4_test2 lab4_test2.cpp)
target_include_directories(lab4_test2 PRIVATE ${CMAKE_SOURCE_DIR}/lab4/include)
target_link_libraries(lab4_test2 PRIVATE lib2 GTest::GTest)
