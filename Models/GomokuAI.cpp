//
// Created by Samuel He on 2025/10/9.
//

#include "GomokuAI.h"
#include "BoardManager.h"
#include <QThread>

// Define static member variable
char GomokuAI::color;

BoardPosition GomokuAI::getBestMove(const BoardManager &boardManager) {
    if (QThread::currentThread()->isInterruptionRequested()) {
        return {-1, -1};
    }
    if (boardManager.isBoardEmpty()) {
        return {BOARD_SIZE / 2, BOARD_SIZE / 2};
    }

    BoardManager simulatedBoard = boardManager;
    return minimaxAlphaBeta(simulatedBoard, MAX_DEPTH, true, getColor(),
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
               boardManager.getCell(forwardRow, forwardCol) == player &&
               count < 5 // Winning condition is handled by wouldWin()
            ) {
            count++;
            forwardRow += dir[0];
            forwardCol += dir[1];
        }

        int backwardRow = position.row - dir[0];
        int backwardCol = position.col - dir[1];
        while (backwardRow >= 0 && backwardRow < BOARD_SIZE &&
               backwardCol >= 0 && backwardCol < BOARD_SIZE &&
               boardManager.getCell(backwardRow, backwardCol) == player &&
               count < 5
            ) {
            count++;
            backwardRow -= dir[0];
            backwardCol -= dir[1];
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



std::vector<BoardPosition> GomokuAI::candidateMoves(const BoardManager& boardManager) {
    std::vector<BoardPosition> threatMoves;
    std::vector<BoardPosition> moves;

    for (const auto& pos : boardManager.getCandidateMoves()) {
        if (wouldWin(boardManager, pos, color) ||
            wouldWin(boardManager, pos, getOpponent(color))) {
            // Prioritize immediate winning/blocking moves
            return {pos};
        } else if (posesThreat(boardManager, pos, color) ||
                   posesThreat(boardManager, pos, getOpponent(color))) {
            threatMoves.push_back(pos);
        } else {
            moves.push_back(pos);
        }
    }

    // Prefer center control 
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
            return 300;
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

std::pair<GomokuAI::SequenceSummary, GomokuAI::SequenceSummary> 
GomokuAI::evaluateSequences(const BoardManager& boardManager) {
    const int directions[4][2] = {
        {0, 1},
        {1, 0},
        {1, 1},
        {1, -1}
    };

    SequenceSummary playerSummary;
    SequenceSummary opponentSummary;

    auto evaluateForPlayer = [&](
        const char player,
        SequenceSummary& summary,
        const int row,
        const int col
    ) {
        if (boardManager.getCell(row, col) != player) {
            return;
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

            while (
                isInsideBoard(nextRow, nextCol) &&
                boardManager.getCell(nextRow, nextCol) == player
            ) {
                ++length;
                nextRow += dir[0];
                nextCol += dir[1];
            }

            const bool openStart = isInsideBoard(prevRow, prevCol) && 
                                    boardManager.getCell(prevRow, prevCol) == EMPTY;
            const bool openEnd = isInsideBoard(nextRow, nextCol) &&
                                    boardManager.getCell(nextRow, nextCol) == EMPTY;
            const int openSides = static_cast<int>(openStart) + static_cast<int>(openEnd);
            summary.score += sequenceScore(length, openSides);

            if (length >= 5) {
                if (openSides > 0) {
                    summary.openFours += 1;
                }
                continue;
            }

            if (length == 4) {
                if (openSides == 2) {
                    summary.openFours += 1;
                } else if (openSides == 1) {
                    summary.semiOpenFours += 1;
                }
            } else if (length == 3) {
                if (openSides == 2) {
                    summary.openThrees += 1;
                } else if (openSides == 1) {
                    summary.semiOpenThrees += 1;
                }
            }
        }
    };

    for (int row = 0; row < BOARD_SIZE; ++row) {
        for (int col = 0; col < BOARD_SIZE; ++col) {
            evaluateForPlayer(color, playerSummary, row, col);
            evaluateForPlayer(getOpponent(color), opponentSummary, row, col);
        }
    }

    return {playerSummary, opponentSummary};
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
    const auto [playerSummary, opponentSummary] = evaluateSequences(boardManager);

    if (playerSummary.openFours > 0) {
        return 400000 + playerSummary.openFours * 2000;
    }
    if (opponentSummary.openFours > 0) {
        return -400000 - opponentSummary.openFours * 2000;
    }

    int score = playerSummary.score - opponentSummary.score;

    const int openThreeBonus = 15000;
    score += openThreeBonus * (playerSummary.openThrees - opponentSummary.openThrees);

    const int doubleThreeBonus = 60000;
    if (playerSummary.openThrees >= 2) {
        score += doubleThreeBonus;
    }
    if (opponentSummary.openThrees >= 2) {
        score -= doubleThreeBonus;
    }

    const int semiOpenThreeBonus = 4000;
    score += semiOpenThreeBonus * (playerSummary.semiOpenThrees - opponentSummary.semiOpenThrees);

    const int semiOpenFourBonus = 20000;
    score += semiOpenFourBonus * (playerSummary.semiOpenFours - opponentSummary.semiOpenFours);

    const int centerWeight = 2;
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
        if (QThread::currentThread()->isInterruptionRequested()) {
            return {0, {-1, -1}};
        }
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