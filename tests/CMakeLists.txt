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

add_executable(lab3_test lab3_test.cpp)

target_link_libraries(lab3_test PRIVATE GTest::GTest lab3_lib)
target_include_directories(lab3_test PRIVATE ${CMAKE_SOURCE_DIR}/lab3/include)
