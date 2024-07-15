#include <iostream>

#include "chess.h"

int main()
{
  Chess::Initialize();
  Chess *game = new Chess("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8");
  std::cout << game->perft(1) << std::endl;
  // std::cout << game->perft(2) << std::endl;
  // std::cout << game->perft(3) << std::endl;
  // Chess *game = new Chess("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 0");
  // std::cout << game->perft(1) << std::endl;
  delete game;
  return 0;
}