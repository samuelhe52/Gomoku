cmake_minimum_required(VERSION 3.31)
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
        UI/MainWindow.cpp
        UI/MainWindow.h
        UI/GameWidget.cpp
        UI/GameWidget.h
        UI/BoardWidget.cpp
        UI/BoardWidget.h
        Models/BoardManager.cpp
        Models/BoardManager.h
        Models/GameManager.cpp
        Models/GameManager.h
        Models/GomokuAI.cpp
        Models/GomokuAI.h)
target_link_libraries(Gomoku
        Qt::Core
        Qt::Gui
        Qt::Widgets
)

