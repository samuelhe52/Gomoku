//
// Created by Samuel He on 2025/10/2.
//

#include "GameWidget.h"

GameWidget::GameWidget(QWidget *parent) : QWidget(parent) {
    initializeComponents();
    setupBoard();
    setupOverlay();
    setupLayouts();
    setupConnections();
}

void GameWidget::initializeComponents() {
    // Background color
    QPalette palette = this->palette();
    palette.setColor(QPalette::Window, QColor(218, 160, 108));
    this->setPalette(palette);
    this->setAutoFillBackground(true);
    
    // Main layout
    mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 15);
    mainLayout->setSpacing(5);

    // Stacked widget for board and overlay
    boardStack = new QStackedWidget(this);
    
    // Reset button
    resetButton = new QPushButton("Reset Game", this);
    resetButton->setFixedSize(160, 45);
    resetButton->setCursor(Qt::PointingHandCursor);
    resetButton->setStyleSheet(button_style_sheet);
}

void GameWidget::setupBoard() {
    board = new BoardWidget(boardStack);
    board->setGameManager(&gameManager);
    
    // Add board to stack
    boardStack->addWidget(board);
}

void GameWidget::setupOverlay() {
    auto *overlayWidget = new QWidget(boardStack);
    overlayWidget->setAttribute(Qt::WA_TransparentForMouseEvents, false);
    overlayWidget->setStyleSheet("background: transparent;");
    
    colorChooser = new ColorChooserWidget(overlayWidget);
    overlayLayout = new QVBoxLayout(overlayWidget);
    overlayLayout->setContentsMargins(0, 0, 0, 0);
    overlayLayout->addWidget(colorChooser);
    
    boardStack->addWidget(overlayWidget);
    boardStack->setCurrentIndex(1); // Show color chooser initially
}

void GameWidget::setupLayouts() {
    mainLayout->addWidget(boardStack, 1);

    // Configure button layout
    buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    buttonLayout->addWidget(resetButton);
    buttonLayout->addStretch();
    
    mainLayout->addLayout(buttonLayout);
}

void GameWidget::setupConnections() {
    connectGameManagerSignals();
    connectUISignals();
    connectResetButton();
}

// Connect signals from GameWidget to GameManager slots
void GameWidget::connectGameManagerSignals() {
    connect(this, &GameWidget::handleHumanMove, &gameManager, &GameManager::handleHumanMove);
    connect(this, &GameWidget::startGame, this, [this](bool playerIsBlack) {
        const char humanColor = playerIsBlack ? BLACK : WHITE;
        gameManager.startNewGame(humanColor);
        board->refresh();
        
        // Trigger AI first move if it's AI's turn
        if (gameManager.isAITurn()) {
            gameManager.makeAIFirstMove();
        }
    });

    // Refresh the board when a move is applied
    connect(&gameManager, &GameManager::moveApplied, this, [this](MoveResult result) {
        board->refresh();
    });
}

void GameWidget::connectUISignals() {
    connect(board, &BoardWidget::cellSelected, this, [this](int row, int col) {
        BoardPosition position{row, col};
        emit handleHumanMove(position);
    });
    
    // Trigger game start
    connect(colorChooser, &ColorChooserWidget::colorChosen, this, [this](bool playerIsBlack) {
        boardStack->setCurrentIndex(0);
        emit startGame(playerIsBlack);
    });
}

void GameWidget::connectResetButton() {
    connect(resetButton, &QPushButton::clicked, this, [this]() {
        gameManager.resetGame();
        board->refresh();
        boardStack->setCurrentIndex(1);
    });
}
