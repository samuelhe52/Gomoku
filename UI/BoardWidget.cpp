//
// Created by Samuel He on 2025/9/29.
//

#include "BoardWidget.h"

BoardWidget::BoardWidget(QWidget *parent) : QWidget(parent) {}

void BoardWidget::paintEvent(QPaintEvent *) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(rect(), bgColor);

    // Initialize board layout sizes
    calculateBoardLayout();

    drawBorders(painter);
    drawGridLines(painter);
    drawCriticalPoints(painter);
    drawStones(painter);

    if (winner != EMPTY) {
        // Semi-transparent overlay
        QColor overlayColor(0, 0, 0, 100);
        painter.fillRect(rect(), overlayColor);

        QString winnerText = (winner == BLACK) ? "Black Wins!" : "White Wins!";
        QFont font = painter.font();
        font.setPointSize(32);
        font.setBold(true);
        painter.setFont(font);
        painter.setPen(Qt::white);
        
        // Draw text at 20% down from top
        QRect textRect = rect();
        textRect.setTop(static_cast<int>(rect().height() * 0.2));
        painter.drawText(textRect, Qt::AlignHCenter | Qt::AlignTop, winnerText);
    }
}

void BoardWidget::mousePressEvent(QMouseEvent *event) {
    if (winner != EMPTY) return; // Ignore clicks if game is over
    if (event->button() != Qt::LeftButton) {
        // Propagate the event if it's not a left click
        QWidget::mousePressEvent(event);
        return;
    }

    calculateBoardLayout();
    const int x = static_cast<int>(event->position().x());
    const int y = static_cast<int>(event->position().y());

    const int col = static_cast<int>((x - startX + cellSize / 2) / cellSize);
    const int row = static_cast<int>((y - startY + cellSize / 2) / cellSize);
    const int targetX = startX + col * cellSize;
    const int targetY = startY + row * cellSize;
    if (abs(x - targetX) > cellSize / 2.5 || abs(y - targetY) > cellSize / 2.5) {
        event->ignore(); 
        return;
    }

    winner = board.makeMove({row, col});
    update(); // Trigger repaint
    event->accept(); // Stop propagation of mouse click event
}

/* Drawing Actions */

void BoardWidget::calculateBoardLayout() {
    cellSize = boardCellSize();
    borderSize = cellSize * (board.size - 1);
    startX = (width() - borderSize) / 2;
    startY = (height() - borderSize) / 2;
}

void BoardWidget::drawBorders(QPainter &painter) const {
    QColor lineColor = Qt::black;
    QPen borderPen(lineColor, borderWidth, Qt::SolidLine);
    painter.setPen(borderPen);
    
    const QRect borderRect(startX, startY, borderSize, borderSize);
    painter.drawRect(borderRect);
}

void BoardWidget::drawGridLines(QPainter &painter) const {
    QColor lineColor = Qt::black;
    QPen linePen(lineColor, linesWidth, Qt::SolidLine);
    painter.setPen(linePen);

    for (int i = 0; i < board.size; ++i) {
        const int x = startX + i * cellSize;
        // Vertical line
        painter.drawLine(x, startY, x, startY + borderSize);
        // Horizontal line
        for (int j = 0; j < board.size; ++j) {
            const int y = startY + j * cellSize;
            painter.drawLine(startX, y, startX + borderSize, y);
        }
    }
}

void BoardWidget::drawCriticalPoints(QPainter &painter) const {
    QColor lineColor = Qt::black;
    for (const auto &point : board.criticalPoints) {
        const int centerX = startX + point.col * cellSize;
        const int centerY = startY + point.row * cellSize;
        const int radius = criticalPointRadius();
        painter.setBrush(lineColor);
        painter.drawEllipse(QPoint(centerX, centerY), radius, radius);
    }
}

void BoardWidget::drawStones(QPainter &painter) const {
    for (int row = 0; row < board.size; ++row) {
        for (int col = 0; col < board.size; ++col) {
            if (board.getCell(row, col) == EMPTY) continue;
            const int centerX = startX + col * cellSize;
            const int centerY = startY + row * cellSize;
            const double radius = static_cast<double>(cellSize) / 3 + 1.5;
            drawStone(painter, QPointF(centerX, centerY), radius, board.getCell(row, col) == BLACK);
        }
    }
}

void BoardWidget::drawStone(QPainter &painter, QPointF center, double radius, bool isBlack) {
    // Shadow
    QPointF shadowOffset(3, 3);
    double shadowScale = 0.95;
    QColor shadowColor(0, 0, 0, 75);

    painter.setBrush(shadowColor);
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(center + shadowOffset, radius * shadowScale, radius * shadowScale);

    // Stone
    QRadialGradient gradient(center, radius);
    gradient.setFocalPoint(center.x() - radius / 3.0, center.y() - radius / 3.0);
    // 3D effect using gradient
    if (isBlack) {
        gradient.setColorAt(0.0, QColor(80, 80, 80));
        gradient.setColorAt(0.3, QColor(40, 40, 40));
        gradient.setColorAt(1.0, QColor(0, 0, 0));
    } else {
        gradient.setColorAt(0.0, QColor(255, 255, 255));
        gradient.setColorAt(0.7, QColor(200, 200, 200));
        gradient.setColorAt(1.0, QColor(150, 150, 150));
    }

    painter.setBrush(QBrush(gradient));
    painter.drawEllipse(center, radius, radius);
}

int BoardWidget::criticalPointRadius() const {
    return std::max(1, static_cast<int>(sqrt(boardCellSize()) / 1.5));
}

int BoardWidget::boardCellSize() const {
    const int verticalCellSize = width() / board.size;
    const int horizontalCellSize = height() / board.size;
    return std::min(verticalCellSize, horizontalCellSize);
}
