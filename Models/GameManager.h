//
// Created by Samuel He on 2025/10/9.
//

#ifndef GOMOKU_GAMEMANAGER_H
#define GOMOKU_GAMEMANAGER_H

#include "BoardManager.h"
#include "GomokuAI.h"

class GameManager {
public:
    GameManager();
    void resetGame() { boardManager.resetGame(); }
    // Returns the winner after the move, EMPTY if no winner, BLACK if black wins, WHITE if white wins
    int makeMove(BoardPosition position);
    // Returns the winner after the move, EMPTY if no winner, BLACK if black wins, WHITE if white wins
    int makeAIMove();
    // Returns EMPTY if no winner, BLACK if black wins, WHITE if white wins
    [[nodiscard]] int checkWinner() const { return boardManager.checkWinner(); }
    [[nodiscard]] int getCell(const int row, const int col) const { return boardManager.getCell(row, col); }
    [[nodiscard]] int getCell(const BoardPosition position) const { return boardManager.getCell(position); }
    [[nodiscard]] bool blackTurn() const { return boardManager.blackTurn(); }
    [[nodiscard]] bool isBoardFull() const { return boardManager.isBoardFull(); }
    [[nodiscard]] bool isBoardEmpty() const { return boardManager.isBoardEmpty(); }

private:
    BoardManager boardManager;
};

#endif //GOMOKU_GAMEMANAGER_H