cmake_minimum_required(VERSION 3.11.0)
project(experiments VERSION 0.1.0)

option(EXPERIMENTS_ARHELPER_QT                      "Archvie Helper"                                            ON)
option(EXPERIMENTS_FS_FILTER_QT                     "Filesystem Filter"                                         ON)
option(EXPERIMENTS_FS_HISTORY_QT                    "Filesystem History"                                        ON)
option(EXPERIMENTS_ELEMENTARY_MATH_QML              "Elementary Math"                                           ON)
option(EXPERIMENTS_QTABLEVIEW_DEMO                  "QTableView Demo"                                           ON)
option(EXPERIMENTS_TIDDLER_INSPECTOR_QT             "Tiddler Inspector"                                         ON)
option(EXPERIMENTS_SDL                              "SDL playground"                                            ON)

option(EXPERIMENTS_USE_EXTERNAL_JSON                "Use external nlohmann json library, download otherwise"    OFF)

option(EXPERIMENTS_BUILD_TESTS                      "Build unit tests"                                          ON)
option(EXPERIMENTS_COVERAGE_REPORT                  "Compile with coverage instrumentation"                     OFF)

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

if(EXPERIMENTS_TIDDLER_INSPECTOR_QT)
    if(EXPERIMENTS_USE_EXTERNAL_JSON)
        find_package(nlohmann_json 3.4.0 REQUIRED)
    else()
        include(FetchContent)
        FetchContent_Declare(nlohmann_json
            URL                 https://github.com/nlohmann/json/releases/download/v3.4.0/include.zip
            URL_HASH            SHA256=bfec46fc0cee01c509cf064d2254517e7fa80d1e7647fea37cf81d97c5682bdc
            UPDATE_DISCONNECTED 1
            QUIET
            )
        FetchContent_GetProperties(nlohmann_json)
        if(NOT nlohmann_json_POPULATED)
            FetchContent_Populate(nlohmann_json)
        endif()
        include_directories(${nlohmann_json_SOURCE_DIR})
    endif()
endif()

if(EXPERIMENTS_SDL)
    include(FindPkgConfig)
    pkg_search_module(SDL2 sdl2>=2.0.10)
    pkg_search_module(SDL2IMAGE SDL2_image>=2.0.5)
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
