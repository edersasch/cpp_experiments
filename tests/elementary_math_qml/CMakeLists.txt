project(elementary_math_qml_test)
include(prjqt)
include(prjexe)
target_link_libraries(${PROJECT_NAME}
    PRIVATE
    qt_gtest_helper
    elementary_math
    Qt5::Quick
)

add_test(${PROJECT_NAME} ${PROJECT_NAME})
