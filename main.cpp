#include <iostream>
#include <vector>

#include "chess.h"

int main()
{
  // Chess *currGame = new Chess("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 0");
  // Chess *currGame = new Chess("3k4/3pp3/8/8/8/8/3PP3/3K4 w - - 0 1");
  // Chess *currGame = new Chess("5k2/8/8/8/8/8/8/4K2R w K - 0 1");
  Chess *currGame = new Chess("r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1");
  std::cout << currGame->Perft(6).toString() << std::endl;
  currGame->ClearPieces();
  delete currGame;
  return 0;
}