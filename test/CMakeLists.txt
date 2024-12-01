
find_package(GTest)

if (NOT BUILD_TESTS)
  set(BUILD_TESTS OFF)
endif()

if (${GTest_FOUND} AND ${BUILD_TESTS})

  add_executable(test-lr1cc
    test-symbol.cc test-grammar.cc test-nfa.cc test-dfa.cc test-automaton.cc test-conflict.cc test-output.cc test-input.cc test-cli.cc)

  target_link_libraries(test-lr1cc
    PRIVATE lr1cc-core GTest::gtest_main)

endif()
