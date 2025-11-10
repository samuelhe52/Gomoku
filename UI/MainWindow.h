//
// Created by Samuel He on 2025/9/29.
//

#pragma once

#include <QMainWindow>

class BoardWidget;
class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
private:
    QToolBar *toolbar{};
    QAction *resetAction{};
};
