# Performance Optimizations for candidatesMoveCache

## Overview

This document describes the performance bottlenecks identified in the `candidatesMoveCache` system and the optimizations implemented.

## Identified Bottlenecks

### 1. **Hash Lookups for Existence Checks (Major Bottleneck)**

**Problem:** Using `count()` or hash table lookups to check if a candidate already exists in the cache during the hot loop.

```cpp
// OLD - hash computation + table lookup every iteration
if (candidateMovesCache.count(newPos) == 0) {
    candidateMovesCache.insert(newPos);
}
```

**Solution:** Use a parallel boolean array for O(1) guaranteed lookups. Also batch insert candidates for a small additional gain.

```cpp
// NEW - simple array access, no hashing needed
if (!candidateMap[newRow][newCol]) {
    lastRecord.addedCandidates.push_back(newPos);
    candidateMap[newRow][newCol] = true;
}
// After loop: batch insert (minor additional optimization)
candidateMovesCache.insert(lastRecord.addedCandidates.begin(), 
                           lastRecord.addedCandidates.end());
```

**Impact:** The boolean array eliminated the critical bottleneck by replacing hash lookups with direct array access. Batch insertion provides a small additional improvement.

---

### 2. **Unnecessary Set Copying**

**Problem:** `getCandidateMoves()` returned by value, copying the entire unordered_set.

```cpp
// OLD - copies entire set
std::unordered_set<BoardPosition> getCandidateMoves() const
```

**Solution:** Return by const reference.

```cpp
// NEW - no copying
const std::unordered_set<BoardPosition>& getCandidateMoves() const
```

**Impact:** Eliminates O(n) copy operation called thousands of times during minimax search.

---

### 3. **Intermediate Vector Allocation**

**Problem:** `candidatesAround()` created a temporary vector, then each element was checked.

```cpp
// OLD
for (const auto& newPos : candidatesAround(pos, candidateRadius)) {
    if (candidateMovesCache.count(newPos) == 0) {
        // ...
    }
}
```

**Solution:** Inline the candidate generation loop directly in `updateCandidatesCache()`.

```cpp
// NEW - no vector allocation
for (int dr = minRow - pos.row; dr <= maxRow - pos.row; ++dr) {
    for (int dc = minCol - pos.col; dc <= maxCol - pos.col; ++dc) {
        // Direct processing
    }
}
```

**Impact:** Eliminates vector allocation and deallocation on every move.

---

### 4. **Repeated Boundary Calculations**

**Problem:** Boundary checks were performed inside nested loops.

**Solution:** Pre-calculate bounds once.

```cpp
const int minRow = std::max(0, pos.row - candidateRadius);
const int maxRow = std::min(BOARD_SIZE - 1, pos.row + candidateRadius);
const int minCol = std::max(0, pos.col - candidateRadius);
const int maxCol = std::min(BOARD_SIZE - 1, pos.col + candidateRadius);
```

**Impact:** Reduces redundant calculations in tight loops.

---

### 5. **Inefficient Hash Function**

**Problem:** Hash function used XOR and multiple function calls.

```cpp
// OLD
return hash<int>()(pos.row) ^ (hash<int>()(pos.col) << 1);
```

**Solution:** Optimized bit-packing hash for small coordinate values.

```cpp
// NEW - single operation
return (static_cast<size_t>(pos.row) << 4) | static_cast<size_t>(pos.col);
```

**Impact:** Faster hash computation with better distribution for BOARD_SIZE=15.

---

### 6. **Hash Table Rehashing**

**Problem:** No initial capacity reservation, causing rehashing during gameplay.

**Solution:** Reserve capacity based on expected game size.

```cpp
candidateMovesCache.reserve(64);
```

**Impact:** Avoids costly rehashing operations during gameplay.

---

## Performance Improvements Summary

| Optimization | Impact |
|-------------|--------|
| **Boolean array for lookups** | **Major improvement** - Eliminates hash overhead in tight loop |
| Batch insert | Minor additional gain |
| Return by reference | Eliminates O(n) copies in minimax |
| Inline candidate generation | No vector allocations per move |
| Pre-calculate bounds | Fewer arithmetic ops in loops |
| Optimized hash function | Faster hash computation |
| Reserve capacity | Avoids rehashing during game |

**Overall Impact:**

- Significant reduction in cache operation time
- Noticeable improvement in AI move calculation speed
- Most beneficial in deeper minimax searches (depth 4-6)

---

## Further Optimization Opportunities

### 1. **Move Ordering Cache**

Cache the sorted move list to avoid re-sorting in `candidateMoves()`.

### 2. **Transposition Table**

Add position hashing to avoid recalculating the same board states.

### 3. **Bitboard Representation**

For very advanced optimization, consider using bitboards instead of 2D arrays.

### 4. **Parallel Search**

Implement parallel minimax for multi-core systems.

### 5. **Alternative Data Structure**

For very small candidate sets (<10), a sorted `std::vector` can be faster than `unordered_set` due to better cache locality.

---

## Testing Recommendations

1. **Benchmark before/after** using the same test positions
2. **Profile with instruments/perf** to verify bottleneck elimination
3. **Test edge cases**: early game (few candidates) vs. mid-game (many candidates)
4. **Verify correctness**: Run extensive game simulations to ensure no regression

---

*Last Updated: November 3, 2025*
