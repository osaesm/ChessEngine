#include <iostream>
#include <vector>

#include "chess.h"

int main()
{
    // Chess *currGame = new Chess("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 0");
    // Chess *currGame = new Chess("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 0");
    Chess *currGame = new Chess("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 0");
    // Chess *currGame = new Chess("r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1");
    std::cout << currGame->Perft(3) << std::endl;
    currGame->ClearPieces();
    delete currGame;
    return 0;
}