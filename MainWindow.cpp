//
// Created by Samuel He on 2025/9/29.
//

#include "MainWindow.h"

#include <QPushButton>
#include <QVBoxLayout>

#include "BoardWidget.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    BoardWidget *board = new BoardWidget(this);
    setCentralWidget(board);

    setWindowTitle("Gomoku");
    resize(600, 600);
}