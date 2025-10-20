//
// Created by Samuel He on 2025/10/9.
//

#include "GameManager.h"

#include <cstdlib>
#include <iostream>

GameManager::GameManager() {
    resetGame();
}

void GameManager::startNewGame(const char humanColor) {
    _humanColor = humanColor;
    _aiColor = (humanColor == BLACK) ? WHITE : BLACK;
    GomokuAI::setColor(_aiColor);
    resetGame();
}

void GameManager::resetGame() {
    boardManager.resetGame();
    _currentTurn = BLACK;
    _winner = EMPTY;
}

MoveResult GameManager::playHumanMove(const BoardPosition position) {
    if (!isHumansTurn()) {
        return {false, _winner, boardManager.isBoardFull(), {-1, -1}};
    }

    if (!canPlayAt(position)) {
        return {false, _winner, boardManager.isBoardFull(), {-1, -1}};
    }

    return applyMove(position);
}

MoveResult GameManager::playAIMove() {
    if (!isAITurn()) {
        return {false, _winner, boardManager.isBoardFull(), {-1, -1}};
    }

    if (_winner != EMPTY) {
        return {false, _winner, boardManager.isBoardFull(), {-1, -1}};
    }

    BoardPosition aiMove = GomokuAI::getBestMove(boardManager);
    if (!boardManager.isValidMove(aiMove)) {
        std::cerr << "AI attempted invalid move at " << aiMove << std::endl;
        exit(EXIT_FAILURE);
    }

    return applyMove(aiMove);
}

MoveResult GameManager::applyMove(const BoardPosition position) {
    MoveResult result;
    result.position = position;

    const char moveWinner = boardManager.makeMove(position);
    result.moveApplied = true;
    _currentTurn = (_currentTurn == BLACK) ? WHITE : BLACK;

    if (moveWinner != EMPTY) {
        _winner = moveWinner;
    }

    result.winner = _winner;
    result.boardIsFull = boardManager.isBoardFull();
    return result;
}

bool GameManager::canPlayAt(const BoardPosition position) const {
    if (_winner != EMPTY) {
        return false;
    }

    return boardManager.isValidMove(position);
}