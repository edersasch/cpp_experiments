project(emvis1000)
include(prjqt)
include(prjexe)
target_link_libraries(${PROJECT_NAME}
    PRIVATE
    Qt5::Quick
    elementary_math
)
