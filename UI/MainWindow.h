//
// Created by Samuel He on 2025/9/29.
//

#pragma once
#include <QMainWindow>
#include <QToolBar>

#ifndef GOMOKU_MAINWINDOW_H
#define GOMOKU_MAINWINDOW_H

class BoardWidget;
class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
private:
    QToolBar *toolbar{};
    QAction *resetAction{};
};

#endif //GOMOKU_MAINWINDOW_H