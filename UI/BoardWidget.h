//
// Created by Samuel He on 2025/9/29.
//

#pragma once
#include <QWidget>
#include <QGraphicsEffect>
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
    void resetGame();
    // Returns 0 if no winner, 1 if black wins, 2 if white wins
    [[nodiscard]]int winner() const { return _winner; }

protected:
    void paintEvent(QPaintEvent *event) override;

    void mousePressEvent(QMouseEvent *event) override;

private:
    struct BoardPosition {
        int row;
        int col;
    };

    [[nodiscard]] int boardCellSize() const;

    const int boardSize = 15; // 15x15 board
    const int borderWidth = 3;
    const int linesWidth = 2;

    // Cached layout values (calculated per paint event)
    int cellSize = 0;
    int borderSize = 0;
    int startX = 0;
    int startY = 0;

    void calculateBoardLayout();
    [[nodiscard]] int criticalPointRadius() const;

    // Critical points on a 15x15 board
    const std::vector<BoardPosition> criticalPoints = {
        {3, 3}, {3, 11}, {7, 7}, {11, 3}, {11, 11}
    };
    std::vector<std::vector<int>> board;
    bool blackTurn = true;
    int _winner = EMPTY; // 0: no winner, 1: black wins, 2: white wins

    static void drawStone(QPainter &painter, QPointF center, double radius, bool isBlack);
    void drawBorders(QPainter &painter) const;
    void drawGridLines(QPainter &painter) const;
    void drawCriticalPoints(QPainter &painter) const;
    void drawStones(QPainter &painter) const;
    
    void makeMove(BoardPosition position);
    // Check for a winner and writes to `_winner`
    void checkWinner();
};

#endif //GOMOKU_BOARDWIDGET_H
