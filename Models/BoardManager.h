//
// Created by Samuel He on 2025/10/1.
//

#ifndef GOMOKU_BOARDMANAGER_H
#define GOMOKU_BOARDMANAGER_H

#define EMPTY 0
#define BLACK 1
#define WHITE 2

#include <vector>

struct BoardPosition {
    int row;
    int col;
};

class BoardManager {
public:
    BoardManager();

    void resetGame();
    int makeMove(BoardPosition position);
    // Returns EMPTY if no winner, BLACK if black wins, WHITE if white wins
    [[nodiscard]] int checkWinner() const;

    [[nodiscard]] bool blackTurn() const { return _blackTurn; }
    [[nodiscard]] int getCell(const int row, const int col) const { return board[row][col]; }
    [[nodiscard]] int getCell(const BoardPosition position) const { return board[position.row][position.col]; }
    const int size = 15; // 15x15 board

    // Critical points on a 15x15 board
    const std::vector<BoardPosition> criticalPoints = {
        {3, 3}, {3, 11}, {7, 7}, {11, 3}, {11, 11}
    };

private:
    std::vector<std::vector<int>> board;
    bool _blackTurn = true;
};


#endif //GOMOKU_BOARDMANAGER_H