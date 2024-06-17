.PHONY: all
all: chess check

chess: chess.cpp main.cpp
	g++ -std=c++20 -o out/chess chess.cpp main.cpp

check: chess.cpp tests.cpp
	g++ -std=c++20 -o out/check chess.cpp tests.cpp

.PHONY: clean
clean:
	rm -rf out/*