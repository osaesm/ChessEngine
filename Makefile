.PHONY: all
all: chess

chess: chess.cpp board.cpp
	g++ -std=c++20 -o out/chess chess.cpp board.cpp

check: tests.cpp board.cpp
	g++ -std=c++20 -o out/check tests.cpp board.cpp

.PHONY: clean
clean:
	rm -rf out/*