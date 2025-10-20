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
    char makeMove(BoardPosition position);
    // Returns the winner after the move, EMPTY if no winner, BLACK if black wins, WHITE if white wins
    char makeAIMove();
    // Set the AI's color, BLACK(1) or WHITE(2)
    void setAIColor(char c) { GomokuAI::setColor(c); }
    // Returns EMPTY if no winner, BLACK if black wins, WHITE if white wins
    [[nodiscard]] char checkWinner() const { return boardManager.checkWinner(); }
    [[nodiscard]] char getCell(const int row, const int col) const { return boardManager.getCell(row, col); }
    [[nodiscard]] char getCell(const BoardPosition position) const { return boardManager.getCell(position); }
    [[nodiscard]] bool isBoardFull() const { return boardManager.isBoardFull(); }
    [[nodiscard]] bool isBoardEmpty() const { return boardManager.isBoardEmpty(); }

private:
    BoardManager boardManager;
};

#endif //GOMOKU_GAMEMANAGER_H