#include <iostream>
#include <vector>

#include "chess.h"

int main()
{
    Chess *currGame = new Chess("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 0");
    // currGame->PrintBoard();
    // std::cout << currGame->ConvertToFEN() << std::endl;
    // std::cout << currGame->InCheck(4, Piece::Color::WHITE) << std::endl;
    int depth = 4;
    std::vector<Chess *> previousLayer;
    std::vector<Chess *> nextLayer;
    previousLayer.push_back(currGame);
    for (int x = 0; x < depth; ++x) {
        std::cout << previousLayer.size() << std::endl;
        if (nextLayer.size() != 20 && nextLayer.size()) {
            for (Chess * nextGame: nextLayer) {
                nextGame->PrintBoard();
            }
            break;
        }
        for (auto y = 0; y < previousLayer.size(); ++y) {
            Chess *game = previousLayer[y];
            for (Chess* nextGame : game->LegalMoves()) {
                nextLayer.push_back(nextGame);
            }
            free(game);
        }
        previousLayer = nextLayer;
        nextLayer.clear();
    }
    std::cout << previousLayer.size() << std::endl;
    free(currGame);
    return 0;
}