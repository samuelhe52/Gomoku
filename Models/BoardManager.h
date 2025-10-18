//
// Created by Samuel He on 2025/10/1.
//

#ifndef GOMOKU_BOARDMANAGER_H
#define GOMOKU_BOARDMANAGER_H

#define EMPTY 0
#define BLACK 1
#define WHITE 2

#include <vector>
#include <iostream>

struct BoardPosition {
    int row;
    int col;

    bool operator==(const BoardPosition& other) const {
        return row == other.row && col == other.col;
    }

    bool operator!=(const BoardPosition& other) const {
        return !(*this == other);
    }

    friend std::ostream& operator<<(std::ostream& os, const BoardPosition& pos) {
        os << "(" << pos.row << ", " << pos.col << ")";
        return os;
    }
};

class BoardManager {
public:
    BoardManager();

    void resetGame();
    int makeMove(BoardPosition position);
    void undoMove(BoardPosition position);

    // Returns EMPTY if no winner, BLACK if black wins, WHITE if white wins
    [[nodiscard]] int checkWinner() const;

    [[nodiscard]] bool blackTurn() const { return _blackTurn; }
    [[nodiscard]] int getCell(const int row, const int col) const { return board[row][col]; }
    [[nodiscard]] int getCell(const BoardPosition position) const { return board[position.row][position.col]; }
    [[nodiscard]] bool isValidMove(BoardPosition position) const;

    [[nodiscard]] bool isBoardFull() const;
    [[nodiscard]] bool isBoardEmpty() const;
    [[nodiscard]] std::vector<BoardPosition> getLastTwoMoves() const {
        return lastTwoMoves;
    }

    static const int size;

    // Critical points on a 15x15 board
    static const std::vector<BoardPosition> criticalPoints;

private:
    std::vector<std::vector<int>> board;
    bool _blackTurn = true;
    // Keep track of last two moves for undo and win checking
    std::vector<BoardPosition> lastTwoMoves;
};

#endif //GOMOKU_BOARDMANAGER_H