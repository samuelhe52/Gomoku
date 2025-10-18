//
// Created by Samuel He on 2025/10/9.
//

#ifndef GOMOKU_GOMOKUAI_H
#define GOMOKU_GOMOKUAI_H

#include "BoardManager.h"
#include <vector>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <limits>

#define MAX_DEPTH 4
#define MAX_SEARCH_RADIUS 2

class GomokuAI {
public:
    static BoardPosition getBestMove(BoardManager& boardManager);
    static BoardPosition randomMove(const BoardManager& boardManager);

    static void setColor(int c) { color = c; } // BLACK(1) or WHITE(2)
    [[nodiscard]] static int getColor() { return color; }

private:
    static int color; // BLACK(1) or WHITE(2)
    [[nodiscard]] static int getOpponent(int player) { return (player == BLACK) ? WHITE : BLACK; }

    // TODO: 
    // checked 1. hasNInRow(enable checking only for open) 
    // checked 2. possibleBestMoves(board, radius) -> get moves surrounding existing non-empty cells
    // checked 3. evaluate(board, player) -> heuristic evaluation, returns a score
    // checked 4. minimax() -> core algorithm for determining a best move
    // checked 5. alpha-beta pruning

    // Check if some player has N in a row. If onlyOpen is true, only count open N in a row.
    [[nodiscard]] static int nInRowCount(
        const BoardManager &boardManager,
        int player,
        int n,
        bool onlyOpen);

    // Check if some player has open N in a row. Returns a pair of
    [[nodiscard]] static int openNInRowCount(const BoardManager &boardManager,
                                             int player,
                                             int n) {
        return nInRowCount(boardManager, player, n, true);
    }

    [[nodiscard]] static int fourInRowCount(const BoardManager& boardManager, int player) {
        return nInRowCount(boardManager, player, 4, false);
    }

    // Get possible best moves within a certain radius of existing pieces
    [[nodiscard]] static std::vector<BoardPosition> possibleBestMoves(const BoardManager& boardManager, int radius);

    // Heuristic evaluation of the board for a given player. Returns a score relative to the player's perspective.
    [[nodiscard]] static int evaluate(const BoardManager& boardManager, int player);

    // minimax with alpha-beta pruning. Returns a pair of (score, best move)
    [[nodiscard]] static std::pair<int, BoardPosition> minimaxAlphaBeta(BoardManager& boardManager, int depth, bool isMaximizing, int currentPlayer, int alpha, int beta);
};


#endif //GOMOKU_GOMOKUAI_H