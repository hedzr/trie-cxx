message(STATUS "----------- src directory ------------")

#set(MY_FLEX_TARGET MyScanner)
#set(MY_BISON_TARGET MyParser)
#add_subdirectory(parser)


set(PROJECT_MACRO_CLI_NAME trie-cli)
set(PROJECT_MACRO_CLI_SHORT_NAME trie)
set(PROJECT_MACRO_CLI_MID_NAME trie-cli)
set(PROJECT_MACRO_CLI_PREFIX "TRIE_CLI")

#message(STATUS "---- Project ${PROJECT_MACRO_CLI_NAME} ------------")
#
#project(${PROJECT_MACRO_CLI_NAME}
#        VERSION ${VERSION}
#        DESCRIPTION "${PROJECT_MACRO_CLI_MID_NAME} / ${PROJECT_MACRO_CLI_NAME} - a cxx17 common template library."
#        LANGUAGES CXX)

set(${PROJECT_MACRO_CLI_PREFIX}_detail_header_files
    #${CMAKE_CURRENT_SOURCE_DIR}/include/ctk_cxx/detail/${PROJECT_MACRO_SHORT_NAME}-if.hh
)
set(${PROJECT_MACRO_CLI_PREFIX}_header_files
    #${CMAKE_CURRENT_SOURCE_DIR}/include/${PROJECT_MACRO_NAME}.hh
    #${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-version.hh
    #${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config.hh
    #${CMAKE_CURRENT_SOURCE_DIR}/include/${PROJECT_MACRO_NAME}/${PROJECT_MACRO_SHORT_NAME}.hh
    #${CMAKE_CURRENT_SOURCE_DIR}/include/${PROJECT_MACRO_NAME}/${PROJECT_MACRO_SHORT_NAME}-def.hh
    #${CMAKE_CURRENT_SOURCE_DIR}/include/${PROJECT_MACRO_NAME}/${PROJECT_MACRO_SHORT_NAME}-expr.hh
    #${CMAKE_CURRENT_SOURCE_DIR}/include/${PROJECT_MACRO_NAME}/${PROJECT_MACRO_SHORT_NAME}-fmt.hh
    #${CMAKE_CURRENT_SOURCE_DIR}/include/${PROJECT_MACRO_NAME}/${PROJECT_MACRO_SHORT_NAME}-if.hh
    #${CMAKE_CURRENT_SOURCE_DIR}/include/${PROJECT_MACRO_NAME}/${PROJECT_MACRO_SHORT_NAME}-port.hh
    #${CMAKE_CURRENT_SOURCE_DIR}/include/${PROJECT_MACRO_NAME}/${PROJECT_MACRO_SHORT_NAME}-term.hh
    #${CMAKE_CURRENT_SOURCE_DIR}/include/${PROJECT_MACRO_NAME}/mmstr.hh
    #        ${CMAKE_CURRENT_SOURCE_DIR}/parser/scanner.hh
)
set(${PROJECT_MACRO_CLI_PREFIX}_source_files
    ${PROJECT_MACRO_CLI_SHORT_NAME}-core.cc
    ${PROJECT_MACRO_CLI_NAME}.cc
)
set(${PROJECT_MACRO_CLI_PREFIX}_libs
    libs::trie
)

get_filename_component(INCDIR ${CMAKE_CURRENT_SOURCE_DIR}/include ABSOLUTE)

message(STATUS "---- define ${PROJECT_NAME} / ${PROJECT_MACRO_CLI_NAME} ------------")
define_cxx_executable_project(${PROJECT_MACRO_CLI_NAME}
    PREFIX ${PROJECT_MACRO_CLI_PREFIX}
    LIBRARIES ${${PROJECT_MACRO_CLI_PREFIX}_libs}
    SOURCES ${${PROJECT_MACRO_CLI_PREFIX}_source_files}
    INCLUDE_DIRECTORIES ${INCDIR}
)
enable_version_increaser(${PROJECT_MACRO_CLI_NAME} ${PROJECT_MACRO_NAME} ${PROJECT_MACRO_SHORT_NAME} ${PROJECT_MACRO_PREFIX})
message(STATUS "---- defined ${PROJECT_NAME} / ${PROJECT_MACRO_CLI_NAME} -----------")


##if (${AML_CXX_BUILD_TESTS_EXAMPLES} OR (${CMAKE_CURRENT_SOURCE_DIR} STREQUAL ${CMAKE_SOURCE_DIR}))
#if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/examples/")
#    enable_testing()
#    add_subdirectory(examples/)
#endif ()
#if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests/")
#    enable_testing()
#    add_subdirectory(tests/)
#endif ()
##endif ()


message(STATUS "----------- src directory END ------------")
