#include <iostream>
#include <vector>

#include "chess.h"

int main()
{
  // Chess *currGame = new Chess("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 0");
  // Chess *currGame = new Chess("3k4/3pp3/8/8/8/8/3PP3/3K4 w - - 0 1");
  // Chess *currGame = new Chess("5k2/8/8/8/8/8/8/4K2R w K - 0 1");
  // Chess *currGame = new Chess("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 0");
  Chess *currGame = new Chess("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");
  std::cout << currGame->Perft(5).toString() << std::endl;
  currGame->ClearPieces();
  delete currGame;
  return 0;
}