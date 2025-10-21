//
// Created by Samuel He on 2025/10/1.
//

#ifndef GOMOKU_BOARDMANAGER_H
#define GOMOKU_BOARDMANAGER_H

#include "Constants.h"
#include <vector>
#include <iostream>
#include <algorithm>
#include <set>

struct BoardPosition {
    int row;
    int col;

    bool operator==(const BoardPosition& other) const {
        return row == other.row && col == other.col;
    }

    bool operator!=(const BoardPosition& other) const {
        return !(*this == other);
    }

    bool operator<(const BoardPosition& other) const {
        if (row != other.row) return row < other.row;
        return col < other.col;
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
    char makeMove(BoardPosition position);
    void undoMove();

    // Returns EMPTY if no winner, BLACK if black wins, WHITE if white wins
    [[nodiscard]] char checkWinner() const;

    [[nodiscard]] char getCell(const int row, const int col) const { return board[row][col]; }
    [[nodiscard]] char getCell(const BoardPosition position) const { return board[position.row][position.col]; }
    [[nodiscard]] bool isValidMove(BoardPosition position) const;

    [[nodiscard]] bool isBoardFull() const;
    [[nodiscard]] bool isBoardEmpty() const;
    
    // Check if placing a piece at position would win for the given player
    [[nodiscard]] bool wouldWin(BoardPosition position, char player) const;

    [[nodiscard]] std::set<BoardPosition> getCandidateMoves() const {
        return candidateMovesCache;
    }

    static const int size;

    // Critical points on a 15x15 board
    static const std::vector<BoardPosition> criticalPoints;

private:
    char board[BOARD_SIZE][BOARD_SIZE] = {{EMPTY}};
    bool _blackTurn = true;

    // Performs the combined action of making a move, adding to history, and switching turn
    void _makeMove(BoardPosition position);

    // Keep track of moves for undo and win checking with cache information
    struct CandidatesDelta {
        std::set<BoardPosition> addedCandidates;
        bool removedFromCache;
    };

    std::set<BoardPosition> candidateMovesCache;
    CandidatesDelta updateCandidatesCache(BoardPosition pos, char player);
    std::vector<BoardPosition> candidatesAround(BoardPosition position, int radius) const;
    void reverseCandidatesCache(const CandidatesDelta& delta, BoardPosition moveUndone);
    const int candidateRadius = MAX_CANDIDATE_RADIUS;

    struct MoveRecord {
        BoardPosition position;
        CandidatesDelta candidatesDelta;
    };

    std::vector<MoveRecord> movesHistory;
};

#endif //GOMOKU_BOARDMANAGER_H