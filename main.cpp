#include <iostream>
#include <memory>

#include "chess.hpp"

/**
 * TODO:
 * - Multithread perft
 * - Create an eval function
 *   - minimax, maybe NN/CNN, BFS vs DFS?
 * - Store previous results with indexes [Board idx][depth] = #
 */

int main() {
  Chess::Initialize();
  // std::unique_ptr<Chess> game(
  //     new Chess("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"));
  std::unique_ptr<Chess> game(new Chess(
      "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1"));
  std::cout << game->perft(5, Move::Check::NO_CHECK) << std::endl;
  return 0;
}
