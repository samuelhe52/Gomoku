#include "../Models/GomokuAI.h"
#include "../Models/BoardManager.h"

#include <thread>
#include <future>
#include <chrono>
#include <iostream>

// Tests for testing whether the overhead of creating threads outweighs the benefits

void testAsyncOverhead() {
    using namespace std::chrono;

    constexpr int iterations = 1000;

    // Test thread creation overhead
    auto start = high_resolution_clock::now();

    for (int i = 0; i < iterations; ++i) {
        std::async(std::launch::async, []{}).get();  // create and immediately join
    }

    auto end = high_resolution_clock::now();

    auto total_ns = duration_cast<nanoseconds>(end - start).count();
    double avg_us = (total_ns / 1000.0) / iterations;

    std::cout << "Average async task creation+join overhead: "
              << avg_us << " microseconds\n";
}

void testEvaluationTime() {
    using namespace std::chrono;

    constexpr int iterations = 1000;

    // Test evaluation for 60 cells
    GomokuAI ai(WHITE);
    BoardManager boardManager;
    const BoardPosition testPos{7, 7};
    boardManager.makeMove(testPos);

    BoardPosition posToEvaluate = {7, 8};
    auto evalStart = high_resolution_clock::now();
    for (int i = 0; i < iterations; ++i) {
        auto _ = ai.evaluateForPlayerAtPos(boardManager, WHITE, posToEvaluate.row, posToEvaluate.col);
    }
    auto evalEnd = high_resolution_clock::now();

    auto evalTotal_ns = duration_cast<nanoseconds>(evalEnd - evalStart).count();
    double evalAvg_us = (evalTotal_ns / 1000.0) / iterations;

    std::cout << "Average evaluation for a single cell: "
              << evalAvg_us << " microseconds\n";
}

int main() {
    testAsyncOverhead();
    testEvaluationTime();
}