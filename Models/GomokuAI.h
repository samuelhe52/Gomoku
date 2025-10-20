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

    // TODO: 
    // checked 1. hasNInRow(enable checking only for open) 
    // checked 2. possibleBestMoves(board, radius) -> get moves surrounding existing non-empty cells
    // checked 3. evaluate(board, player) -> heuristic evaluation, returns a score
    // checked 4. minimax() -> core algorithm for determining a best move
    // checked 5. alpha-beta pruning

    // Check if some player has N in a row. Returns a pair of (open count, closed count).
    [[nodiscard]] static std::pair<int, int> nInRowCount(
        const BoardManager &boardManager,
        char player,
        int n);

    // Check if some player has open N in a row. Returns a pair of
    [[nodiscard]] static int openNInRowCount(const BoardManager &boardManager,
                                             char player,
                                             int n) {
        return nInRowCount(boardManager, player, n).first;
    }

    [[nodiscard]] static int fourInRowCount(const BoardManager& boardManager, char player) {
        auto [openCount, closedCount] = nInRowCount(boardManager, player, 4);
        return openCount + closedCount;
    }

    // Get possible candidate moves within a certain radius of existing pieces
    [[nodiscard]] static std::vector<BoardPosition> candidateMoves(const BoardManager& boardManager);

    // Heuristic evaluation of the board for a given player. Returns a score relative to the player's perspective.
    [[nodiscard]] static int evaluate(const BoardManager& boardManager, char player);

    // minimax with alpha-beta pruning. Returns a pair of (score, best move)
    [[nodiscard]] static std::pair<int, BoardPosition> minimaxAlphaBeta(BoardManager& boardManager, int depth, bool isMaximizing, char currentPlayer, int alpha, int beta);
};


#endif //GOMOKU_GOMOKUAI_H