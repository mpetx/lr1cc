
add_library(lr1cc-core STATIC
  symbol.cc grammar.cc nfa.cc dfa.cc conflict.cc output.cc input-lexer.cc input-parser.cc cli.cc)

target_include_directories(lr1cc-core
  INTERFACE .)

add_executable(lr1cc
  main.cc)

target_link_libraries(lr1cc
  PRIVATE lr1cc-core)

install(TARGETS lr1cc DESTINATION bin)
