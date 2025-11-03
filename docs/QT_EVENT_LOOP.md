# Qt Event Loop and Signal-Slot Behavior

## Overview
This document explains how Qt's event loop, signals/slots, and widget painting interact, based on findings from debugging the Gomoku UI update issue.

## Key Concepts

### 1. Signal Emission (emit)

**Default behavior with Qt::AutoConnection (same thread):**
```cpp
emit moveApplied(result);  // Direct function call - executes IMMEDIATELY
// Connected slot runs here, synchronously
// Next line executes only AFTER all connected slots finish
```

`emit` is **synchronous** by default when sender and receiver are in the same thread. It's essentially a direct function call to all connected slots.

**Different threads:**
```cpp
emit moveApplied(result);  // Queued - added to receiver's event queue
// Returns immediately
// Slot executes later when receiver's event loop processes it
```

### 2. Widget Painting is Deferred

Widget update methods like `refresh()` or `update()` don't paint immediately:

```cpp
board->refresh();  // Marks widget as "needs repaint"
                   // Adds paint event to event queue
                   // Returns immediately
```

Actual painting happens later when the event loop processes paint events.

### 3. Event Compression/Coalescing

Qt optimizes redundant events. If the same widget gets multiple paint requests before the event loop processes them, Qt merges them into a single paint event:

```cpp
board->refresh();  // Adds paint event to queue
// ... some code ...
board->refresh();  // Qt sees BoardWidget already needs painting
                   // Doesn't add another event - reuses the existing one
```

## The Problem We Encountered

### Issue
When a human made a move in Gomoku, only the AI's response appeared on the board. The human's move seemed to be skipped.

### Root Cause

```cpp
void GameManager::handleHumanMove(const BoardPosition position) {
    MoveResult result = playHumanMove(position);
    emit moveApplied(result);  // GameWidget slot runs immediately
                                // board->refresh() marks board as dirty
                                // BUT painting is deferred
    
    // AI move calculated immediately - blocks UI thread
    MoveResult aiResult = playAIMove();
    emit moveApplied(aiResult);  // Another refresh request
    
    // Function returns, event loop processes events
    // Paint events were coalesced - only one paint with BOTH moves
}
```

**Event timeline (BEFORE fix):**
1. Human move applied to board state
2. `emit moveApplied(humanResult)` → `board->refresh()` adds paint event to queue
3. AI move calculated (blocking, takes time)
4. AI move applied to board state
5. `emit moveApplied(aiResult)` → `board->refresh()` tries to add paint event
6. Qt coalesces the paint events into one
7. Function returns, event loop runs
8. Single paint event executes → board shows final state (human + AI moves together)

### Solution: QTimer::singleShot with Small Delay

```cpp
void GameManager::handleHumanMove(const BoardPosition position) {
    MoveResult result = playHumanMove(position);
    emit moveApplied(result);
    
    // Defer AI move to allow UI to update first
    if (isAITurn() && result.winner == EMPTY && !result.boardIsFull) {
        QTimer::singleShot(1, this, [this]() {  // 1ms delay
            MoveResult aiResult = playAIMove();
            if (aiResult.moveApplied) {
                emit moveApplied(aiResult);
            }
        });
    }
}
```

**Why `QTimer::singleShot(0, ...)` didn't work:**

Using a 0ms delay schedules the callback for the next event loop iteration, but Qt's paint event system is also deferred. Even though the paint event was queued, Qt might not have processed it by the time the timer callback runs with 0ms delay. The events might still be processed in the same batch.

**Solution:** Use a small non-zero delay (1-10ms) to ensure the paint event has time to be processed before the AI move starts.

**Event timeline (AFTER fix with 1ms delay):**

1. Human move applied
2. `emit moveApplied(humanResult)` → `board->refresh()` adds paint event
3. `QTimer::singleShot(1, ...)` schedules timer callback for ~1ms from now
4. Function returns
5. Event loop processes paint event → **Human move visible** ✓
6. ~1ms passes (enough time for painting to complete)
7. Timer callback fires → AI calculates and applies move
8. `emit moveApplied(aiResult)` → Another paint event added
9. Event loop processes paint event → **AI move visible** ✓

## QTimer::singleShot Explained

```cpp
QTimer::singleShot(delayMs, contextObject, callback);
```

**Parameters:**
- `delayMs`: Delay in milliseconds before callback executes
- `contextObject`: If this object is deleted, the callback is cancelled (prevents crashes)
- `callback`: Lambda or function to execute

**Why a small delay (1-10ms) is needed:**

A 0ms delay only guarantees the callback runs in the next event loop iteration, but doesn't guarantee paint events have been processed. Qt's event system processes multiple events per iteration, and paint events might be batched together.

A small non-zero delay (1-10ms) gives the UI thread enough time to:
1. Process the paint event
2. Actually render the widget to screen
3. Complete before the AI calculation starts

**Event Queue with `singleShot(1)`:**
```
[Paint BoardWidget] → [Render] → ... → [Timer Callback: AI move] → [Paint BoardWidget] → ...
      ↑ Processed first            ↑ ~1ms later              ↑ Processed after AI move
```

This introduces a barely noticeable delay (1-10ms is imperceptible to users) but ensures smooth visual feedback.

## Best Practices

1. **Long-running operations** should not block the UI thread. Use `QTimer::singleShot(0, ...)` to defer work and let UI updates happen.

2. **Widget updates** are automatically deferred by Qt. Don't assume `update()` or `refresh()` paints immediately.

3. **Signal emission** is synchronous by default (same thread). Be aware that connected slots run immediately unless you explicitly use `Qt::QueuedConnection`.

4. **Event compression** is a Qt optimization. Multiple redundant events of the same type for the same object may be merged.

## References

- Qt Documentation: [The Event System](https://doc.qt.io/qt-6/eventsandfilters.html)
- Qt Documentation: [QTimer](https://doc.qt.io/qt-6/qtimer.html)
- Qt Documentation: [Signals & Slots](https://doc.qt.io/qt-6/signalsandslots.html)
