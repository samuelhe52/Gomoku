//
// Created by Samuel He on 2025/9/29.
//

#include "BoardWidget.h"
#include <QPainter>
#include <QMouseEvent>

BoardWidget::BoardWidget(QWidget *parent)
    : QWidget(parent), board(boardSize, std::vector<int>(boardSize, 0)) {}

void BoardWidget::paintEvent(QPaintEvent *) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Brown background
    QColor bgColor = QColor(218, 160, 108).darker(150);
    painter.fillRect(rect(), bgColor);

    QColor lineColor = Qt::lightGray;
    lineColor = lineColor.lighter(125);
    
    // Borders

    QPen borderPen(lineColor, borderWidth, Qt::SolidLine);
    painter.setPen(borderPen);

    const int cellSize = boardCellSize();
    const int borderSize = cellSize * (boardSize - 1);
    // Center the board
    const int startX = (width() - borderSize) / 2;
    const int startY = (height() - borderSize) / 2;
    const QRect borderRect(startX, startY, borderSize, borderSize);
    painter.drawRect(borderRect);

    // Grid lines
    QPen linePen(lineColor, linesWidth, Qt::SolidLine);
    painter.setPen(linePen);

    for (int i = 0; i < boardSize; ++i) {
        const int x = startX + i * cellSize;
        // Vertical line
        painter.drawLine(x, startY, x, startY + borderSize);
        // Horizontal line
        for (int j = 0; j < boardSize; ++j) {
            const int y = startY + j * cellSize;
            painter.drawLine(startX, y, startX + borderSize, y);
        }
    }

    // Critical points
    for (const auto &point : criticalPoints) {
        const int centerX = startX + point.col * cellSize;
        const int centerY = startY + point.row * cellSize;
        const int radius = criticalPointRadius();
        painter.setBrush(lineColor);
        painter.drawEllipse(QPoint(centerX, centerY), radius, radius);
    }

}

int BoardWidget::criticalPointRadius() {
    return std::max(1, (int)(sqrt(boardCellSize()) / 1.2));
}

int BoardWidget::boardCellSize() {
    const int verticalCellSize = width() / boardSize;
    const int horizontalCellSize = height() / boardSize;
    return std::min(verticalCellSize, horizontalCellSize);
}

void BoardWidget::mousePressEvent(QMouseEvent *event) {

}