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

if (EXPERIMENTS_FS_FILTER_QT AND EXPERIMENTS_FS_HISTORY_QT)
    add_subdirectory(fs_history_search_qt)
endif()

if (EXPERIMENTS_ELEMENTARY_MATH_QML)
    add_subdirectory(elementary_math_qml)
    add_subdirectory(emvis1000_qml)
endif()

if(EXPERIMENTS_ARHELPER_QT OR EXPERIMENTS_FS_FILTER_QT)
    add_subdirectory(qt_utilities)
endif()
