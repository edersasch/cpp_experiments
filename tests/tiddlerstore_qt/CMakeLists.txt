project(tiddler_model_qt_test)
include(prjqt)
include(prjexe)
target_link_libraries(${PROJECT_NAME}
    PRIVATE
    qt_gtest_helper
    tiddlerstore_qt
    Qt6::Gui
    Qt6::Widgets
)
add_test(${PROJECT_NAME} ${PROJECT_NAME})
