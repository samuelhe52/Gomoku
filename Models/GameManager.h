//
// Created by Samuel He on 2025/10/9.
//

#ifndef GOMOKU_GAMEMANAGER_H
#define GOMOKU_GAMEMANAGER_H

#include "BoardManager.h"
#include "GomokuAI.h"

struct MoveResult {
    bool moveApplied = false;
    char winner = EMPTY;
    bool boardIsFull = false;
    BoardPosition position{-1, -1};
};

class GameManager {
public:
    GameManager();

    void startNewGame(char humanColor);
    void resetGame();

    MoveResult playHumanMove(BoardPosition position);
    MoveResult playAIMove();

    [[nodiscard]] bool isHumansTurn() const { return _currentTurn == _humanColor; }
    [[nodiscard]] bool isAITurn() const { return _currentTurn == _aiColor; }
    [[nodiscard]] char currentTurn() const { return _currentTurn; }
    [[nodiscard]] char humanColor() const { return _humanColor; }
    [[nodiscard]] char aiColor() const { return _aiColor; }
    [[nodiscard]] char winner() const { return _winner; }

    [[nodiscard]] bool canPlayAt(BoardPosition position) const;
    [[nodiscard]] char getCell(const int row, const int col) const { return boardManager.getCell(row, col); }
    [[nodiscard]] char getCell(const BoardPosition position) const { return boardManager.getCell(position); }
    [[nodiscard]] bool isBoardFull() const { return boardManager.isBoardFull(); }
    [[nodiscard]] bool isBoardEmpty() const { return boardManager.isBoardEmpty(); }

private:
    MoveResult applyMove(BoardPosition position);

    BoardManager boardManager;
    char _humanColor = BLACK;
    char _aiColor = WHITE;
    char _currentTurn = BLACK;
    char _winner = EMPTY;
};

#endif //GOMOKU_GAMEMANAGER_H