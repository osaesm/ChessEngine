#include <iostream>

#include "chess.h"

int main()
{
    Chess *currGame = new Chess("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 0");
    // currGame->PrintBoard();
    std::cout << currGame->ConvertToFEN() << std::endl;
    free(currGame);
    return 0;
}