//
// Created by Samuel He on 2025/9/29.
//

#pragma once
#include <QWidget>
#include <vector>

#ifndef GOMOKU_BOARDWIDGET_H
#define GOMOKU_BOARDWIDGET_H

#define EMPTY 0
#define BLACK 1
#define WHITE 2

class BoardWidget : public QWidget {
    Q_OBJECT
public:
    explicit BoardWidget(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private:
    struct BoardPosition {
        int row;
        int col;
    };
    [[nodiscard]] int boardCellSize();
    const int boardSize = 15; // 15x15 board
    const int borderWidth = 3;
    const int linesWidth = 2;
    int criticalPointRadius();
    // Critical points on a 15x15 board
    const std::vector<BoardPosition> criticalPoints = {
        {3, 3}, {3, 11}, {7, 7}, {11, 3}, {11, 11}
    };
    std::vector<std::vector<int>> board;
    bool blackTurn = true;
};

#endif //GOMOKU_BOARDWIDGET_H