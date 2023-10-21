#include <iostream>
#include <vector>
#include <unordered_map>

#include "board.h"
#define BISHOP_MOVEMENT \
                short leftCol = currCol - 1;\
                short upRow = currRow + 1;\
                short upLeftIdx = 8 * (upRow - 1) + leftCol - 1;\
                while (leftCol > 0 && upRow <= 8 && ((isEmpty(upLeftIdx - 1) || activePieces[upLeftIdx]->color != turn)))\
                {\
                    legalOptions.push_back(std::pair<short, short>(currPosition, upLeftIdx));\
                    leftCol--;\
                    upRow++;\
                }\
                short rightCol = currCol + 1;\
                upRow = currRow + 1;\
                short upRightIdx = 8 * (upRow - 1) + rightCol - 1;\
                while (rightCol <= 8 && upRow <= 8 && (isEmpty(upRightIdx - 1) || activePieces[upRightIdx]->color != turn))\
                {\
                    legalOptions.push_back(std::pair<short, short>(currPosition, upRightIdx));\
                    rightCol++;\
                    upRow++;\
                }\
                rightCol = currCol + 1;\
                short downRow = currRow - 1;\
                short downRightIdx = 8 * (downRow - 1) + rightCol - 1;\
                while (rightCol <= 8 && downRow > 0 && (isEmpty(downRightIdx - 1) || activePieces[downRightIdx]->color != turn))\
                {\
                    legalOptions.push_back(std::pair<short, short>(currPosition, downRightIdx));\
                    rightCol++;\
                    downRow--;\
                }\
                leftCol = currCol - 1;\
                downRow = currRow - 1;\
                short leftDownIdx = 8 * (downRow - 1) + leftCol - 1;\
                while (leftCol > 0 && downRow > 0 && (isEmpty(leftDownIdx - 1) || activePieces[leftDownIdx]->color != turn))\
                {\
                    legalOptions.push_back(std::pair<short, short>(currPosition, leftDownIdx));\
                    leftCol--;\
                    downRow--;\
                }

#define ROOK_MOVEMENT \
                if (currRow < 8) {\
                    for (short up = currPosition + 8; up < 64 && (isEmpty(up) || activePieces[up]->color != turn); up += 8)\
                    {\
                        legalOptions.push_back(std::pair<short, short>(currPosition, up));\
                    }\
                }\
                if (currRow > 1) {\
                    for (short down = currPosition - 8; down >= 0 && (isEmpty(down) || activePieces[down]->color != turn); down -= 8)\
                    {\
                        legalOptions.push_back(std::pair<short, short>(currPosition, down));\
                    }\
                }\
                if (currCol < 8) {\
                    for (short right = currPosition + 1; (right % 8) != 0 && (isEmpty(right) || activePieces[right]->color != turn); right++)\
                    {\
                        legalOptions.push_back(std::pair<short, short>(currPosition, right));\
                    }\
                }\
                if (currCol > 1) {\
                    for (short left = currPosition - 1; (left % 8) != 7 && (isEmpty(left) || activePieces[left]->color != turn); left--)\
                    {\
                        legalOptions.push_back(std::pair<short, short>(currPosition, left));\
                    }\
                }

Board::Board()
{
    turn = BLACK;
    whiteCastle = true;
    whiteLongCastle = true;
    blackCastle = true;
    blackLongCastle = true;

    // Initialize white second row
    activePieces[0] = new Piece(WHITE, ROOK);
    activePieces[1] = new Piece(WHITE, KNIGHT);
    activePieces[2] = new Piece(WHITE, BISHOP);
    activePieces[3] = new Piece(WHITE, QUEEN);
    activePieces[4] = new Piece(WHITE, KING);
    activePieces[5] = new Piece(WHITE, BISHOP);
    activePieces[6] = new Piece(WHITE, KNIGHT);
    activePieces[7] = new Piece(WHITE, ROOK);

    // Initialize white pawns
    for (short i = 8; i < 16; i++)
    {
        activePieces[i] = new Piece(WHITE, PAWN);
    }

    // Initialize black pawns
    for (short i = 48; i < 56; i++)
    {
        activePieces[i] = new Piece(BLACK, PAWN);
    }

    // Initialize black second row
    activePieces[56] = new Piece(BLACK, ROOK);
    activePieces[57] = new Piece(BLACK, KNIGHT);
    activePieces[58] = new Piece(BLACK, BISHOP);
    activePieces[59] = new Piece(BLACK, QUEEN);
    activePieces[60] = new Piece(BLACK, KING);
    activePieces[61] = new Piece(BLACK, BISHOP);
    activePieces[62] = new Piece(BLACK, KNIGHT);
    activePieces[63] = new Piece(BLACK, ROOK);
}

Board::~Board()
{
    for (auto piece : activePieces)
    {
        delete piece.second;
        activePieces.erase(piece.first);
    }
}

TURN_COLORS Board::Start()
{
    for (auto p : LegalMoves())
    {
        std::cout << p.first << " to " << p.second << std::endl;
    }
    return WHITE;
}

std::vector<std::pair<short, short>> Board::LegalMoves()
{
    std::vector<std::pair<short, short>> legalOptions;
    for (auto currPiece : activePieces)
    {
        auto currPosition = currPiece.first;
        auto currColor = currPiece.second->color;
        auto currType = currPiece.second->pieceType;

        short currRow = currPosition / 8 + 1;
        short currCol = currPosition + 8 - 8 * currRow + 1;
        if (currColor == turn)
        {
            switch (currType)
            {
            case PAWN:
            {
                // std::cout << "starting pawn" << std::endl;
                if (turn == WHITE)
                {
                    bool blocked = !isEmpty(currPosition + 8);
                    // Check if pawn can move 2 up
                    if (currRow == 2 && !blocked && isEmpty(currPosition + 16))
                    {
                        legalOptions.push_back(std::pair<short, short>{currPosition, currPosition + 8});
                        legalOptions.push_back(std::pair<short, short>{currPosition, currPosition + 16});
                    }
                    else if (currRow < 8 && !blocked)
                    {
                        // If not 2 spaces, check for 1
                        legalOptions.push_back(std::pair<short, short>{currPosition, currPosition + 8});
                    }
                    // Check left diagonal take
                    if (currRow < 8 && currCol > 1 && !isEmpty(currPosition + 7) && activePieces[currPosition + 7]->color != turn)
                    {
                        legalOptions.push_back(std::pair<short, short>(currPosition, currPosition + 7));
                    }
                    // Check right diagonal take
                    if (currRow < 8 && currCol < 8 && !isEmpty(currPosition + 9) && activePieces[currPosition + 9]->color != turn)
                    {
                        legalOptions.push_back(std::pair<short, short>(currPosition, currPosition + 9));
                    }
                } else {
                    bool blocked = !isEmpty(currPosition - 8);
                    // Check if pawn can move 2 down
                    if (currRow == 7 && !blocked && isEmpty(currPosition - 16))
                    {
                        legalOptions.push_back(std::pair<short, short>{currPosition, currPosition - 8});
                        legalOptions.push_back(std::pair<short, short>{currPosition, currPosition - 16});
                    }
                    else if (currRow > 1 && !blocked)
                    {
                        // If not 2 spaces, check for 1
                        legalOptions.push_back(std::pair<short, short>{currPosition, currPosition - 8});
                    }
                    // Check left diagonal take
                    if (currRow > 1 && currCol > 1 && !isEmpty(currPosition - 9) && activePieces[currPosition - 9]->color != turn)
                    {
                        legalOptions.push_back(std::pair<short, short>(currPosition, currPosition - 9));
                    }
                    // Check right diagonal take
                    if (currRow > 1 && currCol > 1 && !isEmpty(currPosition - 7) && activePieces[currPosition - 7]->color != turn)
                    {
                        legalOptions.push_back(std::pair<short, short>(currPosition, currPosition - 7));
                    }
                }
                // std::cout << "finshing pawn" << std::endl;
                break;
            }
            case KNIGHT:
            {
                // std::cout << "starting knight" << std::endl;
                if (currRow > 2 && currCol > 1 && (isEmpty(currPosition - 17) || activePieces[currPosition - 17]->color != currColor))
                {
                    legalOptions.push_back(std::pair<short, short>(currPosition, currPosition - 17));
                }
                if (currRow > 1 && currCol > 2 && (isEmpty(currPosition - 10) || activePieces[currPosition - 10]->color != currColor))
                {
                    legalOptions.push_back(std::pair<short, short>(currPosition, currPosition - 10));
                }
                if (currRow < 8 && currCol > 2 && (isEmpty(currPosition + 6) || activePieces[currPosition + 6]->color != currColor))
                {
                    legalOptions.push_back(std::pair<short, short>(currPosition, currPosition + 6));
                }
                if (currRow < 7 && currCol > 1 && (isEmpty(currPosition + 15) || activePieces[currPosition + 15]->color != currColor))
                {
                    legalOptions.push_back(std::pair<short, short>(currPosition, currPosition + 15));
                }
                if (currRow < 7 && currCol < 8 && (isEmpty(currPosition + 17) || activePieces[currPosition + 17]->color != currColor))
                {
                    legalOptions.push_back(std::pair<short, short>(currPosition, currPosition + 17));
                }
                if (currRow < 8 && currCol < 7 && (isEmpty(currPosition + 10) || activePieces[currPosition + 10]->color != currColor))
                {
                    legalOptions.push_back(std::pair<short, short>(currPosition, currPosition + 10));
                }
                if (currRow > 1 && currCol < 7 && (isEmpty(currPosition - 6) || activePieces[currPosition - 6]->color != currColor))
                {
                    legalOptions.push_back(std::pair<short, short>(currPosition, currPosition - 6));
                }
                if (currRow > 2 && currCol < 8 && (isEmpty(currPosition - 15) || activePieces[currPosition - 15]->color != currColor))
                {
                    legalOptions.push_back(std::pair<short, short>(currPosition, currPosition - 15));
                }
                // std::cout << "finishing knight" << std::endl;
                break;
            }
            case BISHOP:
            {
                // std::cout << "starting bishop" << std::endl;
                BISHOP_MOVEMENT
                // std::cout << "finishing bishop" << std::endl;
                break;
            }
            case ROOK:
            {
                // std::cout << "starting rook" << std::endl;
                ROOK_MOVEMENT
                // std::cout << "finishing rook" << std::endl;
                break;
            }
            case QUEEN:
            {
                // std::cout << "starting queen" << std::endl;
                BISHOP_MOVEMENT
                ROOK_MOVEMENT
                // std::cout << "finishing queen" << std::endl;
                break;
            }
            case KING:
            {
                // std::cout << "starting king" << std::endl;
                if (currRow < 8 && currCol < 8)
                {
                    for (auto k : {1, 8, 9}) {
                        if (isEmpty(currPosition + k) || activePieces[currPosition + k]->color != turn) {
                            legalOptions.push_back(std::pair<short, short>(currPosition, currPosition + k));
                        }
                    }
                }
                else if (currCol < 8)
                {
                    if (isEmpty(currPosition + 1) || activePieces[currPosition + 1]->color != turn) {
                        legalOptions.push_back(std::pair<short, short>(currPosition, currPosition + 1));
                    }
                }
                else if (currRow < 8)
                {
                    if (isEmpty(currPosition + 8) || activePieces[currPosition + 8]->color != turn) {
                        legalOptions.push_back(std::pair<short, short>(currPosition, currPosition + 8));
                    }
                }
                if (currRow > 1 && currCol > 1)
                {
                    for (auto k : {1, 8, 9}) {
                        if (isEmpty(currPosition - k) || activePieces[currPosition - k]->color != turn) {
                            legalOptions.push_back(std::pair<short, short>(currPosition, currPosition - k));
                        }
                    }
                }
                else if (currRow > 1)
                {
                    if (isEmpty(currPosition - 8) || activePieces[currPosition - 8]->color != turn) {
                        legalOptions.push_back(std::pair<short, short>(currPosition, currPosition - 8));
                    }
                }
                else if (currCol > 1)
                {
                    if (isEmpty(currPosition - 1) || activePieces[currPosition - 1]->color != turn) {
                        legalOptions.push_back(std::pair<short, short>(currPosition, currPosition - 1));
                    }
                }
                if (currRow > 1 && currCol < 8)
                {
                    if (isEmpty(currPosition - 7) || activePieces[currPosition - 7]->color != turn) {
                        legalOptions.push_back(std::pair<short, short>(currPosition, currPosition - 7));
                    }
                }
                if (currRow < 8 && currCol > 1)
                {
                    if (isEmpty(currPosition + 7) || activePieces[currPosition + 7]->color != turn) {
                        legalOptions.push_back(std::pair<short, short>(currPosition, currPosition + 7));
                    }
                }
                // std::cout << "finishing king" << std::endl;
                break;
            }
            };
        }
    }
    return legalOptions;
}
