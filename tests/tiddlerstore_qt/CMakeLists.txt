project(tiddler_model_qt_test)
include(prjqt)
include(prjexe)
target_link_libraries(${PROJECT_NAME}
    PRIVATE
    qt_gtest_helper
    tiddlerstore_qt
    Qt5::Gui
    Qt5::Widgets
)
add_test(${PROJECT_NAME} ${PROJECT_NAME})
