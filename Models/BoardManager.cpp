//
// Created by Samuel He on 2025/10/1.
//

#include "BoardManager.h"

BoardManager::BoardManager() : board(size, std::vector<int>(size, EMPTY)) {}

void BoardManager::resetGame() {
    for (auto &row: board) {
        std::fill(row.begin(), row.end(), EMPTY);
    }
    _blackTurn = true;
}

// Returns EMPTY if no winner after the move, BLACK if black wins, WHITE if white wins
int BoardManager::makeMove(const BoardPosition position) {
    if (position.row < 0 || position.row >= size || position.col < 0 || position.col >= size) return EMPTY;
    if (board[position.row][position.col] != EMPTY) return EMPTY; // Cell already occupied

    board[position.row][position.col] = _blackTurn ? BLACK : WHITE;
    _blackTurn = !_blackTurn; // Switch turn
    return checkWinner();
}

int BoardManager::checkWinner() const {
    const int directions[4][2] = {
        {0, 1},  // Horizontal
        {1, 0},  // Vertical
        {1, 1},  // Diagonal
        {1, -1}  // Diagonal
    };

    for (int row = 0; row < size; row++) {
        for (int col = 0; col < size; col++) {
            const int currentCell = board[row][col];
            if (currentCell == EMPTY) continue;
            
            for (const auto& dir : directions) {
                int count = 1;
                for (int step = 1; step < 5; step++) {
                    int newRow = row + dir[0] * step;
                    int newCol = col + dir[1] * step;
                    
                    if (newRow < 0 || newRow >= size || 
                        newCol < 0 || newCol >= size ||
                        board[newRow][newCol] != currentCell) {
                        break;
                    }
                    count++;
                }
                
                if (count >= 5) {
                    return currentCell;
                }
            }
        }
    }
    return EMPTY;
}
