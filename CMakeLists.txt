cmake_minimum_required(VERSION 4.0)
project(Gomoku)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)


find_package(Qt6 COMPONENTS
        Core
        Gui
        Widgets
        REQUIRED)

add_executable(Gomoku main.cpp
        MainWindow.cpp
        MainWindow.h
        BoardWidget.cpp
        BoardWidget.h)
target_link_libraries(Gomoku
        Qt::Core
        Qt::Gui
        Qt::Widgets
)

