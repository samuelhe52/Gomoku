//
// Created by Samuel He on 2025/10/1.
//

#include "BoardManager.h"

BoardManager::BoardManager() : board(size, std::vector<int>(size, EMPTY)) {}

const std::vector<BoardPosition> BoardManager::criticalPoints = {
    {3, 3}, {3, 11}, {7, 7}, {11, 3}, {11, 11}
};

const int BoardManager::size = 15;

void BoardManager::resetGame() {
    for (auto &row: board) {
        std::fill(row.begin(), row.end(), EMPTY);
    }
    _blackTurn = true;
}

// Returns EMPTY if no winner after the move, BLACK if black wins, WHITE if white wins
int BoardManager::makeMove(const BoardPosition position) {
    if (!isValidMove(position)) { 
        std::cerr << "Invalid move attempted: " << position << std::endl;
        return EMPTY; // Invalid move
    }

    board[position.row][position.col] = _blackTurn ? BLACK : WHITE;
    _blackTurn = !_blackTurn; // Switch turn
    movesHistory.push_back(position);
    return checkWinner();
}

void BoardManager::undoMove(const BoardPosition position) {
    if (position.row < 0 || position.row >= size || position.col < 0 || position.col >= size) return;
    if (board[position.row][position.col] == EMPTY) return; // Cell already empty
    if (movesHistory.empty() || position != movesHistory.back()) {
        std::cerr << "Illegal undoMove" << std::endl;
        std::cerr << movesHistory.back() << std::endl;
        std::cerr << position << std::endl;
        return; // Not the last move
    }
    
    board[position.row][position.col] = EMPTY;
    movesHistory.pop_back(); // Remove the undone move from history
    _blackTurn = !_blackTurn; // Switch turn back
}

int BoardManager::checkWinner() const {
    const int directions[4][2] = {
        {0, 1},  // Horizontal
        {1, 0},  // Vertical
        {1, 1},  // Diagonal
        {1, -1}  // Diagonal
    };

    auto isInvalid = [&](int r, int c, int player) {
        return r < 0 || r >= size || c < 0 || c >= size || board[r][c] != player;
    };

    std::vector<BoardPosition> lastTwoMoves;
    if (!movesHistory.empty()) {
        lastTwoMoves.push_back(movesHistory.back());
        if (movesHistory.size() >= 2) {
            lastTwoMoves.push_back(movesHistory[movesHistory.size() - 2]);
        }
    }

    for (auto move : lastTwoMoves) {
        for (const auto& dir : directions) {
            auto [row, col] = move;
            const int currentCell = board[row][col];
            if (currentCell == EMPTY) continue;
            int count = 1;

            // Check in the positive direction
            for (int step = 1; step < 5; step++) {
                int newRow = row + dir[0] * step;
                int newCol = col + dir[1] * step;
                if (isInvalid(newRow, newCol, currentCell)) break;
                count++;
            }

            // Check in the negative direction
            for (int step = 1; step < 5; step++) {
                int newRow = row - dir[0] * step;
                int newCol = col - dir[1] * step;
                if (isInvalid(newRow, newCol, currentCell)) break;
                count++;
            }

            if (count >= 5) {
                return currentCell;
            }
        }
    }

    return EMPTY;
}

bool BoardManager::isValidMove(const BoardPosition position) const {
    return position.row >= 0 && position.row < size &&
           position.col >= 0 && position.col < size &&
           board[position.row][position.col] == EMPTY;
}

bool BoardManager::isBoardFull() const {
    for (const auto &row : board) {
        for (const auto &cell : row) {
            if (cell == EMPTY) return false;
        }
    }
    return true;
}

bool BoardManager::isBoardEmpty() const {
    for (const auto &row : board) {
        for (const auto &cell : row) {
            if (cell != EMPTY) return false;
        }
    }
    return true;
}

bool BoardManager::wouldWin(BoardPosition position, int player) const {
    if (!isValidMove(position)) return false;
    
    const int directions[4][2] = {
        {0, 1},  // Horizontal
        {1, 0},  // Vertical
        {1, 1},  // Diagonal
        {1, -1}  // Diagonal
    };

    for (const auto& dir : directions) {
        int count = 1; // Count the position itself

        // Check in the positive direction
        for (int step = 1; step < 5; step++) {
            int newRow = position.row + dir[0] * step;
            int newCol = position.col + dir[1] * step;
            if (newRow < 0 || newRow >= size || newCol < 0 || newCol >= size || 
                board[newRow][newCol] != player) {
                break;
            }
            count++;
        }

        // Check in the negative direction
        for (int step = 1; step < 5; step++) {
            int newRow = position.row - dir[0] * step;
            int newCol = position.col - dir[1] * step;
            if (newRow < 0 || newRow >= size || newCol < 0 || newCol >= size || 
                board[newRow][newCol] != player) {
                break;
            }
            count++;
        }

        if (count >= 5) {
            return true;
        }
    }

    return false;
}