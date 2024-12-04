project(examples
    VERSION ${VERSION}
    DESCRIPTION "examples - all examples apps for ticker-cxx library"
    LANGUAGES CXX)

set(PROJECT_ARCHIVE_NAME ${PROJECT_NAME}s-${PROJECT_VERSION})

find_package(Threads REQUIRED)

define_example_program(hello hello.cc
    LIBRARIES libs::trie)

define_example_program(values values.cc
    LIBRARIES libs::trie)

message(STATUS "END of examples")
