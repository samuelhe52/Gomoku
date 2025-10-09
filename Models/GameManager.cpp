//
// Created by Samuel He on 2025/10/9.
//

#include "GameManager.h"

GameManager::GameManager() = default;

int GameManager::makeMove(const BoardPosition position) {
    if (!boardManager.isValidMove(position)) return EMPTY; // Invalid move
    const int winner = boardManager.makeMove(position);
    if (winner != EMPTY) return winner; // Player wins
    // AI's turn
    return makeAIMove();
}

int GameManager::makeAIMove() {
    // AI's turn
    // Get AI move
    BoardPosition aiMove{};
    do {
        aiMove = GomokuAI::getBestMove(boardManager);
    } while (!boardManager.isValidMove(aiMove));
    // Programs should be stuck here if AI makes an invalid move (should never happen)
    return boardManager.makeMove(aiMove);
}