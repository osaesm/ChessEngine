#include "board.h"

/**
 * TODO:
 * - Work on figuring out if piece is pinned to king or not
 * - 
 * - Figure out robust testing?
*/

int main() {
    // Initialize game
    Board* gameBoard = new Board();
    gameBoard->Start();
    return 0;
}