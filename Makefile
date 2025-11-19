.PHONY: build launch perf test clean help pdf
build:
	mkdir -p cmake-build-release
	cd cmake-build-release && cmake -DCMAKE_BUILD_TYPE=Release ../
	cmake --build cmake-build-release

launch:
	cmake --build cmake-build-release && cmake-build-release/Gomoku

perf:
	cmake --build cmake-build-release && cmake-build-release/GomokuAIPerf

test:
	cmake --build cmake-build-release && cmake-build-release/GomokuAIOverHeadTests

clean:
	rm -rf cmake-build-release

pdf:
	@./Report/generate-pdf.sh

help:
	@echo "Available commands:"
	@echo "  build   - Build the Gomoku project in release mode"
	@echo "  launch  - Build and launch the Gomoku application"
	@echo "  perf    - Build and run the Gomoku AI performance tests"
	@echo "  test    - Build and run the Gomoku AI overhead tests"
	@echo "  clean   - Remove build artifacts"
	@echo "  help    - Show this help message"
