# Repository Guidelines

## Project Structure & Module Organization

This is a Qt6-based C++ Gomoku game with a clean separation between UI and game logic:

- **`UI/`** - Qt widgets and UI components (MainWindow, GameWidget, BoardWidget, ColorChooserWidget)
- **`Models/`** - Core game logic (BoardManager, GameManager, GomokuAI, Constants.h)
- **`main.cpp`** - Application entry point
- **`cmake-build-release/`** - Build output directory
- **`docs/`** - Project documentation

The architecture uses Qt's signal/slot mechanism with GameManager running in a background thread for AI computation.

## Build, Test, and Development Commands

### Using Make (recommended for quick development)
```bash
make build    # Configure and build the project
make launch   # Build and run the game
make clean    # Remove build artifacts
```

### Using CMake directly
```bash
# Configure
cmake -B cmake-build-release -DCMAKE_BUILD_TYPE=Release

# Build
cmake --build cmake-build-release --config Release

# Run (platform-specific)
cmake-build-release/Gomoku                  # macOS/Linux
cmake-build-release/Release/Gomoku.exe      # Windows
```

**Prerequisites**: CMake 3.31+, Qt 6.8.0+, C++17 compiler

## Coding Style & Naming Conventions

- **Indentation**: 4 spaces (no tabs)
- **Standard**: C++17
- **File headers**: Include creation date comments (`// Created by ... on YYYY/MM/DD`)
- **Naming**: 
  - Classes: PascalCase (e.g., `BoardManager`, `GameWidget`)
  - Variables: camelCase (e.g., `gameManager`, `boardFullSnapshot`)
  - Constants: UPPER_SNAKE_CASE (e.g., `BLACK`, `WHITE`, `EMPTY`)
  - Private members: camelCase with descriptive names
- **Header guards**: Use `#pragma once` followed by traditional include guards
- **Qt conventions**: Follow Qt naming for slots/signals, use `Q_OBJECT` macro

## Commit & Pull Request Guidelines

### Commit Message Patterns
Based on repository history, use concise, imperative descriptions:

- Good: `Fix compute not cancelled on reset button press`
- Good: `Optimize updateCandidatesCache()`
- Good: `Add threat detection evaluate()`
- Avoid: Generic messages like "Update code" or "Fix bug"

Focus on **what changed** and **why**. Use present tense for functionality descriptions.

### Pull Request Requirements
- Ensure code compiles on all platforms (tested via CI)
- Maintain compatibility with Qt6 and C++17 standards
- Test UI responsiveness and AI behavior
- Link related issues if applicable
- Keep changes focused and atomic

## Architecture Notes

- **Threading**: GameManager runs in a separate QThread to prevent UI blocking during AI computation
- **State management**: BoardManager handles board state; GameManager coordinates game flow
- **Meta types**: Custom types (e.g., `MoveResult`) are registered with Qt's meta-object system for cross-thread signals
