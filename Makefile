.PHONY: build
build:
	cmake --build cmake-build-release

.PHONY: launch
launch:
	cmake --build cmake-build-release && cmake-build-release/Gomoku
