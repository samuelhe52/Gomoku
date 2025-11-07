# UI and Model Structure

The current UI and model layers separate responsibilities so the user
interface drives turn flow while core rules remain reusable.

```txt
┌─────────────────┐
│   GameWidget     │  QWidget controller
│─────────────────│
│ owns GameManager │
│ embeds BoardWidget
│ overlays ColorChooserWidget
└────────┬────────┘
         │ signals+slots
         ▼
┌─────────────────┐
│  BoardWidget     │  QWidget view
│─────────────────│
│ holds GameManager*
│ emits cellSelected(row,col)
│ paints board state
└────────┬────────┘
         │ direct calls
         ▼
┌─────────────────┐
│  GameManager     │  game orchestration
│─────────────────│
│ fields: human/AI color,
│          current turn, winner
│ uses BoardManager
│ uses GomokuAI for AI moves
└────────┬────────┘
         │ method calls
         ▼
┌─────────────────┐
│ BoardManager     │  board rules/state
│─────────────────│
│ validates moves
│ detects winners
│ manages candidate cache
└────────┬────────┘
         │ static helpers
         ▼
┌─────────────────┐
│   GomokuAI       │  AI engine
│─────────────────│
│ static color
│ minimax / heuristics
│ queries BoardManager
└─────────────────┘
```

## Responsibilities

- `GameWidget`: Coordinates UI widgets, listens to color selection, and
  issues human/AI moves through the manager.
- `BoardWidget`: Renders board state and relays click positions back to
  `GameWidget`; it does not mutate the model directly.
- `GameManager`: Keeps authoritative turn, winner, and board status while
  mediating between the board rules and AI logic.
- `BoardManager`: Owns the grid, validates moves, updates caches, and
  detects win/draw conditions.
- `GomokuAI`: Supplies best-move decisions with minimax evaluation using
  `BoardManager` queries.
