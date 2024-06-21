#include <iostream>
#include <vector>

#include "chess.h"

int main()
{
    // Chess *currGame = new Chess("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 0");
    Chess *currGame = new Chess("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 0");
    std::cout << currGame->Perft(3) << std::endl;
    currGame->ClearPieces();
    delete currGame;
    return 0;
}