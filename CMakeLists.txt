cmake_minimum_required(VERSION 3.10.0)
project(experiments VERSION 0.1.0)

option(EXPERIMENTS_BUILD_TESTS "Build unit tests" ON)
OPTION(EXPERIMENTS_FS_FILTER_QT "Filesystem Filter" ON)
OPTION(EXPERIMENTS_FS_HISTORY_QT "Filesystem History" ON)
OPTION(EXPERIMENTS_ELEMENTARY_MATH_QML "Elementary Math" ON)
option(EXPERIMENTS_COVERAGE_REPORT "Compile with coverage instrumentation" OFF)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

if(EXPERIMENTS_BUILD_TESTS)
    set(MEMORYCHECK_SUPPRESSIONS_FILE "${CMAKE_SOURCE_DIR}/data/memcheck.supp")
    include(CTest)
endif()

include(ProcessorCount)
ProcessorCount(PROCESSOR_COUNT)
if(NOT PROCESSOR_COUNT)
    set(PROCESSOR_COUNT 1)
endif()

if(EXPERIMENTS_COVERAGE_REPORT)
    include(CodeCoverage)
    APPEND_COVERAGE_COMPILER_FLAGS()
    set(COVERAGE_LCOV_INCLUDES '${CMAKE_SOURCE_DIR}/src/*')
    SETUP_TARGET_FOR_COVERAGE_LCOV(
        NAME
        coverage_report
        EXECUTABLE
        ctest -j ${PROCESSOR_COUNT}
        )
endif()

set(DOXYGEN_EXCLUDE_PATTERNS "*/*.md")
include(FindDoxygen)
if(DOXYGEN_FOUND)
    doxygen_add_docs(
        apidoc
        ${CMAKE_SOURCE_DIR}
        )
endif()

include(GNUInstallDirs)

include(FindThreads)

include_directories(src)
include_directories(${PROJECT_BINARY_DIR}/include)
add_subdirectory(src)

if(EXPERIMENTS_BUILD_TESTS AND BUILD_TESTING)
    include(AddGoogleTest)
    add_subdirectory(tests)
endif()
