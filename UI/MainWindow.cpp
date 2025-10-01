//
// Created by Samuel He on 2025/9/29.
//

#include "MainWindow.h"

#include <QVBoxLayout>
#include <QToolBar>

#include "BoardWidget.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    auto *board = new BoardWidget(this);
    setCentralWidget(board);
    toolbar = new QToolBar("Gomoku Game", this);
    resetAction = new QAction("Reset", this);
    connect(resetAction, &QAction::triggered, board, &BoardWidget::resetGame);
    toolbar->addAction(resetAction);
    addToolBar(Qt::TopToolBarArea, toolbar);

    setWindowTitle("Gomoku");
    resize(600, 600);
}