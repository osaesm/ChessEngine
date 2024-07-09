#include <iostream>

#include "chess.h"

int main()
{
  Chess::Initialize();
  Chess * game = new Chess("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 0");
  // Chess * game = new Chess("rnbqkbnr/pppppppp/8/8/8/6P1/PPPPPP1P/RNBQKBNR b KQkq - 0 0");
  std::cout << game->ConvertToFEN() << std::endl;
  std::cout << game->perft(1) << std::endl;
  std::cout << game->perft(2) << std::endl;
  delete game;
  return 0;
}