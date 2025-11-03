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
    // Reset candidate map
    for (auto &row : candidateMap) {
        std::fill(std::begin(row), std::end(row), false);
    }
    // Reserve capacity to avoid rehashing during game
    // Average game has ~50-100 moves, with radius 2, expect ~20-40 candidates
    candidateMovesCache.reserve(64);
}

void BoardManager::_makeMove(BoardPosition position) {
    board[position.row][position.col] = _blackTurn ? BLACK : WHITE;
    _blackTurn = !_blackTurn; // Switch turn
    
    MoveRecord record;
    record.position = position;
    record.candidatesDelta = updateCandidatesCache(position, board[position.row][position.col]);
    movesHistory.push_back(record);
}

char BoardManager::makeMove(const BoardPosition position) {
    if (!isValidMove(position)) { 
        std::cerr << "Invalid move attempted: " << position << std::endl;
        return EMPTY; // Invalid move
    }

    _makeMove(position);
    return checkWinner();
}

void BoardManager::undoMove() {
    if (movesHistory.empty()) return;
    
    MoveRecord lastRecord = movesHistory.back();
    BoardPosition position = lastRecord.position;
    
    // Reset the board position
    board[position.row][position.col] = EMPTY;

    reverseCandidatesCache(lastRecord.candidatesDelta, position);
    
    movesHistory.pop_back(); // Remove the undone move from history
    _blackTurn = !_blackTurn; // Switch turn back
}

void BoardManager::reverseCandidatesCache(const CandidatesDelta& delta, BoardPosition moveUndone) {
     // Reverse the cache changes
    // Remove all candidates that were added for this move
    for (const auto& candidate : delta.addedCandidates) {
        candidateMovesCache.erase(candidate);
        candidateMap[candidate.row][candidate.col] = false;  // Update map
    }
    
    // If this position was in the cache before the move, add it back
    if (delta.removedFromCache) {
        candidateMovesCache.insert(moveUndone);
        candidateMap[moveUndone.row][moveUndone.col] = true;  // Update map
    }
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

BoardManager::CandidatesDelta BoardManager::updateCandidatesCache(
    const BoardPosition pos,
    const char player
) {
    CandidatesDelta lastRecord;
    
    // Check using the fast boolean array instead of hash lookup
    lastRecord.removedFromCache = candidateMap[pos.row][pos.col];
    
    // Remove from both structures
    candidateMovesCache.erase(pos);
    candidateMap[pos.row][pos.col] = false;

    // Pre-calculate bounds to avoid repeated boundary checks
    const int minRow = std::max(0, pos.row - candidateRadius);
    const int maxRow = std::min(BOARD_SIZE - 1, pos.row + candidateRadius);
    const int minCol = std::max(0, pos.col - candidateRadius);
    const int maxCol = std::min(BOARD_SIZE - 1, pos.col + candidateRadius);

    // Pre-allocate space for added candidates to avoid reallocation
    lastRecord.addedCandidates.reserve((2 * candidateRadius + 1) * (2 * candidateRadius + 1));

    // OPTIMIZATION: First collect all new candidates, then batch insert
    // This reduces hash table operations significantly
    for (int newRow = minRow; newRow <= maxRow; ++newRow) {
        for (int newCol = minCol; newCol <= maxCol; ++newCol) {
            // Skip the center position and occupied cells
            if ((newRow == pos.row && newCol == pos.col) || board[newRow][newCol] != EMPTY) {
                continue;
            }
            
            if (!candidateMap[newRow][newCol]) {
                BoardPosition newPos{newRow, newCol};
                lastRecord.addedCandidates.push_back(newPos);
                candidateMap[newRow][newCol] = true;
            }
        }
    }
    
    // Batch insert all new candidates at once to minimize hash table overhead
    candidateMovesCache.insert(lastRecord.addedCandidates.begin(), 
                               lastRecord.addedCandidates.end());
    
    return lastRecord;
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
