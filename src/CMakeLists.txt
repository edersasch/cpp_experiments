cmake_minimum_required(VERSION 3.10.0)

if(EXPERIMENTS_ARHELPER_QT)
    add_subdirectory(arhelper_qt)
endif()

if(EXPERIMENTS_ARHELPER_QT AND EXPERIMENTS_FS_HISTORY_QT)
    add_subdirectory(ar_history_search_qt)
endif()

if(EXPERIMENTS_FS_FILTER_QT)
    add_subdirectory(fs_filter_qt)
endif()

if(EXPERIMENTS_FS_HISTORY_QT)
    add_subdirectory(fs_history_qt)
endif()

if(EXPERIMENTS_FS_FILTER_QT AND EXPERIMENTS_FS_HISTORY_QT)
    add_subdirectory(fs_history_search_qt)
endif()

if(EXPERIMENTS_ELEMENTARY_MATH_QML)
    add_subdirectory(elementary_math_qml)
    add_subdirectory(emvis1000_qml)
endif()

if(EXPERIMENTS_QTABLEVIEW_DEMO)
    add_subdirectory(qtableview_demo)
endif()

if(EXPERIMENTS_ARHELPER_QT OR EXPERIMENTS_FS_FILTER_QT OR EXPERIMENTS_QTABLEVIEW_DEMO)
    add_subdirectory(qt_utilities)
endif()

if(EXPERIMENTS_TIDDLER_INSPECTOR_QT)
    add_subdirectory(tiddlerstore)
endif()

if(EXPERIMENTS_TIDDLER_INSPECTOR_QT)
    add_subdirectory(tiddlerstore_qt)
endif()

if(EXPERIMENTS_TIDDLER_INSPECTOR_QT)
    add_subdirectory(tiddler_inspector_qt)
endif()

if(EXPERIMENTS_SDL AND SDL2_FOUND AND SDL2IMAGE_FOUND)
    add_subdirectory(sdl)
endif()
