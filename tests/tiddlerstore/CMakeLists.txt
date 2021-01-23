project(tiddlerstore_test)
include(prjexe)
target_link_libraries(${PROJECT_NAME}
    PRIVATE
    qt_gtest_helper
    tiddlerstore
)
add_test(${PROJECT_NAME} ${PROJECT_NAME})
