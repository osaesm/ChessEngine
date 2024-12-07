#include <iostream>
#include <memory>

#include "chess.hpp"

/**
 * TODO:
 * - First, try replacing premove dictionaries with arrays and see what that does (12% speedup; 27->24 sec)
 * - No precomputed Rook/Queen/Bishop moves for now
 * - Maybe remove pawn/knight/king precomputed moves
 * - Create Move class so moves can be done & undone on the same object
 */

int main()
{
  Chess::Initialize();
  std::unique_ptr<Chess> game(new Chess("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"));
  // std::unique_ptr<Chess> game(new Chess("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1"));
  std::cout << game->perft(6, Move::Check::NO_CHECK) << std::endl;
  return 0;
}
