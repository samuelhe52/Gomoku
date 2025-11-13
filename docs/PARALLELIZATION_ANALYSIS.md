# Parallelization Analysis & Findings

## Executive Summary

Parallelizing board evaluation in the minimax algorithm **degrades performance** due to thread synchronization overhead dominating the actual computation time. Sequential evaluation is faster for this use case.

## Problem Description

### Initial Approach

Implemented parallel board evaluation using `QtConcurrent::blockingMappedReduced` with a custom thread pool:

```cpp
// Called thousands of times during minimax
int evaluate(const BoardManager &boardManager, const char player) const {
    const auto [playerSummary, opponentSummary] = evaluateSequencesParallel(boardManager);
    // ... scoring logic
}
```

The parallel implementation divided the 225 cells (15×15 board) into 5 chunks and evaluated them concurrently.

### Performance Bottleneck Identified

Profiler analysis revealed:

**Parent thread (minimax caller):**

- `QThreadPool::start()` - thread spawning overhead
- `QWaitCondition::wait()` - blocking until all threads complete

**Child threads:**

- `QWaitCondition::wait()` - waiting for work assignment
- `startThreadInternal()` - thread startup overhead

### Root Cause

The minimax algorithm calls `evaluate()` **thousands of times** per move decision (once for each board state explored). Each call incurred:

1. **Thread synchronization overhead**: 5-10ms
   - Waking/starting 5 threads
   - Wait conditions for task distribution
   - Wait conditions for result collection

2. **Actual computation time**: 2-3ms
   - Evaluating 225 board positions sequentially would take ~2-3ms
   - Parallel evaluation only saved ~1ms of computation time

**Net result**: **Paying 5-10ms overhead to save 1ms of computation = 5-10× slower**

## Solution

### Use Sequential Evaluation

```cpp
int evaluate(const BoardManager &boardManager, const char player) const {
    const char opponent = getOpponent(player);
    // Use sequential evaluation - parallel overhead dominates at this level
    const auto [playerSummary, opponentSummary] = evaluateSequences(boardManager);
    // ... scoring logic
}
```

**Rationale:**

- Single-threaded evaluation of 225 cells is fast enough (~2-3ms)
- No thread synchronization overhead
- Better cache locality with sequential access
- Simpler code with no race conditions to debug

## Key Learnings

### When Parallelism Hurts Performance

Parallelism is **counterproductive** when:

1. **Work granularity is too small** - The task completes faster than thread overhead
2. **Called in tight loops** - Synchronization overhead multiplies with call frequency
3. **Cache locality is important** - Parallel access can thrash caches
4. **Overhead > Speedup** - Thread management costs exceed parallel gains

### Amdahl's Law in Practice

For this use case:

- **Parallelizable portion**: ~60% (board evaluation)
- **Overhead**: 5-10ms per call
- **Call frequency**: Thousands per move
- **Result**: Sequential is faster

### Alternative Parallelization Strategy (Future Consideration)

Instead of parallelizing at the **evaluation level**, parallelize at the **decision tree level**:

```cpp
// Parallelize root move evaluation (called ONCE)
BoardPosition getBestMove(const BoardManager &boardManager) const {
    auto moves = candidateMoves(boardManager);
    
    // Evaluate each root move in parallel (5-20 moves)
    // Each evaluation runs sequential minimax
    auto results = QtConcurrent::blockingMapped(moves, [&](BoardPosition pos) {
        // ... run minimax for this move
    });
}
```

**Advantages:**

- Overhead paid **once** instead of thousands of times
- Much larger work units (entire minimax branches)
- Better parallelization efficiency

**Challenges:**

- Need to handle board state copying for thread safety
- Alpha-beta pruning is harder to implement across parallel branches
- Move ordering becomes more complex

## Benchmark Results

Under the depth of 5:

### Before (Parallel Evaluation using 4 threads)

- Average move calculation time: **~2-3 seconds**
- Profiler: 70-80% time in thread synchronization

### After (Sequential Evaluation)

- Average move calculation time: **~0.3 seconds**
- Profiler: 80-90% time in actual evaluation logic

## Conclusion

**For CPU-bound tasks called frequently in tight loops, sequential execution often outperforms parallelization due to thread synchronization overhead.**

The best parallelization strategy depends on:

1. Work granularity (larger is better for parallelism)
2. Call frequency (less frequent is better for parallelism)
3. Overhead-to-work ratio (must be << 1 for parallelism to help)

For Gomoku AI minimax, **sequential evaluation + potential top-level parallelization** is the optimal approach.

---

*Last updated: 2025-11-13*
