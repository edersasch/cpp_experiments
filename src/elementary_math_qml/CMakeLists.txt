project(elementary_math)
include(prjqt)
qtquick_compiler_add_resources(SRC_LIST vis_1000.qrc)
include(prjlib)
target_link_libraries(${PROJECT_NAME}
    PRIVATE
    Qt5::Quick
)
