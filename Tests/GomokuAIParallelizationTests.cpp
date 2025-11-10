//
// Created by GitHub Copilot on 2025/11/10.
//

#include "../Models/GomokuAI.h"
#include "../Models/BoardManager.h"
#include "../Models/Constants.h"

#include <chrono>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

namespace {
	struct Scenario {
		std::string name;
		BoardManager board;
		char aiColor;
        BoardPosition expectedMove;
	};

	Scenario buildScenario(
        const std::string& name,
        const std::vector<BoardPosition>& moves,
        BoardPosition expectedMove
    ) {
		BoardManager board;
		char winner = EMPTY;

		for (const auto& move : moves) {
			if (!board.isValidMove(move)) {
				std::cerr << "Invalid seeded move at " << move << " in scenario '" << name << "'\n";
				continue;
			}

			winner = board.makeMove(move);
			if (winner != EMPTY) {
				std::cerr << "Scenario '" << name << "' produced a winner while seeding; stopping early.\n";
				break;
			}
		}

		const char aiColor = (moves.size() % 2 == 0) ? BLACK : WHITE;
		return {name, board, aiColor, expectedMove};
	}

	std::vector<Scenario> buildScenarios() {
		std::vector<Scenario> scenarios;

        scenarios.push_back(buildScenario(
            "First Move after Center",
            {
                {7, 7}
            },
            {6, 6}
        ));

		scenarios.push_back(buildScenario(
			"Early Opening Pressure",
			{
				{7, 7}, {7, 8}, {7, 6}, {6, 6}, {8, 7}, {6, 7}
			},
            {6, 5}
		));

		scenarios.push_back(buildScenario(
			"Midgame Crossfire",
			{
				{7, 7}, {7, 8}, {6, 7}, {8, 8}, {8, 7}, {6, 8}, {9, 7}, {5, 7}, {9, 6}
			},
            {10, 7}
		));

		scenarios.push_back(buildScenario(
			"Late-Game Threat Net",
			{
				{7, 7}, {8, 8}, {7, 6}, {8, 7}, {7, 8}, {8, 6}, {6, 7}, {9, 7}, {6, 6}, {9, 6}, {10, 7}, {5, 7}
			},
            {8, 9}
		));

		return scenarios;
	}

	void runScenario(const Scenario& scenario) {
		GomokuAI ai(scenario.aiColor);

		const auto start = std::chrono::steady_clock::now();
		const BoardPosition bestMove = ai.getBestMove(scenario.board);
		const auto stop = std::chrono::steady_clock::now();
		const auto elapsedMs = std::chrono::duration<double, std::milli>(stop - start);

		const char color = scenario.aiColor;
		const char* colorName = (color == BLACK) ? "Black" : "White";

        if (bestMove != scenario.expectedMove) {
            std::cout << "Scenario: " << scenario.name << " FAILED!\n"
                      << "  Expected move: (" << scenario.expectedMove.row << ", " << scenario.expectedMove.col << ")\n"
                      << "  Got move     : (" << bestMove.row << ", " << bestMove.col << ")\n\n";
            return;
        }

		std::cout << "Scenario: " << scenario.name << "\n"
				  << "  AI color      : " << colorName << "\n"
				  << "  Best move     : (" << bestMove.row << ", " << bestMove.col << ")\n"
				  << "  Elapsed (ms)  : " << std::fixed << std::setprecision(2) << elapsedMs.count() << "\n\n";
	}
}

int main() {
    const auto scenarios = buildScenarios();
    std::cout << "Evaluating " << scenarios.size() << " Gomoku AI scenarios...\n\n";

    for (const auto& scenario : scenarios) {
        runScenario(scenario);
    }

    std::cout << "Done." << std::endl;
    return 0;
}
