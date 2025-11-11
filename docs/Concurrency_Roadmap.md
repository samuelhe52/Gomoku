<!-- markdownlint-disable MD041 -->

Parallelizing evaluate() is almost senseless here because the overhead of creating threads is killing the performance. We're going to turn to OpenMP or QT-based threading models to try save evaluate(). If that doesn't work, we'll switch to copying board every time when recursing into a new minimaxAlphaBeta() function.

---

# C++ Concurrency Roadmap: Parallelizing evaluate() in GomokuAI

## Where parallelism fits in your code

In `GomokuAI::evaluate`, most of the work happens in `evaluateSequences`, which:

- Scans the whole board, visiting each cell.
- For each stone, walks four directions and updates a running summary (`SequenceSummary`) for the AI and the opponent.

This is a textbook "embarrassingly parallel map + reduce" pattern:

- Map: independently compute per-cell sequence contributions.
- Reduce: combine per-thread partial `SequenceSummary`s into one result.

You'll aim to parallelize `evaluateSequences` safely and deterministically via local accumulators + a final reduction (no shared writes inside the inner loops).

## Quick "contract" for a parallel evaluate

- Inputs: `const BoardManager&`, `char player`.
- Output: deterministic `int` identical to the single-threaded version.
- Constraints:
  - No data races: read-only board, per-thread local `SequenceSummary`.
  - Reduction is associative and thread-safe.
  - Overhead must not dominate on small boards or few stones.
- Edge cases:
  - Empty board (fast path already exists in `getBestMove`).
  - Early win/lose cases (already handled in `minimaxAlphaBeta`).
  - False sharing between threads writing to adjacent memory (mitigate with local objects and final merge).

---

## Learning roadmap (targeted to your evaluate())

### Phase 1 — Essentials of C++ concurrency (1–2 sessions)

- What to learn:
  - Threads and tasks: `std::thread`, `std::async`, thread lifecycle (`join`, `detach`).
  - Data races vs. race conditions. C++ memory model (happens-before at a high level).
  - Synchronization tools: `std::mutex`, `std::lock_guard`, `std::unique_lock`, `std::condition_variable`.
  - Atomics: when they help (counters), when they don't (compound structs like `SequenceSummary`).
- Why it matters here:
  - You'll avoid global/shared updates during the tight loops in `evaluateSequences`.
- Resources:
  - cppreference: Threads, Futures, Atomics, Mutex.
  - "C++ Concurrency in Action" (Anthony Williams), Chapters 2–5.

### Phase 2 — Parallel patterns you'll use (1 session)

- What to learn:
  - Map–reduce pattern for numeric aggregations.
  - Reduction strategies: local accumulation per worker + final merge.
  - Work partitioning: rows, tiles, or linearized indices; choosing chunk size.
  - Avoid false sharing (keep each thread's accumulator separate).
- Exercise in this repo:
  - Define `SequenceSummary& operator+=(const SequenceSummary&)` and `SequenceSummary combine(const SequenceSummary&, const SequenceSummary&)`.
  - Write a quick unit or ad-hoc test that:
    - Runs the current serial `evaluateSequences`.
    - Runs a stub "parallel" version that just splits work into two halves in the same thread and reduces.
    - Asserts equality of summaries and final `evaluate`.

### Phase 3 — Pick your parallel tool (choose 1 to start)

Your project already uses Qt (`QThread`), so you have three viable routes:

Option A: Standard library parallel algorithms (portable, minimal deps)

- What to learn:
  - `std::for_each` with `std::execution::par` (C++17/20), `std::transform_reduce`.
  - Limitations: parallel algorithms require random-access iteration and careful capture of locals.
- How you'd apply it:
  - Linearize board indices 0..N-1.
  - `transform_reduce(std::execution::par, indices.begin(), indices.end(), initialSummary, combine, perIndexFn)`.
  - `perIndexFn` builds a local `SequenceSummary` for that index (both players), returns it.
  - `combine` merges two `SequenceSummary`s.

Option B: QtConcurrent (fits Qt projects nicely)

- What to learn:
  - `QtConcurrent::mappedReduced` or `blockingMappedReduced`.
  - Thread-safety rules for reducer (reduce runs in a single thread by default, or specify `QtConcurrent::UnorderedReduce`).
- How you'd apply it:
  - Map: index → local `SequenceSummary`.
  - Reduce: `summary += local;` in the reducer function.
- Pros: Simple; integrates with Qt's thread pool and cancellation patterns.
- Cons: Adds QtConcurrent module dependency if not already linked.

Option C: OpenMP pragmas (quickest way to parallelize loops)

- What to learn:
  - `#pragma omp parallel for reduction(...)` for custom reductions (requires decl of custom reduction or reduce on scalar fields and combine at the end).
- Pros: Very fast to prototype; great for loop-parallel evaluation.
- Cons: Compiler and build flag dependent; slightly less portable; custom reduction for a struct needs extra boilerplate.

Pick one to implement first (I'd suggest QtConcurrent for fastest integration in a Qt app or std::execution for portability).

### Phase 4 — Implement a safe reduction (1–2 sessions)

- Steps:
  1. Add `operator+=` for `SequenceSummary`.
  2. Extract the "per-cell contribution" into a pure function that:
     - Reads board state for (row, col).
     - Computes contributions for both players (as your current `evaluateForPlayer` does).
     - Returns a `std::pair<SequenceSummary, SequenceSummary>` or a combined struct.
  3. Parallelize over all cells:
     - Each worker computes a local pair (player/opponent).
     - Reduce with `+=` into a global pair.
  4. Replace the serial double loop in `evaluateSequences` with your chosen parallel pattern.
- Correctness checks:
  - Bit-for-bit equality with the serial implementation on:
    - Empty board
    - Opening moves
    - Mid-game sparse board
    - Dense endgame
  - Run 100+ randomized boards and assert equality (optional).

### Phase 5 — Measure and set thresholds (1 session)

- What to learn:
  - Microbenchmarking with `std::chrono::steady_clock`.
  - Understand overhead: on small boards or few stones, single-thread may be faster.
- What to do:
  - Add timing around `evaluate`.
  - Only enable parallel path if:
    - Number of occupied cells exceeds a threshold, or
    - Candidate set size is large, or
    - Depth is shallow but breadth is high.
  - Use `std::thread::hardware_concurrency()` to cap parallelism (or use Qt's default pool).

### Phase 6 — Integrate with the rest of the engine (1–2 sessions)

- Topics:
  - Interactions with `minimaxAlphaBeta`: while you're starting with `evaluate`, the biggest speedup later typically comes from parallelizing the root ply in `minimax`.
  - Cancellation: you already check `QThread::currentThread()->isInterruptionRequested()` in search. Avoid putting that check inside the tight inner loops of `evaluateSequences` to keep it fast; checking once per chunk is fine if you support cancellation.
  - Exception safety: keep per-task functions `noexcept` if possible, or handle exceptions in the reducer.

### Phase 7 — Performance tuning and pitfalls (ongoing)

- Tuning ideas:
  - Chunking: process rows or tiles to improve cache locality.
  - Pre-filter: skip empty zones; you already have `getCandidateMoves()`—consider a fast bounding box to minimize scanned area.
  - Avoid repeated `getCell` bounds checks by hoisting conditionals or using tight loops per direction.
  - Consider using separate accumulators per direction and combine—sometimes improves branch predictability.
- Pitfalls to avoid:
  - Writing to shared `SequenceSummary` from multiple threads (don't).
  - Non-associative reductions (ensure `a += b` is associative enough for integer arithmetic here).
  - False sharing (keep thread-local objects on separate cache lines if they're long-lived; simple locals returned by value are fine).

---

## Concrete "mini-syllabus" with repo exercises

1. Read/skim

- cppreference: std::thread, futures, mutex, atomics, execution policies.
- Qt docs: QtConcurrent overview, mappedReduced.
- Optional: OpenMP loop parallelism and custom reductions.

1. Repo exercise 1 (serial refactor)

- Add `SequenceSummary operator+=`.
- Extract a pure function: `SequenceSummaryPair evaluateCell(const BoardManager&, int row, int col)`.
- Reimplement `evaluateSequences` by looping cells and doing `total += evaluateCell(...)` (still serial).
- Confirm `evaluate` unchanged across a few scenarios.

1. Repo exercise 2 (first parallel version)

- Implement one approach:
  - std::execution::par with transform_reduce, or
  - QtConcurrent::blockingMappedReduced, or
  - OpenMP `parallel for` with final combine.
- Verify correctness vs serial on random boards.

1. Repo exercise 3 (benchmark + threshold)

- Add a lightweight timing helper.
- Print or log evaluation time for serial vs parallel.
- Introduce a simple heuristic to choose serial/parallel.

1. Stretch goal

- Parallelize only the top ply of `minimaxAlphaBeta` (split candidate moves among threads), keep deeper plies serial to avoid overhead and complexity.

---

## Example sketches (for learning, not drop-in code)

- Std parallel transform_reduce shape:
  - Indices: `std::vector<int> idx(BOARD_SIZE*BOARD_SIZE); std::iota(...)`
  - transform: idx → local pair of `SequenceSummary`s
  - reduce: pair-combine using `+=`
- QtConcurrent mappedReduced shape:
  - Map lambda: index → local pair
  - Reduce lambda: `accum += local;` (runs in reducer thread by default)

These examples reinforce the map–reduce mental model you'll use in `evaluateSequences`.

---

## Recommended references

- Anthony Williams — "C++ Concurrency in Action" (2nd ed.)
- cppreference.com:
  - Threads and tasks: std::thread, std::async, std::future
  - Synchronization: std::mutex, std::lock_guard, std::condition_variable, std::atomic
  - Parallel algorithms: execution policies, transform_reduce, for_each
- Qt docs:
  - QtConcurrent::mappedReduced
  - QThread interruption and thread pools
- OpenMP (if you try it): Official OpenMP docs/tutorials (sections on `parallel for` and reduction)

---

## Suggested timeline

- Day 1: Phases 1–2 (fundamentals + patterns), Repo exercise 1 (serial refactor).
- Day 2: Phase 3–4 (choose tool, first parallel version), correctness tests.
- Day 3: Phase 5 (benchmarks + threshold), Phase 6 (integration notes), initial tuning.
- Ongoing: Phase 7 tuning and, later, root-level search parallelism.

---

## What you'll have at the end

- A solid grasp of C++ concurrency basics.
- A parallel `evaluateSequences` using a clean map–reduce implementation with deterministic results.
- Benchmarks that prove the speedup and a heuristic to avoid overhead on small positions.
- A path to parallelizing the search root for even larger gains.

If you want, I can implement the serial refactor (operator+= and evaluateCell) now as a safe first step, then layer in your chosen parallel approach.

---
