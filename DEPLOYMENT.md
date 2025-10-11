# Deployment Guide

This document provides detailed instructions for creating deployable packages of the Gomoku application.

## Quick Deployment

### Windows - Bundled with Qt DLLs
```bash
cmake -B cmake-build-release -DCMAKE_BUILD_TYPE=Release
cmake --build cmake-build-release --config Release
mkdir deploy
cp cmake-build-release/Release/Gomoku.exe deploy/
windeployqt deploy/Gomoku.exe --release --no-translations
```

The `deploy/` folder now contains a self-contained application with all Qt dependencies.

### macOS - Bundle Qt Frameworks
```bash
cmake -B cmake-build-release -DCMAKE_BUILD_TYPE=Release
cmake --build cmake-build-release --config Release
mkdir deploy
cp cmake-build-release/Gomoku deploy/
macdeployqt deploy/Gomoku -always-overwrite
```

### Linux - Basic Binary
```bash
cmake -B cmake-build-release -DCMAKE_BUILD_TYPE=Release
cmake --build cmake-build-release --config Release
mkdir deploy
cp cmake-build-release/Gomoku deploy/
```

**Note**: Linux binaries require Qt6 to be installed on the target system.

## Troubleshooting

### Windows
- **Missing DLL errors**: Ensure `windeployqt` was run successfully
- **Antivirus warnings**: Sign the executable or add exception

### macOS
- **Gatekeeper blocking**: Run `xattr -cr Gomoku` or right-click → Open
- **Missing frameworks**: Ensure `macdeployqt` completed without errors

### Linux
- **Library version mismatch**: Run `ldd Gomoku` to check dependencies
- **Qt not found**: Install Qt6 via package manager:
  - Ubuntu/Debian: `sudo apt-get install qt6-base-dev`
  - Fedora: `sudo dnf install qt6-qtbase`
  - Arch: `sudo pacman -S qt6-base`
