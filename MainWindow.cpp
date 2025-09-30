//
// Created by Samuel He on 2025/9/29.
//

#include "MainWindow.h"

#include <QPushButton>
#include <QVBoxLayout>
#include <QToolBar>
#include <QAction>

#include "BoardWidget.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    BoardWidget *board = new BoardWidget(this);
    setCentralWidget(board);
    QToolBar *toolbar = new QToolBar("Gomoku Game", this);
    QAction *resetAction = new QAction("Reset", this);
    connect(resetAction, &QAction::triggered, board, &BoardWidget::resetGame);
    toolbar->addAction(resetAction);
    addToolBar(Qt::TopToolBarArea, toolbar);

    setWindowTitle("Gomoku");
    resize(600, 600);
}