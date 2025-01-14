#include "chess.hpp"
#include <iostream>
#include <memory>
#include <string>

/**
 * TODO:
 * - Multithread perft
 * - Create an eval function
 *   - minimax, maybe NN/CNN, BFS vs DFS?
 * - Store previous results with indexes [Board idx][depth] = #
 */

int main(int argc, char *argv[]) {
  Chess::Initialize();

  int numThreads = 1;
  for (int i = 1; i < argc; ++i) {
    std::string arg = argv[i];
    if (arg.rfind("-numThreads=", 0) == 0) {
      try {
        numThreads = std::stoi(arg.substr(12));
      } catch (const std::exception &e) {
        std::cerr << "Invalid value for numThreads: " << arg.substr(12)
                  << std::endl;
        return 1;
      }
    }
  }
  // std::unique_ptr<Chess> game(
  //     new Chess("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"));

  std::unique_ptr<Chess> game(new Chess(
      "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1"));

  std::cout << game->perftRecurse(5, Move::Check::NO_CHECK, numThreads)
            << std::endl;
  return 0;
}
