//
// Created by Samuel He on 2025/10/9.
//

#pragma once

#include "BoardManager.h"
#include "Constants.h"
#include <vector>
#include <future>
#include <QThread>
#include <QtConcurrent/QtConcurrent>
#include <QThreadPool>

class GomokuAI {
public:
    explicit GomokuAI(char color, int maxDepth = MAX_DEPTH);

    BoardPosition getBestMove(const BoardManager& boardManager) const;

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


    [[nodiscard]] SequenceSummary evaluateForPlayerAtPos(
        const BoardManager& boardManager,
        char player,
        int row,
        int col
    ) const;

private:
    char _color; // BLACK(1) or WHITE(2)
    int _maxDepth;

    // Cap the thread count to ensure pruning efficiency
    int threadCount = std::min(QThread::idealThreadCount(), 12);
    // Use mutable to allow const methods to use the thread pool
    mutable QThreadPool threadPool;

    [[nodiscard]] static char getOpponent(char player) { return (player == BLACK) ? WHITE : BLACK; }

    /// @brief Splits a vector into smaller chunks of specified size.
    /// @param content the vector to be split into chunks.
    /// @param chunkSize the desired size of each chunk. 
    /// @return The vector of chunks.
    /// @note The last chunk may be smaller if the total size is not divisible by chunkSize.
    template<typename T>
    [[nodiscard]] inline std::vector<std::vector<T>> splitIntoChunks(
        const std::vector<T>& content,
        int chunkSize
    ) const {
        std::vector<std::vector<T>> chunks;
        int totalSize = static_cast<int>(content.size());

        for (int i = 0; i < totalSize; i += chunkSize) {
            auto startIter = content.begin() + i;
            auto endIter = (i + chunkSize < totalSize) ? (startIter + chunkSize) : content.end();
            chunks.emplace_back(startIter, endIter);
        }

        return chunks;
    }

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

    [[nodiscard]] inline static bool isInsideBoard(int row, int col) {
        return row >= 0 && row < BOARD_SIZE && col >= 0 && col < BOARD_SIZE;
    }

    [[nodiscard]] inline static int sequenceScore(int length, int openSides) {
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

    [[nodiscard]] std::pair<SequenceSummary, SequenceSummary> 
    evaluateSequences(const BoardManager& boardManager) const;

    [[nodiscard]] int centerControlBias(const BoardManager& boardManager, char player) const;

    // minimax with alpha-beta pruning. Returns a pair of (score, best move)
    [[nodiscard]] std::pair<int, BoardPosition>
    minimaxAlphaBeta(
        BoardManager& boardManager,
        int depth,
        bool isMaximizing,
        int alpha,
        int beta
    ) const;

    // minimax with alpha-beta pruning and parallelizing the root level.
    // Returns a pair of (score, best move)
    [[nodiscard]] BoardPosition minimaxAlphaBetaRootParallel(
        BoardManager& boardManager,
        int depth
    ) const;
};

