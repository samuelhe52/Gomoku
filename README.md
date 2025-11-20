# Gomoku

A classic Gomoku (Five in a Row) game built with Qt6 and C++.

[![Build](https://github.com/samuelhe52/Gomoku/actions/workflows/cmake-multi-platform.yml/badge.svg)](https://github.com/samuelhe52/Gomoku/actions/workflows/cmake-multi-platform.yml)

## Features

- **Modern UI**: Qt 6 Widgets with custom painting and responsive layout.
- **Smart AI**: Multithreaded AI opponent running in background threads.
- **Cross-Platform**: Windows, macOS, and Linux support.

## Downloads

Latest automated builds for Windows, macOS, and Linux are available as artifacts in [GitHub Actions](https://github.com/samuelhe52/Gomoku/actions).

## Getting Started

### Prerequisites

- CMake 3.31+
- Qt 6.8.0+
- C++17 compiler

### Quick Build (Makefile)

```bash
make build    # Configure and build
make launch   # Run the game
```

### Manual Build (CMake)

```bash
cmake -B cmake-build-release -DCMAKE_BUILD_TYPE=Release
cmake --build cmake-build-release --config Release
```

## Documentation & Structure

- **`UI/`**: Visual elements (MainWindow, GameWidget).
- **`Models/`**: Game logic and AI (BoardManager, GomokuAI).
- **`docs/`**: Detailed architectural and performance documentation.

See [AGENTS.md](AGENTS.md) for development guidelines and [DEPLOYMENT.md](DEPLOYMENT.md) for deployment.

## License

See [LICENSE](LICENSE) for details.
