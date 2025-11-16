//
// Created by Samuel He on 2025/10/9.
//

#include "GomokuAI.h"
#include "BoardManager.h"

GomokuAI::GomokuAI(const char color, const int maxDepth)
    : _color(color), _maxDepth(maxDepth) {
        threadPool.setMaxThreadCount(threadCount);
    };

BoardPosition GomokuAI::getBestMove(const BoardManager &boardManager) const {
    if (QThread::currentThread()->isInterruptionRequested()) {
        return {-1, -1};
    }
    if (boardManager.isBoardEmpty()) {
        return {BOARD_SIZE / 2, BOARD_SIZE / 2};
    }

    BoardManager simulatedBoard = boardManager;
    return minimaxAlphaBetaRootParallel(simulatedBoard, _maxDepth);
}

bool GomokuAI::wouldWin(const BoardManager& boardManager,
                        const BoardPosition position,
                        const char player) const {
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
                           const char player) const {
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

std::vector<BoardPosition> GomokuAI::candidateMoves(const BoardManager& boardManager) const {
    std::vector<BoardPosition> threatMoves;
    std::vector<BoardPosition> moves;

    for (const auto& pos : boardManager.getCandidateMoves()) {
        if (wouldWin(boardManager, pos, _color) ||
            wouldWin(boardManager, pos, getOpponent(_color))) {
            // Prioritize immediate winning/blocking moves
            return {pos};
        } else if (posesThreat(boardManager, pos, _color) ||
                   posesThreat(boardManager, pos, getOpponent(_color))) {
            threatMoves.push_back(pos);
        } else {
            moves.push_back(pos);
        }
    }

    // Prefer center control
    std::sort(moves.begin(), moves.end(), [&](const BoardPosition& a, const BoardPosition& b) {
        return boardManager.centerManhattanDistance[a.row][a.col] <
               boardManager.centerManhattanDistance[b.row][b.col];
    });

    threatMoves.insert(threatMoves.end(), moves.begin(), moves.end());
    return threatMoves;
}

GomokuAI::SequenceSummary GomokuAI::evaluateForPlayerAtPos(
    const BoardManager& boardManager,
    const char player,
    const int row,
    const int col
) const {
    SequenceSummary summary;

    if (boardManager.getCell(row, col) != player) {
        return summary;
    }

    const int directions[4][2] = {
        {0, 1},
        {1, 0},
        {1, 1},
        {1, -1}
    };

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

    return summary;
}

std::pair<GomokuAI::SequenceSummary, GomokuAI::SequenceSummary> 
GomokuAI::evaluateSequences(const BoardManager& boardManager) const {
    SequenceSummary playerSummary;
    SequenceSummary opponentSummary;

    for (int row = 0; row < BOARD_SIZE; ++row) {
        for (int col = 0; col < BOARD_SIZE; ++col) {
            playerSummary += evaluateForPlayerAtPos(boardManager, _color, row, col);
            opponentSummary += evaluateForPlayerAtPos(boardManager, getOpponent(_color), row, col);
        }
    }

    return {playerSummary, opponentSummary};
}

int GomokuAI::centerControlBias(const BoardManager& boardManager, const char player) const {
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

int GomokuAI::evaluate(const BoardManager &boardManager, const char player) const {
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
    int alpha,
    int beta
) const {
    if (QThread::currentThread()->isInterruptionRequested()) {
        return {0, {-1, -1}};
    }
    char winner = boardManager.checkWinner();
    if (depth == 0 || winner != EMPTY) {
        if (winner == _color) {
            // Prefer immediate wins
            return {std::numeric_limits<int>::max() / 2 + 10000, {}};
        } else if (winner == getOpponent(_color)) {
            return {std::numeric_limits<int>::min() / 2 - 10000, {}};
        }
        // Always evaluate from the AI's perspective
        return {evaluate(boardManager, _color), {}};
    }

    BoardPosition bestMove;
    auto moves = candidateMoves(boardManager);
    
    if (isMaximizing) {
        int maxEval = std::numeric_limits<int>::min();

        for (const auto& pos : moves) {
            boardManager.makeMove(pos);
            auto [eval, _] = minimaxAlphaBeta(boardManager, depth - 1, false, alpha, beta);
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
            auto [eval, _] = minimaxAlphaBeta(boardManager, depth - 1, true, alpha, beta);
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

BoardPosition GomokuAI::minimaxAlphaBetaRootParallel(
    BoardManager& boardManager,
    int depth
) const {
    // We parallelize only the root level of the minimax tree.
    // Divide the possible moves into chunks, where each chunk is processed in parallel.
    // After processing a chunk, update a global alpha based on the results from that chunk.
    // This to some extent preserves pruning: pruning is still valid across chunks;
    // But within each chunk, no pruning occurs.

    if (QThread::currentThread()->isInterruptionRequested()) {
        return {-1, -1};
    }

    BoardPosition bestMove;
    auto moves = candidateMoves(boardManager);
    // Each chunk should have threadCount moves to maximize parallelism
    auto chunks = splitIntoChunks(moves, threadCount);

    int globalAlpha = std::numeric_limits<int>::min();

    for (const auto& chunk : chunks) {
        auto results = QtConcurrent::blockingMapped(
            &threadPool,
            chunk,
            [this, &boardManager, depth, globalAlpha](const BoardPosition& pos) {
                BoardManager simulatedBoard = boardManager;
                simulatedBoard.makeMove(pos);
                auto [eval, _] = minimaxAlphaBeta(
                    simulatedBoard,
                    depth - 1,
                    false,
                    globalAlpha,
                    std::numeric_limits<int>::max()
                );
                return std::make_pair(eval, pos);
            }
        );

        // Find the best move in this chunk
        int chunkMaxEval = std::numeric_limits<int>::min();
        BoardPosition chunkBestMove;
        for (const auto& [eval, pos] : results) {
            if (eval > chunkMaxEval) {
                chunkMaxEval = eval;
                chunkBestMove = pos;
            }
        }

        // Update global alpha for next chunk
        if (chunkMaxEval > globalAlpha) {
            globalAlpha = chunkMaxEval;
            bestMove = chunkBestMove;
        }
    }

    return bestMove;
}
