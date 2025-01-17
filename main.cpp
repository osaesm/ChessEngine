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

  // Chess game("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
  Chess game("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");

  std::cout << game.perftRecurse(5, Move::Check::NO_CHECK)
            << std::endl;
  return 0;
}
