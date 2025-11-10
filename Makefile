.PHONY: build
build:
	mkdir -p cmake-build-release
	cd cmake-build-release && cmake -DCMAKE_BUILD_TYPE=Release ../
	cmake --build cmake-build-release

.PHONY: launch
launch:
	cmake --build cmake-build-release && cmake-build-release/Gomoku

.PHONY: perf
perf:
	cmake --build cmake-build-release && cmake-build-release/GomokuAIPerf

.PHONY: clean
clean:
	rm -rf cmake-build-release
