#include <iostream>
#include <vector>

#include "chess.h"

int main()
{
  Chess *currGame = new Chess("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 0");
  std::cout << currGame->Perft(3) << std::endl;
  currGame->ClearPieces();
  delete currGame;
  return 0;
}