#include <iostream>

#include "chess.h"

int main()
{
  Chess::Initialize();
  Chess *game = new Chess("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 0");
  std::cout << game->perft(1) << std::endl;
  delete game;
  return 0;
}