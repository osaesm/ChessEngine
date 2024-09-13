#include <iostream>

#include "chess.hpp"

/**
 * TODO:
 * - Benchmark some perft stuff before and after
 * - Maybe try storing positions with int array instead of FEN? (12 piece types * 64 squares + 4 castling rights + 1 enpassantsquare + 1 turnColor = 774)
 * - First, try replacing premove dictionaries with arrays and see what that does
 * - No precomputed Rook/Queen/Bishop moves for now
 * - Maybe remove pawn/knight/king precomputed moves
 * - Create Move class so moves can be done & undone on the same object
 */

int main()
{
  Chess::Initialize();
  // Chess *game = new Chess("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 0");
  Chess *game = new Chess("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");
  std::cout << game->perft(4) << std::endl;
  delete game;
  return 0;
}