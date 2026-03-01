# if not found, add Qt installation dir (containing lib/cmake) to CMAKE_PREFIX_PATH
find_package(Qt6 REQUIRED COMPONENTS Widgets Quick)
qt_standard_project_setup()
set(CMAKE_AUTORCC ON)
#set(CMAKE_AUTOMOC ON)
#find_package(Qt5 5.12.0 REQUIRED COMPONENTS Core Gui Widgets Quick)
#find_package(Qt5QuickCompiler REQUIRED)
