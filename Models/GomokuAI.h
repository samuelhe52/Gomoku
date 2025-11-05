//
// Created by Samuel He on 2025/10/9.
//

#ifndef GOMOKU_GOMOKUAI_H
#define GOMOKU_GOMOKUAI_H

#include "BoardManager.h"
#include "Constants.h"
#include <vector>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <limits>

class GomokuAI {
public:
    static BoardPosition getBestMove(BoardManager& boardManager);
    static BoardPosition randomMove(const BoardManager& boardManager);

    static void setColor(char c) { color = c; } // BLACK(1) or WHITE(2)
    [[nodiscard]] static char getColor() { return color; }

private:
    static char color; // BLACK(1) or WHITE(2)
    [[nodiscard]] static char getOpponent(char player) { return (player == BLACK) ? WHITE : BLACK; }

    [[nodiscard]] static bool wouldWin(const BoardManager& boardManager,
                                       BoardPosition position,
                                       char player);

    [[nodiscard]] static bool posesThreat(const BoardManager& boardManager,
                                          BoardPosition position,
                                          char player);

    // Get possible candidate moves within a certain radius of existing pieces
    [[nodiscard]] static std::vector<BoardPosition> candidateMoves(const BoardManager& boardManager);

    // Heuristic evaluation of the board for a given player. Returns a score relative to the player's perspective.
    [[nodiscard]] static int evaluate(const BoardManager& boardManager, char player);

    struct SequenceSummary {
        int score = 0;
        int openThrees = 0;
        int semiOpenThrees = 0;
        int openFours = 0;
        int semiOpenFours = 0;
    };

    [[nodiscard]] static bool isInsideBoard(int row, int col);
    [[nodiscard]] static int sequenceScore(int length, int openSides);
    [[nodiscard]] static std::pair<SequenceSummary, SequenceSummary> 
        evaluateSequences(const BoardManager& boardManager);
    [[nodiscard]] static int centerControlBias(const BoardManager& boardManager, char player);

    // minimax with alpha-beta pruning. Returns a pair of (score, best move)
    [[nodiscard]] static std::pair<int, BoardPosition> minimaxAlphaBeta(BoardManager& boardManager, int depth, bool isMaximizing, char currentPlayer, int alpha, int beta);
};


#endif //GOMOKU_GOMOKUAI_H