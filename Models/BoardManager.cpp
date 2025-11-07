//
// Created by Samuel He on 2025/10/1.
//

#include "BoardManager.h"

BoardManager::BoardManager() {
    // Reserve capacity to avoid rehashing during game
    candidateMovesCache.reserve(64);
    // Calculate manhattan distances from center
    const int center = BOARD_SIZE / 2;
    for (int row = 0; row < BOARD_SIZE; ++row) {
        for (int col = 0; col < BOARD_SIZE; ++col) {
            centerManhattanDistance[row][col] = std::abs(center - row) + std::abs(center - col);
        }
    }
}

const std::vector<BoardPosition> BoardManager::criticalPoints = {
    {3, 3}, {3, 11}, {7, 7}, {11, 3}, {11, 11}
};

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
    // Remove all candidates that were added for this move
    for (const auto& candidate : delta.addedCandidates) {
        candidateMovesCache.erase(candidate);
        candidateMap[candidate.row][candidate.col] = false;
    }
    
    // If this position was in the cache before the move, add it back
    if (delta.removedFromCache) {
        candidateMovesCache.insert(moveUndone);
        candidateMap[moveUndone.row][moveUndone.col] = true;
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
    
    lastRecord.removedFromCache = candidateMap[pos.row][pos.col];
    
    // Mark position as occupied
    candidateMovesCache.erase(pos);
    candidateMap[pos.row][pos.col] = false;

    // Pre-calculate bounds
    const int minRow = std::max(0, pos.row - MAX_CANDIDATE_RADIUS);
    const int maxRow = std::min(BOARD_SIZE - 1, pos.row + MAX_CANDIDATE_RADIUS);
    const int minCol = std::max(0, pos.col - MAX_CANDIDATE_RADIUS);
    const int maxCol = std::min(BOARD_SIZE - 1, pos.col + MAX_CANDIDATE_RADIUS);

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
    
    // Batch insert all collected candidates (minor optimization ~2%)
    candidateMovesCache.insert(lastRecord.addedCandidates.begin(), 
                               lastRecord.addedCandidates.end());
    
    return lastRecord;
}