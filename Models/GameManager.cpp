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
    aiMove = GomokuAI::getBestMove(boardManager);
    if (!boardManager.isValidMove(aiMove)) {
        std::cerr << "AI attempted invalid move at " << aiMove << std::endl;
        exit(EXIT_FAILURE); // fatal
    }
    // Programs should force exit here if AI makes an invalid move (should never happen)
    return boardManager.makeMove(aiMove);
}