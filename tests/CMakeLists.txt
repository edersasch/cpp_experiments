cmake_minimum_required(VERSION 3.10.0)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(qt_gtest_helper)

if(EXPERIMENTS_FS_HISTORY_QT)
    add_subdirectory(fs_history_qt)
endif()

if(EXPERIMENTS_FS_FILTER_QT)
    add_subdirectory(fs_filter_qt)
endif()

if(EXPERIMENTS_ELEMENTARY_MATH_QML)
    add_subdirectory(elementary_math_qml)
endif()

if(EXPERIMENTS_TIDDLER_INSPECTOR_QT)
    add_subdirectory(tiddlerstore)
endif()

if(EXPERIMENTS_TIDDLER_INSPECTOR_QT)
    add_subdirectory(tiddlerstore_qt)
endif()
