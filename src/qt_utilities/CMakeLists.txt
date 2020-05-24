project(qt_utilities)
include(prjqt)
include(prjlib)
target_link_libraries(${PROJECT_NAME}
    PRIVATE
    Qt5::Widgets
)
