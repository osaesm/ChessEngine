#include <iostream>

#include "chess.h"

int main()
{
  Chess::Initialize();
  Chess *game = new Chess("4k3/1P6/8/8/8/8/K7/8 w - - 0 1");
  std::cout << game->perft(6) << std::endl;
  // std::cout << game->perft(2) << std::endl;
  // std::cout << game->perft(3) << std::endl;
  // Chess *game = new Chess("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 0");
  // std::cout << game->perft(1) << std::endl;
  delete game;
  return 0;
}