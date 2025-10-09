//
// Created by Samuel He on 2025/10/9.
//

#include "GomokuAI.h"

BoardPosition GomokuAI::getBestMove(const BoardManager &boardManager) {
    srand(time(nullptr));
    // Placeholder: choose random available cell
    int randomRow, randomCol;
    do {
        randomRow = rand() % BoardManager::size;
        randomCol = rand() % BoardManager::size;
    } while (!boardManager.isValidMove({randomRow, randomCol}));
    return {randomRow, randomCol};
}
