#include "board.h"

/**
 * TODO:
 * - Rooks
 * - Knights
 * - Bishops
 * - Queens
 * - Kings
 * - Black everything
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