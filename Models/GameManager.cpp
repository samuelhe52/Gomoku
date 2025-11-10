//
// Created by Samuel He on 2025/10/9.
//

#include "GameManager.h"

#include <cstdlib>
#include <iostream>
#include <new>

GameManager::GameManager(QObject *parent) : QObject(parent) {
    initializeNewGameState();
}

void GameManager::startNewGame(const char humanColor) {
    _humanColor = humanColor;
    _aiColor = (humanColor == BLACK) ? WHITE : BLACK;
    _aiEngine = new GomokuAI(_aiColor);
    initializeNewGameState();
    // If AI goes first, make the first move
    if (isAITurn()) {
        makeAIFirstMove();
    }
}

void GameManager::initializeNewGameState() {
    boardManager = BoardManager();
    _currentTurn = BLACK;
    _winner = EMPTY;
}

void GameManager::handleHumanMove(const BoardPosition position) {
    MoveResult result = playHumanMove(position);
    if (!result.moveApplied) {
        return;
    }
    
    emit moveApplied(result);
    
    if (isAITurn() && result.winner == EMPTY && !result.boardIsFull) {
        MoveResult aiResult = playAIMove();
        if (aiResult.moveApplied) {
            emit moveApplied(aiResult);
        }
    }
}

void GameManager::makeAIFirstMove() {
    if (!isAITurn() || _winner != EMPTY) return;
    
    MoveResult result = playAIMove();
    if (result.moveApplied) {
        emit moveApplied(result);
    }
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
    const MoveResult invalidResult = {false, _winner, boardManager.isBoardFull(), {-1, -1}};
    if (!isAITurn() || _winner != EMPTY) {
        return invalidResult;
    }

    if (!_aiEngine) {
        _aiEngine = new GomokuAI(_aiColor);
    }

    BoardPosition aiMove = _aiEngine->getBestMove(boardManager);
    if (!boardManager.isValidMove(aiMove)) {
        std::cerr << "AI attempted invalid move at " << aiMove << std::endl;
        return invalidResult;
    }

    return applyMove(aiMove);
}

MoveResult GameManager::applyMove(const BoardPosition position) {
    MoveResult result;
    result.position = position;

    const char placed = _currentTurn;
    const char moveWinner = boardManager.makeMove(position);
    result.moveApplied = true;
    result.placedColor = placed;
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