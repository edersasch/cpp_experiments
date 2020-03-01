cmake_minimum_required(VERSION 3.10.0)

if(EXPERIMENTS_FS_FILTER_QT)
    add_subdirectory(fs_filter_qt)
endif()

if(EXPERIMENTS_FS_HISTORY_QT)
    add_subdirectory(fs_history_qt)
endif()

if (EXPERIMENTS_FS_FILTER_QT AND EXPERIMENTS_FS_HISTORY_QT)
    add_subdirectory(fs_history_search_qt)
endif()
