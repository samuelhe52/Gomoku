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
#include <algorithm>

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

    // Returns EMPTY if no winner after the move, BLACK if black wins, WHITE if white wins
    int makeMove(BoardPosition position);
    void undoMove(BoardPosition position);

    // Returns EMPTY if no winner, BLACK if black wins, WHITE if white wins
    [[nodiscard]] int checkWinner() const;

    [[nodiscard]] int getCell(const int row, const int col) const { return board[row][col]; }
    [[nodiscard]] int getCell(const BoardPosition position) const { return board[position.row][position.col]; }
    [[nodiscard]] bool isValidMove(BoardPosition position) const;

    [[nodiscard]] bool isBoardFull() const;
    [[nodiscard]] bool isBoardEmpty() const;
    
    // Check if placing a piece at position would win for the given player
    [[nodiscard]] bool wouldWin(BoardPosition position, int player) const;

    static const int size;

    // Critical points on a 15x15 board
    static const std::vector<BoardPosition> criticalPoints;

private:
    std::vector<std::vector<int>> board;
    bool _blackTurn = true;
    // Keep track of moves for undo and win checking
    std::vector<BoardPosition> movesHistory;

    // Performs the combined action of making a move, adding to history, and switching turn
    void _makeMove(BoardPosition position);

    // Performs the combined action of undoing a move, removing from history, and switching turn back
    void _undoMove(BoardPosition position);
};

#endif //GOMOKU_BOARDMANAGER_H