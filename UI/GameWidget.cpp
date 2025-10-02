//
// Created by Samuel He on 2025/10/2.
//

#include "GameWidget.h"
#include "BoardWidget.h"

GameWidget::GameWidget(QWidget *parent) : QWidget(parent) {
    setupUI();
}

void GameWidget::setupUI() {
    QPalette palette = this->palette();
    palette.setColor(QPalette::Window, QColor(218, 160, 108));
    this->setPalette(palette);
    this->setAutoFillBackground(true);
    
    mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 15);
    mainLayout->setSpacing(5);
    
    board = new BoardWidget(this);
    mainLayout->addWidget(board, 1); // take all available space
    
    buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    
    resetButton = new QPushButton("Reset Game", this);
    resetButton->setFixedSize(160, 45);
    resetButton->setCursor(Qt::PointingHandCursor);
    resetButton->setStyleSheet(button_style_sheet);

    buttonLayout->addWidget(resetButton);
    buttonLayout->addStretch();
    
    connect(resetButton, &QPushButton::clicked, board, &BoardWidget::resetGame);
    mainLayout->addLayout(buttonLayout);
}
