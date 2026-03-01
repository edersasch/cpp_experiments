project(fs_history_qt)
include(prjqt)
include(prjlib)
target_link_libraries(${PROJECT_NAME}
    PRIVATE
    Qt6::Gui
    Qt6::Widgets
)
