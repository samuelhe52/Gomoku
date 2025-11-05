//
// Created by Samuel He on 2025/10/2.
//

#include "GameWidget.h"
#include <QMetaType>

GameWidget::GameWidget(QWidget *parent) : QWidget(parent) {
    setupGameManager();
    initializeComponents();
    setupBoard();
    setupOverlay();
    setupLayouts();
    setupUISignals();
}
GameWidget::~GameWidget() {
    // Clean up the game thread
    if (gameThread) {
        gameThread->quit();
        gameThread->wait();
    }
}

void GameWidget::setupGameManager() {
    // Move GameManager to a separate thread
    gameManager = new GameManager();
    gameThread = new QThread(this);
    gameManager->moveToThread(gameThread);
    // Ensure MoveResult can be delivered across threads
    qRegisterMetaType<MoveResult>("MoveResult");
    connectGameManagerSignals();
    connect(gameThread, &QThread::finished, gameManager, &QObject::deleteLater);
    connect(gameThread, &QThread::finished, gameThread, &QObject::deleteLater);
    gameThread->start();
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
    
    // Add board to stack
    boardStack->addWidget(board);

    // Connect after board exists
    connect(gameManager, &GameManager::moveApplied, board, &BoardWidget::onMoveApplied);
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

void GameWidget::setupUISignals() {
    connectUISignals();
    connectResetButton();
}

// Connect signals from GameWidget to GameManager slots
void GameWidget::connectGameManagerSignals() {
    connect(this, &GameWidget::handleHumanMove, gameManager, &GameManager::handleHumanMove);
    connect(this, &GameWidget::startGame, this, [this](bool playerIsBlack) {
        const char humanColor = playerIsBlack ? BLACK : WHITE;
        board->resetSnapshot();
        QMetaObject::invokeMethod(gameManager, [gm = gameManager, humanColor]() {
            gm->startNewGame(humanColor);
        }, Qt::QueuedConnection);
    });
}

void GameWidget::connectUISignals() {
    connect(board, &BoardWidget::cellSelected, this, [this](int row, int col) {
        BoardPosition position{row, col};
        emit handleHumanMove(position);
    });
    
    // Trigger game start after color is chosen
    connect(colorChooser, &ColorChooserWidget::colorChosen, this, [this](bool playerIsBlack) {
        boardStack->setCurrentIndex(0);
        board->setHumanColor(playerIsBlack ? BLACK : WHITE);
        emit startGame(playerIsBlack);
    });
}

void GameWidget::connectResetButton() {
    connect(resetButton, &QPushButton::clicked, this, [this]() {
        QMetaObject::invokeMethod(gameManager, &GameManager::resetGame, Qt::QueuedConnection);
        board->resetSnapshot();
        boardStack->setCurrentIndex(1);
    });
}
