//
// Created by Samuel He on 2025/10/9.
//

#pragma once

#include "BoardManager.h"
#include "Constants.h"
#include <vector>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <limits>

class GomokuAI {
public:
    explicit GomokuAI(char color, int maxDepth = MAX_DEPTH);

    BoardPosition getBestMove(const BoardManager& boardManager) const;
    BoardPosition randomMove(const BoardManager& boardManager) const;

    void setColor(char c) { _color = c; }
    [[nodiscard]] char getColor() const { return _color; }
    void setMaxDepth(int depth) { _maxDepth = depth; }
    [[nodiscard]] int getMaxDepth() const { return _maxDepth; }

    struct SequenceSummary {
        int score = 0;
        int openThrees = 0;
        int semiOpenThrees = 0;
        int openFours = 0;
        int semiOpenFours = 0;

        SequenceSummary& operator+=(const SequenceSummary& other) {
            score += other.score;
            openThrees += other.openThrees;
            semiOpenThrees += other.semiOpenThrees;
            openFours += other.openFours;
            semiOpenFours += other.semiOpenFours;
            return *this;
        }
    };

    static SequenceSummary combine(const SequenceSummary& a, const SequenceSummary& b) {
        SequenceSummary result;
        result.score = a.score + b.score;
        result.openThrees = a.openThrees + b.openThrees;
        result.semiOpenThrees = a.semiOpenThrees + b.semiOpenThrees;
        result.openFours = a.openFours + b.openFours;
        result.semiOpenFours = a.semiOpenFours + b.semiOpenFours;
        return result;
    }

private:
    char _color; // BLACK(1) or WHITE(2)
    int _maxDepth;

    [[nodiscard]] static char getOpponent(char player) { return (player == BLACK) ? WHITE : BLACK; }

    [[nodiscard]] bool wouldWin(const BoardManager& boardManager,
                                BoardPosition position,
                                char player) const;

    [[nodiscard]] bool posesThreat(const BoardManager& boardManager,
                                   BoardPosition position,
                                   char player) const;

    // Get possible candidate moves within a certain radius of existing pieces
    [[nodiscard]] std::vector<BoardPosition> candidateMoves(const BoardManager& boardManager) const;

    // Heuristic evaluation of the board for a given player. Returns a score relative to the player's perspective.
    [[nodiscard]] int evaluate(const BoardManager& boardManager, char player) const;

    [[nodiscard]] static bool isInsideBoard(int row, int col);
    [[nodiscard]] static int sequenceScore(int length, int openSides);

    [[nodiscard]] std::pair<SequenceSummary, SequenceSummary> 
    evaluateSequences(const BoardManager& boardManager) const;

    [[nodiscard]] std::pair<SequenceSummary, SequenceSummary>
    evaluationSequencesParallel(const BoardManager& boardManager) const;

    [[nodiscard]] inline SequenceSummary evaluateForPlayerAtPos(
        const BoardManager& boardManager,
        char player,
        int row,
        int col
    ) const;

    [[nodiscard]] int centerControlBias(const BoardManager& boardManager, char player) const;

    // minimax with alpha-beta pruning. Returns a pair of (score, best move)
    [[nodiscard]] std::pair<int, BoardPosition>
    minimaxAlphaBeta(
        BoardManager& boardManager,
        int depth,
        bool isMaximizing,
        char currentPlayer,
        int alpha,
        int beta
    ) const;
};

