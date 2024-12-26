# cmake_minimum_required(VERSION 3.0.0)
# project(trie-cxx VERSION 0.1.0)

# include(CTest)
# enable_testing()

# add_library(distance-cxx distance.cc)

# set(CPACK_PROJECT_NAME ${PROJECT_NAME})
# set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
# include(CPack)

# add_subdirectory(test)

cmake_minimum_required(VERSION 3.20)
# cmake_minimum_required(VERSION 3.9..3.25)

# ### INCLUDES ##########################################################
include(${CMAKE_SOURCE_DIR}/cmake/prerequisites.cmake)

# use_vcpkg()

# ### DEFINITIONS #######################################################
set(PROJECT_MACRO_NAME trie)
set(PROJECT_MACRO_SHORT_NAME trie)
set(PROJECT_MACRO_MID_NAME trie-cxx)
set(PROJECT_MACRO_PREFIX "TRIE")
set_property(GLOBAL PROPERTY PROJECT_MAINTAINER "hedzr <hedzr@duck.com>")
set_property(GLOBAL PROPERTY PROJ_HOMEPAGE "https://github.com/hedzr/trie-cxx")
set_property(GLOBAL PROPERTY PROJ_REPO_URL "https://github.com/hedzr/trie-cxx")

# ### OPTIONS ###########################################################

# option(OK "PL-OK" ON)

# option(${PROJECT_MACRO_PREFIX}_BUILD_TESTS_EXAMPLES "Build Test Examples OK" OFF)
# option(${PROJECT_MACRO_PREFIX}_BUILD_DOCS "Build Docs-OK" OFF)

# the following options are just used for executables
option(${PROJECT_MACRO_PREFIX}_ENABLE_ASSERTIONS "Enable Assertions" OFF)
option(${PROJECT_MACRO_PREFIX}_ENABLE_PRECONDITION_CHECKS "Enable Precondition Checks" OFF)
option(${PROJECT_MACRO_PREFIX}_ENABLE_THREAD_POOL_READY_SIGNAL "Enable ready signal actions in thread_pool template class" OFF)
option(${PROJECT_MACRO_PREFIX}_ENABLE_VERBOSE_LOG "Enable `dbg_verbose_debug` macro definition (TRACE MODE)" OFF)
option(${PROJECT_MACRO_PREFIX}_TEST_THREAD_POOL_DBGOUT "Enable `pool_debug` macro definition" OFF)
option(${PROJECT_MACRO_PREFIX}_UNIT_TEST "Enable the extra unit-tests" OFF)

# ### PROJECTS PREPARE ##################################################
if(ENABLE_TESTS)
    include(CTest)
endif()

find_package(Threads REQUIRED)

# ### PROJECTS ##########################################################
project(${PROJECT_MACRO_NAME}
    VERSION ${VERSION}
    DESCRIPTION "${PROJECT_MACRO_MID_NAME} - cxx20 common template library."
    LANGUAGES CXX)
debug_print_project_title()

# Enable C++ Standard 17/20 here. Change it on fist configuring, or update it with new value:
# cmake -S. -Bbuild -DCXX_STANDARD:STRING=20
set(CXX_STANDARD 20 CACHE STRING "Define The C++ Standard, default is 20")
enable_cxx_standard(CXX_STANDARD)

if(ENABLE_TESTS)
    include(load-catch2)
endif()

# when using compiler with cmake multi-config feature, a special build type 'Asan' can be used for sanitizing test.
enable_sanitizer_for_multi_config()

message(STATUS "---- define ${PROJECT_NAME} / ${PROJECT_MACRO_NAME} ------------")
define_cxx_library_project(${PROJECT_NAME}
    INTERFACE
    PREFIX ${PROJECT_MACRO_PREFIX}
    INSTALL # installable?
    PACK # CPack?
    GENERATE_CONFIG # generate config.h and version.h

    # BUILD_DOCS    # build docs with doxygen? 
    # CXXSTANDARD 20

    # SOURCES
    #   core.cc
    #   driver.cc

    HEADERS
    #   ${CMAKE_CURRENT_SOURCE_DIR}/include/${PROJECT_MACRO_NAME}-all.hh
    ${CMAKE_GENERATED_DIR}/${PROJECT_NAME}-version.hh
    ${CMAKE_GENERATED_DIR}/${PROJECT_NAME}-config.hh
	${CMAKE_CURRENT_SOURCE_DIR}/include/${PROJECT_MACRO_MID_NAME}.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/include/${PROJECT_MACRO_MID_NAME}/${PROJECT_MACRO_SHORT_NAME}-all.hh
)
message(STATUS "---- defined ${PROJECT_NAME} / ${PROJECT_MACRO_NAME} ------------")

add_subdirectory(src/)

# #### TESTS ############################################################

# if (${${PROJECT_MACRO_PREFIX}_BUILD_TESTS_EXAMPLES} OR (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR))
# enable_testing()
# add_subdirectory(examples/)
# add_subdirectory(tests/)
# endif ()

# # TPL ## TAIL ##########
message(STATUS "tail of top level :  Using default C++ Standard ${CXX_STANDARD}")

# apply_all_unit_tests(all_tests)     # invoke CTest unittests automatically.
# dump_feature_summary()              # tail print to cmake-debug/features.log
# env var CTEST_OUTPUT_ON_FAILURE=1 to display ctest error verbosely.
# ctest cmdline options: --verbose or --extra-verbose or --verbose-on-failure
# debug_print_value(DEBUG)
# debug_print_value(BUILD_DOCUMENTATION)
