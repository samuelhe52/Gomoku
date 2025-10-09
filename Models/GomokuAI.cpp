//
// Created by Samuel He on 2025/10/9.
//

#include "GomokuAI.h"

BoardPosition GomokuAI::getBestMove(const BoardManager &boardManager) {
    std::srand(std::time(NULL));
    // Placeholder: choose random available cell
    int randomRow, randomCol;
    do {
        randomRow = std::rand() % BoardManager::size;
        randomCol = std::rand() % BoardManager::size;
    } while (!boardManager.isValidMove({randomRow, randomCol}));
    return {randomRow, randomCol};
}
