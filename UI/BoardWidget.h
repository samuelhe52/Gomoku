//
// Created by Samuel He on 2025/9/29.
//

#pragma once
#include <QWidget>
#include <QGraphicsEffect>
#include <QPainter>
#include <QMouseEvent>
#include "../Models/GameManager.h"

#ifndef GOMOKU_BOARDWIDGET_H
#define GOMOKU_BOARDWIDGET_H

class BoardWidget : public QWidget {
    Q_OBJECT

public:
    explicit BoardWidget(QWidget *parent = nullptr);
    
    // Attach the GameManager to query for board state
    void setGameManager(const GameManager *manager) { gameManager = manager; }
    
    // Trigger a repaint 
    void refresh() { update(); }

protected:
    void paintEvent(QPaintEvent *event) override;

    void mousePressEvent(QMouseEvent *event) override;

signals:
    // Emitted when the player clicks a valid intersection
    void cellSelected(int row, int col);

private:
    const GameManager *gameManager = nullptr;

    [[nodiscard]] int boardCellSize() const;

    const int borderWidth = 3;
    const int linesWidth = 2;

    // Cached layout values 
    int cellSize = 0;
    int borderSize = 0;
    int startX = 0;
    int startY = 0;
    // Dark yellow background
    const QColor bgColor = QColor(218, 160, 108);

    void calculateBoardLayout();
    [[nodiscard]] int criticalPointRadius() const;

    static void drawStone(QPainter &painter, QPointF center, double radius, bool isBlack);
    void drawBorders(QPainter &painter) const;
    void drawGridLines(QPainter &painter) const;
    void drawCriticalPoints(QPainter &painter) const;
    void drawStones(QPainter &painter) const;
    void drawWinnerOverlay(QPainter &painter, const QString &winnerText) const;
};

#endif //GOMOKU_BOARDWIDGET_H
