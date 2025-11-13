# Gomoku

A classic Gomoku (Five in a Row) game built with Qt6 and C++.

[![Build](https://github.com/samuelhe52/Gomoku/actions/workflows/cmake-multi-platform.yml/badge.svg)](https://github.com/samuelhe52/Gomoku/actions/workflows/cmake-multi-platform.yml)

## Download & Run

### Windows

1. Download the latest `gomoku-windows-latest` artifact from [GitHub Actions](https://github.com/samuelhe52/Gomoku/actions)
2. Extract the ZIP file
3. Double-click `Gomoku.exe` to run

### macOS / Linux

1. Download the latest artifact for your platform from [GitHub Actions](https://github.com/samuelhe52/Gomoku/actions)
2. Extract and make executable: `chmod +x Gomoku`
3. Run: `./Gomoku`
4. **Requires Qt6 to be installed on your system**

## Building from Source

### Prerequisites

- CMake 3.31 or higher
- Qt 6.8.0 or higher
- C++17 compatible compiler

### Build Instructions

```bash
# Clone the repository
git clone https://github.com/samuelhe52/Gomoku.git
cd Gomoku

# Configure and build
cmake -B cmake-build-release -DCMAKE_BUILD_TYPE=Release
cmake --build cmake-build-release --config Release
```

### Running the Game

- **Windows**: `cmake-build-release/Release/Gomoku.exe`
- **macOS/Linux**: `cmake-build-release/Gomoku`

## Deployment

For detailed deployment instructions including creating standalone packages, see [DEPLOYMENT.md](DEPLOYMENT.md).

## License

See [LICENSE](LICENSE) for details.
