#include <iostream>

#include "chess.h"

int main()
{
  Chess::Initialize();
  Chess * game = new Chess("8/8/8/2k5/2pP4/8/B7/4K3 b - d3 0 3");
  // Chess * game = new Chess("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 0");
  // Chess * game = new Chess("rnbqkbnr/pppppppp/8/8/8/6P1/PPPPPP1P/RNBQKBNR b KQkq - 0 0");
  // std::cout << game->ConvertToFEN() << std::endl;
  std::cout << game->perft(1) << std::endl;
  // std::cout << game->perft(5) << std::endl;
  // game->divide(1);
  delete game;
  return 0;
}