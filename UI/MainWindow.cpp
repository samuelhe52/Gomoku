//
// Created by Samuel He on 2025/9/29.
//

#include "MainWindow.h"
#include "GameWidget.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    auto *gameWidget = new GameWidget(this);
    setCentralWidget(gameWidget);
    
    setWindowTitle("Gomoku");
    resize(600, 650); // Slightly taller for button
}