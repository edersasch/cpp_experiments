project(emvis1000)
include(prjqt)
include(prjexe)
target_link_libraries(${PROJECT_NAME}
    PRIVATE
    Qt5::Quick
    elementary_math
)
install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
install(FILES ${CMAKE_SOURCE_DIR}/data/emvis1000.desktop DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/applications)
install(FILES ${CMAKE_SOURCE_DIR}/data/emvis1000.svg DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/apps)
