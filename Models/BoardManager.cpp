//
// Created by Samuel He on 2025/10/1.
//

#include "BoardManager.h"

BoardManager::BoardManager() = default;

const std::vector<BoardPosition> BoardManager::criticalPoints = {
    {3, 3}, {3, 11}, {7, 7}, {11, 3}, {11, 11}
};

void BoardManager::resetGame() {
    for (auto &row : board) {
        std::fill(std::begin(row), std::end(row), EMPTY);
    }
    _blackTurn = true;
    movesHistory.clear();
    candidateMovesCache.clear();
}

void BoardManager::_makeMove(BoardPosition position) {
    board[position.row][position.col] = _blackTurn ? BLACK : WHITE;
    _blackTurn = !_blackTurn; // Switch turn
    
    MoveRecord record;
    record.position = position;
    movesHistory.push_back(record);
}

char BoardManager::makeMove(const BoardPosition position) {
    if (!isValidMove(position)) { 
        std::cerr << "Invalid move attempted: " << position << std::endl;
        return EMPTY; // Invalid move
    }

    _makeMove(position);
    updateCandidatesCache();
    return checkWinner();
}

void BoardManager::undoMove() {
    if (movesHistory.empty()) return;
    
    MoveRecord lastRecord = movesHistory.back();
    BoardPosition position = lastRecord.position;
    
    // Reset the board position
    board[position.row][position.col] = EMPTY;
    
    // Reverse the cache changes
    // Remove all candidates that were added for this move
    for (const auto& candidate : lastRecord.addedCandidates) {
        candidateMovesCache.erase(candidate);
    }
    
    // If this position was in the cache before the move, add it back
    if (lastRecord.removedFromCache) {
        candidateMovesCache.insert(position);
    }
    
    movesHistory.pop_back(); // Remove the undone move from history
    _blackTurn = !_blackTurn; // Switch turn back
}

char BoardManager::checkWinner() const {
    if (movesHistory.empty()) return EMPTY;

    const int directions[4][2] = {
        {0, 1},  // Horizontal
        {1, 0},  // Vertical
        {1, 1},  // Diagonal
        {1, -1}  // Diagonal
    };

    auto isInvalid = [&](int r, int c, char player) {
        return r < 0 || r >= BOARD_SIZE || c < 0 || c >= BOARD_SIZE || board[r][c] != player;
    };

    const char player = _blackTurn ? WHITE : BLACK; // Last move was by the opposite player
    BoardPosition lastMove = movesHistory.back().position;

    for (const auto& dir : directions) {
        auto [row, col] = lastMove;
    const char currentCell = board[row][col];
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

    return EMPTY;
}

bool BoardManager::isValidMove(const BoardPosition position) const {
    return position.row >= 0 && position.row < BOARD_SIZE &&
           position.col >= 0 && position.col < BOARD_SIZE &&
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

bool BoardManager::wouldWin(BoardPosition position, char player) const {
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
            if (newRow < 0 || newRow >= BOARD_SIZE || newCol < 0 || newCol >= BOARD_SIZE || 
                board[newRow][newCol] != player) {
                break;
            }
            count++;
        }

        // Check in the negative direction
        for (int step = 1; step < 5; step++) {
            int newRow = position.row - dir[0] * step;
            int newCol = position.col - dir[1] * step;
            if (newRow < 0 || newRow >= BOARD_SIZE || newCol < 0 || newCol >= BOARD_SIZE || 
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

void BoardManager::updateCandidatesCache() {
    if (movesHistory.empty()) return;
    
    // Get reference to the last move record to update it
    MoveRecord& lastRecord = movesHistory.back();
    BoardPosition lastMove = lastRecord.position;
    
    // Check if this position was already in the cache
    lastRecord.removedFromCache = candidateMovesCache.count(lastMove) > 0;
    
    // Remove the move position from cache (it's now occupied)
    candidateMovesCache.erase(lastMove);
    
    // Add new candidates around the move and track them
    for (const auto& pos : candidatesAround(lastMove, candidateRadius)) {
        // Only track truly new candidates (not already in cache)
        if (candidateMovesCache.count(pos) == 0) {
            lastRecord.addedCandidates.insert(pos);
            candidateMovesCache.insert(pos);
        }
    }
}

std::vector<BoardPosition> BoardManager::candidatesAround(
    const BoardPosition position,
    const int radius) const {
    std::vector<BoardPosition> candidates;
    for (int dr = -radius; dr <= radius; ++dr) {
        for (int dc = -radius; dc <= radius; ++dc) {
            if (dr == 0 && dc == 0) continue; // Skip the center position
            int newRow = position.row + dr;
            int newCol = position.col + dc;
            BoardPosition newPos{newRow, newCol};
            if (isValidMove(newPos)) {
                candidates.push_back(newPos);
            }
        }
    }
    return candidates;
}