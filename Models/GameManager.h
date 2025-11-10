//
// Created by Samuel He on 2025/10/9.
//

#pragma once

#include "BoardManager.h"
#include "GomokuAI.h"
#include <QObject>
#include <QTimer>
#include <QMetaType>

// Summary of a move application, used by the UI to refresh state without
// re-querying the entire manager.
struct MoveResult {
    bool moveApplied = false;
    char winner = EMPTY;
    bool boardIsFull = false;
    BoardPosition position{-1, -1};
    char placedColor = EMPTY;
};

Q_DECLARE_METATYPE(MoveResult)

class GameManager : public QObject {
    Q_OBJECT
    
public:
    explicit GameManager(QObject *parent = nullptr);
    ~GameManager() {
        delete _aiEngine;
    }

    // Turn helpers for UI flow control.
    [[nodiscard]] bool isHumansTurn() const { return _currentTurn == _humanColor; }
    [[nodiscard]] bool isAITurn() const { return _currentTurn == _aiColor; }
    [[nodiscard]] char currentTurn() const { return _currentTurn; }
    [[nodiscard]] char humanColor() const { return _humanColor; }
    [[nodiscard]] char aiColor() const { return _aiColor; }
    [[nodiscard]] char winner() const { return _winner; }

    // Check if a move is legal given current state (no winner and empty cell).
    [[nodiscard]] bool canPlayAt(BoardPosition position) const;
    [[nodiscard]] char getCell(const int row, const int col) const { return boardManager.getCell(row, col); }
    [[nodiscard]] char getCell(const BoardPosition position) const { return boardManager.getCell(position); }
    [[nodiscard]] bool isBoardFull() const { return boardManager.isBoardFull(); }
    [[nodiscard]] bool isBoardEmpty() const { return boardManager.isBoardEmpty(); }

public slots:
    // Configure the player colors and initialize a fresh game state.
    void startNewGame(char humanColor);

    // Attempt to place a human move; automatically triggers AI move if game continues.
    void handleHumanMove(BoardPosition position);
    
    // Trigger AI to make the first move (call after startNewGame if AI goes first)
    void makeAIFirstMove();

signals:
    void moveApplied(MoveResult result);

private:
    MoveResult playHumanMove(BoardPosition position);
    MoveResult playAIMove();
    MoveResult applyMove(BoardPosition position);

    BoardManager boardManager;
    char _humanColor = BLACK;
    char _aiColor = WHITE;
    char _currentTurn = BLACK;
    char _winner = EMPTY;
    GomokuAI* _aiEngine = nullptr;
    
    // Clear board and manager state to the beginning of a new game
    void initializeNewGameState();
};
