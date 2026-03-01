project(fs_filter_qt)
include(prjqt)
include(prjlib)
target_link_libraries(${PROJECT_NAME}
    PRIVATE
    Qt6::Widgets
    qt_utilities
)
