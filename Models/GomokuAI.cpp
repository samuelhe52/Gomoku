//
// Created by Samuel He on 2025/10/9.
//

#include "GomokuAI.h"
#include "BoardManager.h"

// Define static member variable
int GomokuAI::color;

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

std::pair<int, int> GomokuAI::nInRowCount(
    const BoardManager& boardManager,
    const int player,
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
            const int currentCell = boardManager.getCell(row, col);
            if (currentCell != player) continue;

            for (const auto& dir : directions) {
                int count = 1;
                for (int step = 1; step < n; step++) {
                    int newRow = row + dir[0] * step;
                    int newCol = col + dir[1] * step;

                    if (newRow < 0 || newRow >= BOARD_SIZE ||
                        newCol < 0 || newCol >= BOARD_SIZE ||
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
    std::vector<BoardPosition> moves;

    for (const auto& pos : boardManager.getCandidateMoves()) {
        if (boardManager.wouldWin(pos, color) ||
            boardManager.wouldWin(pos, getOpponent(color))) {
            // Prioritize immediate winning/blocking moves
            prioritizedMoves.push_back(pos);
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
    
    return moves;
}

int GomokuAI::evaluate(const BoardManager &boardManager, int player) {
    const int opponent = getOpponent(player);

    // Weights for 0 to 5 in a row
    const int weights[6] = {0, 1, 3, 100, 1000, 10000};

    int playerScore = 0;

    // Evaluate all positions around the last move to see if they would result in a win
    int playerOpenFours = openNInRowCount(boardManager, player, 4);
    int opponentOpenFours = openNInRowCount(boardManager, opponent, 4);
    playerScore += weights[4] * (playerOpenFours * 55);
    playerScore -= weights[4] * (opponentOpenFours * 45);

    // Handle 3 and 2 in a row
    for (int n = 3; n >= 2; --n) {
        auto [playerOpen, playerClosed] = nInRowCount(boardManager, player, n);
        auto [opponentOpen, opponentClosed] = nInRowCount(boardManager, opponent, n);
        playerScore += weights[n] * (playerOpen * 10);
        playerScore -= weights[n] * (opponentOpen * 10);
        playerScore += weights[n] * (playerClosed * 3);
        playerScore -= weights[n] * (opponentClosed * 3);
        if (n == 3 && opponentOpen + playerOpen > 0) {
            break;
        }
    }

    // If there is no > 2 in a row, prioritize center control
    // if (playerScore == 0) {
    //     const int center = BOARD_SIZE / 2;
    //     for (int i = 0; i < BOARD_SIZE; ++i) {
    //         for (int j = 0; j < BOARD_SIZE; ++j) {
    //             if (boardManager.getCell(i, j) == player) {
    //                 playerScore += (BOARD_SIZE - (abs(center - i) + abs(center - j)));
    //             } else if (boardManager.getCell(i, j) == opponent) {
    //                 playerScore -= (BOARD_SIZE - (abs(center - i) + abs(center - j)));
    //             }
    //         }
    //     }
    // }

    return playerScore;
}

std::pair<int, BoardPosition> GomokuAI::minimaxAlphaBeta(
    BoardManager& boardManager,
    int depth,
    bool isMaximizing,
    int currentPlayer,
    int alpha,
    int beta) {
        int winner = boardManager.checkWinner();
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