//
// Created by Samuel He on 2025/9/29.
//

#include "BoardWidget.h"
#include <QPainter>
#include <QMouseEvent>

BoardWidget::BoardWidget(QWidget *parent)
    : QWidget(parent), board(boardSize, std::vector<int>(boardSize, 0)) {
        // Test stone
        board[3][5] = BLACK;
        board[3][4] = BLACK;
        board[4][4] = WHITE;
        board[5][4] = WHITE;
        makeMove({7, 7});
        makeMove({7, 8});
        makeMove({8, 7});
        makeMove({8, 8});
    }

void BoardWidget::paintEvent(QPaintEvent *) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Dark yellow background
    QColor bgColor = QColor(218, 160, 108).darker(100);
    painter.fillRect(rect(), bgColor);

    // Initialize board layout sizes
    calculateBoardLayout();

    drawBorders(painter);
    drawGridLines(painter);
    drawCriticalPoints(painter);
    drawStones(painter);
}

void BoardWidget::mousePressEvent(QMouseEvent *event) {
    if (event->button() != Qt::LeftButton) {
        QWidget::mousePressEvent(event); // Propagate other button events
        return;
    }

    calculateBoardLayout();
    const int x = static_cast<int>(event->position().x());
    const int y = static_cast<int>(event->position().y());

    const int col = static_cast<int>((x - startX + cellSize / 2) / cellSize);
    const int row = static_cast<int>((y - startY + cellSize / 2) / cellSize);
    const int targetX = startX + col * cellSize;
    const int targetY = startY + row * cellSize;
    if (abs(x - targetX) > cellSize / 3 || abs(y - targetY) > cellSize / 3) {
        event->ignore(); 
        return;
    }

    makeMove({row, col});
    event->accept(); // Stop propagation of mouse click event
}

/* Drawing Actions */

void BoardWidget::calculateBoardLayout() {
    cellSize = boardCellSize();
    borderSize = cellSize * (boardSize - 1);
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
}

void BoardWidget::drawCriticalPoints(QPainter &painter) const {
    QColor lineColor = Qt::black;
    for (const auto &point : criticalPoints) {
        const int centerX = startX + point.col * cellSize;
        const int centerY = startY + point.row * cellSize;
        const int radius = criticalPointRadius();
        painter.setBrush(lineColor);
        painter.drawEllipse(QPoint(centerX, centerY), radius, radius);
    }
}

void BoardWidget::drawStones(QPainter &painter) const {
    for (int row = 0; row < boardSize; ++row) {
        for (int col = 0; col < boardSize; ++col) {
            if (board[row][col] == EMPTY) continue;
            const int centerX = startX + col * cellSize;
            const int centerY = startY + row * cellSize;
            const double radius = static_cast<double>(cellSize) / 3 + 1.5;
            drawStone(painter, QPointF(centerX, centerY), radius, board[row][col] == BLACK);
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
    const int verticalCellSize = width() / boardSize;
    const int horizontalCellSize = height() / boardSize;
    return std::min(verticalCellSize, horizontalCellSize);
}

/* Game Logic */

void BoardWidget::makeMove(BoardPosition position) {
    if (position.row < 0 || position.row >= boardSize || position.col < 0 || position.col >= boardSize) return;
    if (board[position.row][position.col] != EMPTY) return; // Cell already occupied

    board[position.row][position.col] = blackTurn ? BLACK : WHITE;
    blackTurn = !blackTurn; // Switch turn
    checkWinner();
    update();
}

void BoardWidget::checkWinner() {
    // TODO: Implement win checking logic
    _winner = EMPTY; // Placeholder
}

void BoardWidget::resetGame() {
    for (auto &row : board) {
        std::fill(row.begin(), row.end(), EMPTY);
    }
    blackTurn = true;
    _winner = EMPTY;
    update();
}
