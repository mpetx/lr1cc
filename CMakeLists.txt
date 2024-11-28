cmake_minimum_required(VERSION 3.22.1)

project(Lr1cc CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(lr1cc-core STATIC
  symbol.cc grammar.cc nfa.cc dfa.cc)

add_executable(lr1cc
  main.cc)

target_link_libraries(lr1cc
  PRIVATE lr1cc-core)

find_package(GTest)

if (GTest_FOUND)

  add_executable(test-lr1cc
    test-symbol.cc test-grammar.cc test-nfa.cc test-grammar-to-nfa.cc test-dfa.cc)

  target_link_libraries(test-lr1cc
    PRIVATE lr1cc-core GTest::gtest_main)

endif()
