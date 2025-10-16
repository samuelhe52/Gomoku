//
// Created by Samuel He on 2025/10/9.
//

#include "GomokuAI.h"
#include "BoardManager.h"

// Define static member variable
int GomokuAI::color;

BoardPosition GomokuAI::getBestMove(BoardManager &boardManager) {
    if (boardManager.isBoardEmpty()) {
        return {BoardManager::size / 2, BoardManager::size / 2}; // Start in the center
    }
    return minimax(boardManager, 3, getColor()).second;
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

int GomokuAI::nInRowCount(
    const BoardManager& boardManager,
    const int player,
    const int n,
    const bool onlyOpen
) {
    const int directions[4][2] = {
        {0, 1},  // Horizontal
        {1, 0},  // Vertical
        {1, 1},  // Diagonal
        {1, -1}  // Diagonal
    };
    int openCount = 0;

    for (int row = 0; row < BoardManager::size; row++) {
        for (int col = 0; col < BoardManager::size; col++) {
            const int currentCell = boardManager.getCell(row, col);
            if (currentCell != player) continue;

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
                    // Check the cell before the sequence
                    bool beforeIsValid = boardManager.isValidMove({row - dir[0], col - dir[1]});
                    // Check the cell after the sequence
                    bool afterIsValid = boardManager.isValidMove({row + dir[0] * n, col + dir[1] * n});
                    if (onlyOpen) {
                        if (beforeIsValid && afterIsValid) {
                            openCount++;
                        }
                    } else {
                        if (beforeIsValid || afterIsValid) {
                            openCount++;
                        }
                    }
                }
            }
        }
    }
    return openCount;
}

std::vector<BoardPosition> GomokuAI::possibleBestMoves(
    const BoardManager& boardManager,
    const int radius) {
    std::vector<BoardPosition> moves;

    for (int i = 0; i < BoardManager::size; ++i) {
        for (int j = 0; j < BoardManager::size; ++j) {
            if (boardManager.getCell(i, j) == EMPTY) {
                bool hasPieceNearby = false;
                for (int dx = -radius; dx <= radius; ++dx) {
                    for (int dy = -radius; dy <= radius; ++dy) {
                        if (dx == 0 && dy == 0) continue;

                        int ni = i + dx;
                        int nj = j + dy;

                        if (ni >= 0 &&
                            ni < BoardManager::size && 
                            nj >= 0 && 
                            nj < BoardManager::size) {
                            if (boardManager.getCell(ni, nj) != EMPTY) {
                                hasPieceNearby = true;
                                break;
                            }
                        }
                    }
                    if (hasPieceNearby) {
                        break;
                    }
                }

                if (hasPieceNearby) {
                    moves.push_back({i, j});
                }
            }
        }
    }
    
    return moves;
}

int GomokuAI::evaluate(const BoardManager &boardManager, int player) {
    const int opponent = player == BLACK ? WHITE : BLACK;

    // Weights for 0 to 5 in a row
    const int weights[6] = {0, 1, 10, 100, 1000, 10000000};

    int playerScore = 0;

    std::vector visited(BoardManager::size, std::vector<bool>(BoardManager::size, false));

    int playerFour = fourInRowCount(boardManager, player);
    int opponentFour = fourInRowCount(boardManager, opponent);
    if (playerFour > 0) {
        return weights[5] * 100; // Immediate win
    }
    if (opponentFour > 0) {
        return -weights[5] * 10; // Immediate threat
    }

    for (int n = 5; n >= 2; --n) {
        int playerOpen = openNInRowCount(boardManager, player, n);
        int opponentOpen = openNInRowCount(boardManager, opponent, n);
        playerScore += weights[n] * (playerOpen * 10);
        playerScore -= weights[n] * (opponentOpen * 10);
        if (n == 3 && opponentOpen + playerOpen > 0) {
            break;
        }
    }

    // If there is no > 2 in a row, prioritize center control
    if (playerScore == 0) {
        const int center = BoardManager::size / 2;
        for (int i = 0; i < BoardManager::size; ++i) {
            for (int j = 0; j < BoardManager::size; ++j) {
                if (boardManager.getCell(i, j) == player) {
                    playerScore += (BoardManager::size - (abs(center - i) + abs(center - j)));
                } else if (boardManager.getCell(i, j) == opponent) {
                    playerScore -= (BoardManager::size - (abs(center - i) + abs(center - j)));
                }
            }
        }
    }

    return playerScore;
}

std::pair<int, BoardPosition> GomokuAI::minimax(
    BoardManager& boardManager,
    int depth,
    int maximizingPlayer) {
        int winner = boardManager.checkWinner();
        if (depth == 0) {
            if (winner == maximizingPlayer) {
                return {10000000, {}};
            } else if (winner == (maximizingPlayer == BLACK ? WHITE : BLACK)) {
                return {-10000000, {}};
            }
            return {evaluate(boardManager, maximizingPlayer), {}};
        }

        BoardPosition best_move;
        int best_score = std::numeric_limits<int>::min();
        int opponent = (maximizingPlayer == BLACK) ? WHITE : BLACK;

        for (auto pos : possibleBestMoves(boardManager, 1)) {
            boardManager.makeMove(pos);
            auto [score, _] = minimax(boardManager, depth - 1, opponent);
            score *= -1; // Invert score for opponent to get the correct score for maximizingPlayer
            boardManager.undoMove(pos);
            if (score > best_score) {
                best_score = score;
                best_move = pos;
            }
        }

        return {best_score, best_move};
}