//
// Created by Samuel He on 2025/9/29.
//

#pragma once
#include <QWidget>
#include <QGraphicsEffect>
#include "../Models/BoardManager.h"

#ifndef GOMOKU_BOARDWIDGET_H
#define GOMOKU_BOARDWIDGET_H

class BoardWidget : public QWidget {
    Q_OBJECT

public:
    explicit BoardWidget(QWidget *parent = nullptr);
    void resetGame() { board.resetGame(); winner = EMPTY; update(); }

protected:
    void paintEvent(QPaintEvent *event) override;

    void mousePressEvent(QMouseEvent *event) override;

private:
    BoardManager board;

    [[nodiscard]] int boardCellSize() const;

    const int borderWidth = 3;
    const int linesWidth = 2;

    // Cached layout values (calculated per paint event)
    int cellSize = 0;
    int borderSize = 0;
    int startX = 0;
    int startY = 0;
    int winner = EMPTY; // EMPTY if no winner, BLACK if black wins, WHITE if white wins

    void calculateBoardLayout();
    [[nodiscard]] int criticalPointRadius() const;

    static void drawStone(QPainter &painter, QPointF center, double radius, bool isBlack);
    void drawBorders(QPainter &painter) const;
    void drawGridLines(QPainter &painter) const;
    void drawCriticalPoints(QPainter &painter) const;
    void drawStones(QPainter &painter) const;
};

#endif //GOMOKU_BOARDWIDGET_H
