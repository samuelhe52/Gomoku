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
private:
};


#endif //GOMOKU_GOMOKUAI_H