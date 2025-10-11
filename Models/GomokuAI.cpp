//
// Created by Samuel He on 2025/10/9.
//

#include "GomokuAI.h"
#include "BoardManager.h"

// Define static member variable
int GomokuAI::color;

BoardPosition GomokuAI::getBestMove(const BoardManager &boardManager) {
    return foolishMove(boardManager);
}

BoardPosition GomokuAI::foolishMove(const BoardManager &boardManager) {
    // If three in a row, block it
    if (const auto blockPos = blockOpponentThreeInRow(boardManager); !blockPos.empty()) {
        return blockPos.front();
    }

    // Start testing available cells from the center
    const int center = BoardManager::size / 2;
    for (int layer = 0; layer <= center; ++layer) {
        for (int i = center - layer; i <= center + layer; ++i) {
            for (int j = center - layer; j <= center + layer; ++j) {
                if (boardManager.isValidMove({i, j})) {
                    return {i, j};
                }
            }
        }
    }
    return randomMove(boardManager);
}

BoardPosition GomokuAI::randomMove(const BoardManager &boardManager) {
    srand(time(nullptr));
    // Placeholder: choose random available cell
    int randomRow, randomCol;
    do {
        randomRow = rand() % BoardManager::size;
        randomCol = rand() % BoardManager::size;
    } while (!boardManager.isValidMove({randomRow, randomCol}));
    return {randomRow, randomCol};
}

std::vector<GomokuAI::BlockingPositions> GomokuAI::blockingPositions(
    const BoardManager& boardManager,
    const int n
) {
    const int directions[4][2] = {
        {0, 1},  // Horizontal
        {1, 0},  // Vertical
        {1, 1},  // Diagonal
        {1, -1}  // Diagonal
    };
    std::vector<BlockingPositions> results;

    for (int row = 0; row < BoardManager::size; row++) {
        for (int col = 0; col < BoardManager::size; col++) {
            const int currentCell = boardManager.getCell(row, col);
            if (currentCell == EMPTY) continue;
            
            for (const auto& dir : directions) {
                int count = 1;
                for (int step = 1; step < n; step++) {
                    int newRow = row + dir[0] * step;
                    int newCol = col + dir[1] * step;

                    if (newRow < 0 || newRow >= BoardManager::size ||
                        newCol < 0 || newCol >= BoardManager::size ||
                        boardManager.getCell(newRow, newCol) != currentCell) {
                        break;
                    }
                    count++; // Found another with the same color in the direction
                }
                
                if (count >= n) {
                    std::vector<BoardPosition> blockingPositions;
                    // Check the cell before the sequence
                    if (boardManager.isValidMove({row - dir[0], col - dir[1]})) {
                        blockingPositions.push_back({row - dir[0], col - dir[1]});
                    }
                    // Check the cell after the sequence
                    if (boardManager.isValidMove({row + dir[0] * n, col + dir[1] * n})) {
                        blockingPositions.push_back({row + dir[0] * n, col + dir[1] * n});
                    }
                    results.push_back({blockingPositions, currentCell});
                }
            }
        }
    }
    return results;
}

