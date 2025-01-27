#include "chess.hpp"
#include <chrono>
#include <cmath>
#include <format>
#include <iostream>
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
  Chess game(
      "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");
  const auto start = std::chrono::high_resolution_clock::now();
  uint64_t res = game.perftRecurse(6, Move::Check::NO_CHECK);
  const auto end = std::chrono::high_resolution_clock::now();
  const auto total_time =  std::chrono::duration<double, std::milli> (end - start).count() / 1000.0;
  std::cout << res << " nodes" << std::endl
            << std::format("{:.2f}", total_time) << " seconds" << std::endl
            << std::format("{:L}", std::lround(res / total_time)) << " nodes / sec" << std::endl;
  return 0;
}
