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
    board->updateGameState(gameManager.winner(), gameManager.isBoardFull());
    
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
    // Connect board cell selection to human move handler
    connect(board, &BoardWidget::cellSelected, this, [this](int row, int col) {
        BoardPosition position{row, col};
        handleHumanMove(position);
    });
    
    // Connect color chooser to game start
    connect(colorChooser, &ColorChooserWidget::colorChosen, this, [this](bool playerIsBlack) {
        boardStack->setCurrentIndex(0);
        startGame(playerIsBlack);
    });
    
    // Connect reset button
    connect(resetButton, &QPushButton::clicked, this, [this]() {
        gameManager.resetGame();
        board->updateGameState(gameManager.winner(), gameManager.isBoardFull());
        boardStack->setCurrentIndex(1);
    });
}

void GameWidget::handleHumanMove(const BoardPosition position) {
    MoveResult result = gameManager.playHumanMove(position);
    if (!result.moveApplied) {
        return;
    }

    board->updateGameState(result.winner, result.boardIsFull);

    // Immediately request AI move if game is still ongoing
    if (result.winner == EMPTY && !result.boardIsFull) {
        requestAIMove();
    }
}

void GameWidget::startGame(const bool playerIsBlack) {
    const char humanColor = playerIsBlack ? BLACK : WHITE;
    gameManager.startNewGame(humanColor);
    board->updateGameState(gameManager.winner(), gameManager.isBoardFull());

    if (gameManager.isAITurn()) {
        requestAIMove();
    }
}

void GameWidget::requestAIMove() {
    if (!gameManager.isAITurn() || gameManager.winner() != EMPTY) return;

    MoveResult result = gameManager.playAIMove();
    if (!result.moveApplied) return;

    board->updateGameState(result.winner, result.boardIsFull);
}
