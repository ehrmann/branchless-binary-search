cmake_minimum_required(VERSION 3.13)
project(branchless_binary_search C CXX)
set(CMAKE_C_STANDARD 11)
enable_testing()

#CMAKE_C_FLAGS_[DEBUG|RELEASE|RELWITHDEBINFO|MINSIZEREL]
#set(CMAKE_CXX_FLAGS "-O3 -Winline")

# extern
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/extern/googletest)

# src
add_library(bbsearch SHARED
        src/bbsearch.c
        src/bbsearch.h
        )
target_include_directories(bbsearch PUBLIC src)
target_compile_options(bbsearch PRIVATE "-O3" "-Winline")

# test
add_executable(test_bbsearch
        test/test_bbsearch.cpp
        )
target_include_directories(test_bbsearch
        PUBLIC ${gtest_SOURCE_DIR}/include
        PUBLIC ${gtest_SOURCE_DIR}
        )
target_link_libraries(test_bbsearch
        PUBLIC bbsearch
        PUBLIC gtest
        PUBLIC gtest_main
        )
add_test(NAME test_bbsearch
        COMMAND test_bbsearch
        )
