//
// Created by Samuel He on 2025/10/9.
//

#include "GomokuAI.h"
#include "BoardManager.h"

// Define static member variable
char GomokuAI::color;

BoardPosition GomokuAI::getBestMove(BoardManager &boardManager) {
    if (boardManager.isBoardEmpty()) {
        return {BOARD_SIZE / 2, BOARD_SIZE / 2}; // Start in the center
    }

    return minimaxAlphaBeta(boardManager, MAX_DEPTH, true, getColor(),
                                   std::numeric_limits<int>::min(),
                                   std::numeric_limits<int>::max()).second;
}

BoardPosition GomokuAI::randomMove(const BoardManager &boardManager) {
    srand(time(nullptr));
    // Placeholder: choose random available cell
    int randomRow, randomCol;
    do {
        randomRow = rand() % BOARD_SIZE;
        randomCol = rand() % BOARD_SIZE;
    } while (!boardManager.isValidMove({randomRow, randomCol}));
    return {randomRow, randomCol};
}

bool GomokuAI::wouldWin(const BoardManager& boardManager,
                        const BoardPosition position,
                        const char player) {
    if (!boardManager.isValidMove(position)) return false;

    const int directions[4][2] = {
        {0, 1},  // Horizontal
        {1, 0},  // Vertical
        {1, 1},  // Diagonal
        {1, -1}  // Diagonal
    };

    for (const auto& dir : directions) {
        int count = 1; // Include the hypothetical move itself

        for (int step = 1; step < 5; ++step) {
            const int newRow = position.row + dir[0] * step;
            const int newCol = position.col + dir[1] * step;
            if (newRow < 0 || newRow >= BOARD_SIZE ||
                newCol < 0 || newCol >= BOARD_SIZE ||
                boardManager.getCell(newRow, newCol) != player) {
                break;
            }
            count++;
        }

        for (int step = 1; step < 5; ++step) {
            const int newRow = position.row - dir[0] * step;
            const int newCol = position.col - dir[1] * step;
            if (newRow < 0 || newRow >= BOARD_SIZE ||
                newCol < 0 || newCol >= BOARD_SIZE ||
                boardManager.getCell(newRow, newCol) != player) {
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

bool GomokuAI::posesThreat(const BoardManager& boardManager,
                           const BoardPosition position,
                           const char player) {
    if (!boardManager.isValidMove(position)) return false;

    const int directions[4][2] = {
        {0, 1},
        {1, 0},
        {1, 1},
        {1, -1}
    };

    for (const auto& dir : directions) {
        int count = 1;

        int forwardRow = position.row + dir[0];
        int forwardCol = position.col + dir[1];
        while (forwardRow >= 0 && forwardRow < BOARD_SIZE &&
               forwardCol >= 0 && forwardCol < BOARD_SIZE &&
               boardManager.getCell(forwardRow, forwardCol) == player) {
            count++;
            forwardRow += dir[0];
            forwardCol += dir[1];
        }

        int backwardRow = position.row - dir[0];
        int backwardCol = position.col - dir[1];
        while (backwardRow >= 0 && backwardRow < BOARD_SIZE &&
               backwardCol >= 0 && backwardCol < BOARD_SIZE &&
               boardManager.getCell(backwardRow, backwardCol) == player) {
            count++;
            backwardRow -= dir[0];
            backwardCol -= dir[1];
        }

        if (count > 4) {
            continue; // This move is a direct win, handled by wouldWin
        }

        const bool forwardOpen = forwardRow >= 0 && forwardRow < BOARD_SIZE &&
                                 forwardCol >= 0 && forwardCol < BOARD_SIZE &&
                                 boardManager.getCell(forwardRow, forwardCol) == EMPTY;

        const bool backwardOpen = backwardRow >= 0 && backwardRow < BOARD_SIZE &&
                                  backwardCol >= 0 && backwardCol < BOARD_SIZE &&
                                  boardManager.getCell(backwardRow, backwardCol) == EMPTY;

        if (count == 4 || count == 3) {
            if (forwardOpen || backwardOpen) {
                return true;
            }
        }
    }

    return false;
}

std::pair<int, int> GomokuAI::nInRowCount(
    const BoardManager& boardManager,
    const char player,
    const int n) {
    const int directions[4][2] = {
        {0, 1},  // Horizontal
        {1, 0},  // Vertical
        {1, 1},  // Diagonal
        {1, -1}  // Diagonal
    };
    int openCount = 0;
    int closedCount = 0;

    for (int row = 0; row < BOARD_SIZE; row++) {
        for (int col = 0; col < BOARD_SIZE; col++) {
            const char currentCell = boardManager.getCell(row, col);
            if (currentCell != player) continue;

            for (const auto& dir : directions) {
                int count = 1;
                int encounteredEmpty = 0;
                for (int step = 1; step < n; step++) {
                    int newRow = row + dir[0] * step;
                    int newCol = col + dir[1] * step;

                    if (newRow < 0 || newRow >= BOARD_SIZE ||
                        newCol < 0 || newCol >= BOARD_SIZE ||
                        boardManager.getCell(newRow, newCol) == getOpponent(player)) {
                        break;
                    }
                    if (boardManager.getCell(newRow, newCol) == EMPTY) {
                        encounteredEmpty++;
                        if (encounteredEmpty > 1) break;
                    }
                    count++; // Found another with the same color in the direction
                }
                
                if (count >= n) {
                    // Check the cell before the sequence
                    bool beforeIsValid = boardManager.isValidMove({row - dir[0], col - dir[1]});
                    // Check the cell after the sequence
                    bool afterIsValid = boardManager.isValidMove({row + dir[0] * n, col + dir[1] * n});
                    if (beforeIsValid && afterIsValid) {
                        openCount++;
                    } else if (beforeIsValid || afterIsValid) {
                        closedCount++;
                    }
                }
            }
        }
    }
    return {openCount, closedCount};
}

std::vector<BoardPosition> GomokuAI::candidateMoves(const BoardManager& boardManager) {
    std::vector<BoardPosition> prioritizedMoves;
    std::vector<BoardPosition> threatMoves;
    std::vector<BoardPosition> moves;

    for (const auto& pos : boardManager.getCandidateMoves()) {
        if (wouldWin(boardManager, pos, color) ||
            wouldWin(boardManager, pos, getOpponent(color))) {
            // Prioritize immediate winning/blocking moves
            prioritizedMoves.push_back(pos);
        } else if (posesThreat(boardManager, pos, color) ||
                   posesThreat(boardManager, pos, getOpponent(color))) {
            threatMoves.push_back(pos);
        } else {
            moves.push_back(pos);
        }
    }

    if (!prioritizedMoves.empty()) {
        return prioritizedMoves;
    }

    // Order moves by heuristic: prefer center control 
    // TODO: improve heuristic by preferring existing pieces as well
    std::sort(moves.begin(), moves.end(), [&](const BoardPosition& a, const BoardPosition& b) {
        int scoreA = 0;
        int scoreB = 0;

        int center = BOARD_SIZE / 2;
        scoreA -= (abs(center - a.row) + abs(center - a.col));
        scoreB -= (abs(center - b.row) + abs(center - b.col));

        return scoreA > scoreB; 
    });

    threatMoves.insert(threatMoves.end(), moves.begin(), moves.end());
    return threatMoves;
}

bool GomokuAI::isInsideBoard(const int row, const int col) {
    return row >= 0 && row < BOARD_SIZE && col >= 0 && col < BOARD_SIZE;
}

int GomokuAI::sequenceScore(const int length, const int openSides) {
    if (length >= 5) {
        return 1000000;
    }

    switch (length) {
        case 4:
            if (openSides == 2) return 50000;
            if (openSides == 1) return 10000;
            return 500;
        case 3:
            if (openSides == 2) return 2000;
            if (openSides == 1) return 400;
            return 50;
        case 2:
            if (openSides == 2) return 200;
            if (openSides == 1) return 60;
            return 10;
        case 1:
            if (openSides == 2) return 20;
            if (openSides == 1) return 5;
            return 1;
        default:
            return 0;
    }
}

int GomokuAI::evaluatePlayer(const BoardManager& boardManager, const char player) {
    const int directions[4][2] = {
        {0, 1},
        {1, 0},
        {1, 1},
        {1, -1}
    };

    int score = 0;

    for (int row = 0; row < BOARD_SIZE; ++row) {
        for (int col = 0; col < BOARD_SIZE; ++col) {
            if (boardManager.getCell(row, col) != player) {
                continue;
            }

            for (const auto& dir : directions) {
                const int prevRow = row - dir[0];
                const int prevCol = col - dir[1];

                if (isInsideBoard(prevRow, prevCol) &&
                    boardManager.getCell(prevRow, prevCol) == player) {
                    continue; // Already counted as part of a preceding segment
                }

                int length = 1;
                int nextRow = row + dir[0];
                int nextCol = col + dir[1];

                while (isInsideBoard(nextRow, nextCol) &&
                       boardManager.getCell(nextRow, nextCol) == player) {
                    ++length;
                    nextRow += dir[0];
                    nextCol += dir[1];
                }

                const bool openStart = isInsideBoard(prevRow, prevCol) &&
                                        boardManager.getCell(prevRow, prevCol) == EMPTY;
                const bool openEnd = isInsideBoard(nextRow, nextCol) &&
                                      boardManager.getCell(nextRow, nextCol) == EMPTY;
                score += sequenceScore(length, static_cast<int>(openStart) + static_cast<int>(openEnd));
            }
        }
    }

    return score;
}

int GomokuAI::centerControlBias(const BoardManager& boardManager, const char player) {
    const int center = BOARD_SIZE / 2;
    int score = 0;

    for (int row = 0; row < BOARD_SIZE; ++row) {
        for (int col = 0; col < BOARD_SIZE; ++col) {
            if (boardManager.getCell(row, col) != player) {
                continue;
            }

            const int distance = std::abs(center - row) + std::abs(center - col);
            int contribution = std::max(1, BOARD_SIZE - distance);
            score += contribution;
        }
    }

    return score;
}

int GomokuAI::evaluate(const BoardManager &boardManager, char player) {
    const char opponent = getOpponent(player);
    const int playerSequences = evaluatePlayer(boardManager, player);
    const int opponentSequences = evaluatePlayer(boardManager, opponent);

    int score = playerSequences - opponentSequences;

    const int winThreshold = 500000;
    if (score > winThreshold || score < -winThreshold) {
        return score;
        }

    const int centerWeight = 3;
    const int centerScore = centerControlBias(boardManager, player) -
                            centerControlBias(boardManager, opponent);
    score += centerWeight * centerScore;

    return score;
}

std::pair<int, BoardPosition> GomokuAI::minimaxAlphaBeta(
    BoardManager& boardManager,
    int depth,
    bool isMaximizing,
    char currentPlayer,
    int alpha,
    int beta) {
        char winner = boardManager.checkWinner();
        if (depth == 0 || winner != EMPTY) {
            if (winner == color) {
                // Prefer immediate wins
                return {std::numeric_limits<int>::max() / 2 + 10000, {}};
            } else if (winner == getOpponent(color)) {
                return {std::numeric_limits<int>::min() / 2 - 10000, {}};
            }
            // Always evaluate from the AI's perspective
            return {evaluate(boardManager, color), {}};
        }

        BoardPosition bestMove;
        auto moves = candidateMoves(boardManager);
        
        if (isMaximizing) {
            int maxEval = std::numeric_limits<int>::min();

            for (const auto& pos : moves) {
                boardManager.makeMove(pos);
                auto [eval, _] = minimaxAlphaBeta(boardManager, depth - 1, false, getOpponent(currentPlayer), alpha, beta);
                boardManager.undoMove();
                
                if (eval > maxEval) {
                    maxEval = eval;
                    bestMove = pos;
                }
                
                alpha = std::max(alpha, eval);
                // beta comes from the parent and records the smallest value found by the parent so far
                // so if beta <= alpha, no need to explore further, because the parent will not choose this path
                // (parent is minimizing player)
                if (beta <= alpha) {
                    break; // Alpha cutoff
                }
            }
            
            return {maxEval, bestMove};
        } else {
            int minEval = std::numeric_limits<int>::max();

            for (const auto& pos : moves) {
                boardManager.makeMove(pos);
                auto [eval, _] = minimaxAlphaBeta(boardManager, depth - 1, true, getOpponent(currentPlayer), alpha, beta);
                boardManager.undoMove();
                
                if (eval < minEval) {
                    minEval = eval;
                    bestMove = pos;
                }
                
                beta = std::min(beta, eval);
                if (beta <= alpha) {
                    // alpha comes from the parent and records the largest value found by the parent so far
                    // so if beta <= alpha, no need to explore further, because the parent will not choose this path
                    // (parent is maximizing player)
                    break; // Beta cutoff
                }
            }
            
            return {minEval, bestMove};
        }
}