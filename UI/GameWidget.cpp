//
// Created by Samuel He on 2025/10/2.
//

#include "GameWidget.h"

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

    // Create a stacked widget to overlay the color chooser on the board
    auto *stack = new QStackedWidget(this);
    
    // Create board widget
    board = new BoardWidget(stack);
    
    // Create an overlay widget that contains the color chooser
    auto *overlayWidget = new QWidget(stack);
    overlayWidget->setAttribute(Qt::WA_TransparentForMouseEvents, false);
    overlayWidget->setStyleSheet("background: transparent;");
    
    // Create color chooser overlay centered in the overlay widget
    colorChooser = new ColorChooserWidget(overlayWidget);
    overlayLayout = new QVBoxLayout(overlayWidget);
    overlayLayout->setContentsMargins(0, 0, 0, 0);
    overlayLayout->addWidget(colorChooser);
    
    // Add both to the stack
    stack->addWidget(board);
    stack->addWidget(overlayWidget);
    stack->setCurrentIndex(1); // Show overlay initially
    
    // Connect color chooser signal
    connect(colorChooser, &ColorChooserWidget::colorChosen, this, [this, stack](bool playerIsBlack) {
        stack->setCurrentIndex(0); // Switch to board
        if (!playerIsBlack) {
            // AI goes first
            board->makeAIMove();
        }
        board->setAIColor(playerIsBlack ? WHITE : BLACK);
    });
    
    mainLayout->addWidget(stack, 1); // take all available space
    
    buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    
    resetButton = new QPushButton("Reset Game", this);
    resetButton->setFixedSize(160, 45);
    resetButton->setCursor(Qt::PointingHandCursor);
    resetButton->setStyleSheet(button_style_sheet);

    buttonLayout->addWidget(resetButton);
    buttonLayout->addStretch();
    
    connect(resetButton, &QPushButton::clicked, this, [this, stack]() {
        board->resetGame();
        stack->setCurrentIndex(1); // Show overlay again
    });
    mainLayout->addLayout(buttonLayout);
}
