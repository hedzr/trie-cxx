project(test
    VERSION ${VERSION}
    DESCRIPTION "tests - test apps for cxx20 library"
    LANGUAGES C CXX)

set(PROJECT_ARCHIVE_NAME ${PROJECT_NAME}s-${PROJECT_VERSION})
set(PROJECT_PREFIX "${PROJECT_NAME}") # prefix for the target executables
if(ENABLE_TESTS)
    # include(deps_inc_catch2)
    # include(deps_inc_fmt)
endif()

# set(USE_DEBUG ON) # to enable XXX_ENABLE_ASSERTIONS, XXX_ENABLE_THREAD_POOL_READY_SIGNAL, ...
# set(USE_DEBUG OFF)

# see: https://github.com/catchorg/Catch2/blob/devel/docs/cmake-integration.md
#find_package(Catch2 3 REQUIRED)
#find_package(Catch2 2)

# when using compiler with cmake multi-config feature, a special build type 'Asan' can be used for sanitizing test.
enable_sanitizer_for_multi_config()

set(CMAKE_VERBOSE_MAKEFILE ON) # print compiling command-line before executing it, for cmake debugging only
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # export all commands into a json file (compile_commands.json)
if(ENABLE_TESTS)

    define_test_program(new new.cc
        LIBRARIES Threads::Threads
        CXXDEFINITIONS Y1=1 Y2
    )

    define_test_program(chrono chrono.cc
        LIBRARIES libs::trie # Catch2::Catch2WithMain
        # CXXFLAGS -DTRIE_UNIT_TEST=1 -DTRIE_TEST_THREAD_POOL_DBGOUT=1
    )

    # define_test_program(process process.cc
    #     LIBRARIES libs::trie # Catch2::Catch2WithMain
    # )

endif()

# ######## catch2

if(ENABLE_TESTS)

    define_test_program(catch2-demo catch2-demo.cc
        LIBRARIES libs::trie Catch2::Catch2WithMain
        CXXSTANDARD 11

        # CXXDEFINITIONS CATCH_CONFIG_MAIN YES=1
    )

endif()

# ######### trie, ac automate machine

if(ENABLE_TESTS)

    # word frequency summarizer
    define_test_program(trie-1 trie-1.cc catch2-imp.cc
        LIBRARIES libs::trie Catch2::Catch2WithMain
        CXXSTANDARD 20
    )

    # store, hedzr/store (Golang) in c++, store-cxx
    define_test_program(trie-2 trie-2.cc catch2-imp.cc
        LIBRARIES libs::trie Catch2::Catch2WithMain
        CXXSTANDARD 20
    )

endif()

define_test_program(trie-2-test trie-2-main.cc
    LIBRARIES libs::trie
    CXXSTANDARD 20
)

# # cannot work on a INTERFACE library target
# add_custom_command(TARGET test-btree POST_BUILD
# 		COMMAND ${CMAKE_SOURCE_DIR}/cmake/versions-extract.py
# 		WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
# 		COMMENT "extracting version code and increase build-number part..."
# 		VERBATIM
# )

# # simple tests
# # need c++20
# define_test_program(enum enum.cc
# CXXSTANDARD 20)

# # based on c++11
# define_test_program(mon-obj mon-obj.cc
# CXXSTANDARD 11)

# define_test_program(big-decimal big-decimal.cc
# CXXSTANDARD 11)
message(STATUS "END of tests")
