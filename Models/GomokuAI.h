//
// Created by Samuel He on 2025/10/9.
//

#ifndef GOMOKU_GOMOKUAI_H
#define GOMOKU_GOMOKUAI_H

#include "BoardManager.h"
#include <vector>
#include <cstdlib>
#include <ctime>

class GomokuAI {
public:
    static BoardPosition getBestMove(const BoardManager& boardManager);
    static BoardPosition foolishMove(const BoardManager& boardManager);
    static BoardPosition randomMove(const BoardManager& boardManager);

    static void setColor(int c) { color = c; } // BLACK(1) or WHITE(2)

private:
    static int color; // BLACK(1) or WHITE(2)
    [[nodiscard]] static int getColor() { return color; }
    [[nodiscard]] static int getOpponentColor() { return (color == BLACK) ? WHITE : BLACK; }

    // Check if opponent has three in a row 
    [[nodiscard]] static std::vector<BoardPosition> blockOpponentThreeInRow(
        const BoardManager& boardManager
    ) {
        const auto results =  blockingPositions(boardManager, 3);
        for (const auto& res : results) {
            if (res.player == getOpponentColor() && !res.positions.empty()) {
                return res.positions;
            }
        }
        return {};
    }

    struct BlockingPositions {
        std::vector<BoardPosition> positions; // Available blocking positions
        int player; // The player who has N in a row
    };

    // Check if some player has N in a row and return a set of available blocking positions
    [[nodiscard]] static std::vector<BlockingPositions> blockingPositions(
        const BoardManager& boardManager,
        int n // Number of in a row to check for
    );
};


#endif //GOMOKU_GOMOKUAI_H