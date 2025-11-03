# Performance Optimizations for candidatesMoveCache

## Overview
This document describes the performance bottlenecks identified in the `candidatesMoveCache` system and the optimizations implemented.

## Identified Bottlenecks

### 1. **Redundant `count()` Calls Before Operations**
**Problem:** The code was calling `count()` to check existence, then calling `insert()` or `erase()`.
```cpp
// OLD - 2 hash lookups
if (candidateMovesCache.count(pos) == 0) {
    candidateMovesCache.insert(pos);
}
```

**Solution:** Use the return value of `insert()` and `erase()` directly.
```cpp
// NEW - 1 hash lookup
auto [it, inserted] = candidateMovesCache.insert(pos);
if (inserted) { /* ... */ }

// For erase
size_t removed = candidateMovesCache.erase(pos);
if (removed > 0) { /* ... */ }
```

**Impact:** ~50% reduction in hash table lookups during cache updates.

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

| Optimization | Expected Improvement |
|-------------|---------------------|
| Eliminate redundant `count()` | ~50% fewer hash lookups |
| Return by reference | Eliminates O(n) copies in minimax |
| Inline candidate generation | No vector allocations per move |
| Pre-calculate bounds | Fewer arithmetic ops in loops |
| Optimized hash function | ~3-5x faster hashing |
| Reserve capacity | Avoids 3-4 rehashes per game |

**Overall Expected Impact:** 
- **40-60% reduction** in cache operation time
- **20-30% improvement** in overall AI move calculation speed
- Especially noticeable in deeper minimax searches (depth 4-6)

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
