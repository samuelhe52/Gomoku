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

void BoardManager::makeMove(const BoardPosition position) {
    if (position.row < 0 || position.row >= size || position.col < 0 || position.col >= size) return;
    if (board[position.row][position.col] != EMPTY) return; // Cell already occupied

    board[position.row][position.col] = _blackTurn ? BLACK : WHITE;
    _blackTurn = !_blackTurn; // Switch turn
}

int BoardManager::checkWinner() {
    // TODO: Implement win checking logic
    return EMPTY;
}
