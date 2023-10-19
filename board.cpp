#include <iostream>
#include <vector>
#include <unordered_map>

#include "board.h"

Board::Board()
{
    turn = WHITE;
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
        if (p.first < 8) {
            std::cout << p.first << " to " << p.second << std::endl;
        }
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
                if (turn == WHITE)
                {
                    bool blocked = (bitBoard & (1L << (currPosition + 8))) != 0;
                    // Check if pawn can move 2 up
                    if (currRow == 2 && !blocked && (bitBoard & (1L << (currPosition + 16)) != 0L))
                    {
                        legalOptions.push_back(std::pair<short, short>{currPosition, currPosition + 8});
                        legalOptions.push_back(std::pair<short, short>{currPosition, currPosition + 16});
                    }
                    else if (currRow < 7 && !blocked)
                    {
                        // If not 2 spaces, check for 1
                        legalOptions.push_back(std::pair<short, short>{currPosition, currPosition + 8});
                    }
                    // Check left diagonal take
                    if (currCol > 1 && (bitBoard & (1L << (currPosition + 7))) != 0L)
                    {
                        legalOptions.push_back(std::pair<short, short>(currPosition, currPosition + 7));
                    }
                    // Check right diagonal take
                    if (currCol < 8 && (bitBoard & (1L << (currPosition + 9))) != 0L)
                    {
                        legalOptions.push_back(std::pair<short, short>(currPosition, currPosition + 9));
                    }
                }
                break;
            }
            case KNIGHT:
            {
                if (currRow > 2 && currCol > 1 && currPosition >= 17 && ((bitBoard & (1L << (currPosition - 17))) == 0 || activePieces[currPosition - 17]->color != currColor))
                {
                    legalOptions.push_back(std::pair<short, short>(currPosition, currPosition - 17));
                }
                if (currRow > 1 && currCol > 2 && currPosition >= 10 && ((bitBoard & (1L << (currPosition - 10))) == 0 || activePieces[currPosition - 10]->color != currColor))
                {
                    legalOptions.push_back(std::pair<short, short>(currPosition, currPosition - 10));
                }
                if (currRow < 8 && currCol > 2 && currPosition <= 57 && ((bitBoard & (1L << (currPosition + 6))) == 0 || activePieces[currPosition + 6]->color != currColor))
                {
                    legalOptions.push_back(std::pair<short, short>(currPosition, currPosition + 6));
                }
                if (currRow < 7 && currCol > 1 && currPosition <= 48 && ((bitBoard & (1L << (currPosition + 15))) == 0 || activePieces[currPosition + 15]->color != currColor))
                {
                    legalOptions.push_back(std::pair<short, short>(currPosition, currPosition + 15));
                }
                if (currRow < 7 && currCol < 8 && currPosition <= 46 && ((bitBoard & (1L << (currPosition + 17))) == 0 || activePieces[currPosition + 17]->color != currColor))
                {
                    legalOptions.push_back(std::pair<short, short>(currPosition, currPosition + 17));
                }
                if (currRow < 8 && currCol < 7 && currPosition <= 53 && ((bitBoard & (1L << (currPosition + 10))) == 0 || activePieces[currPosition + 10]->color != currColor))
                {
                    legalOptions.push_back(std::pair<short, short>(currPosition, currPosition + 10));
                }
                if (currRow > 1 && currCol < 7 && currPosition >= 6 && ((bitBoard & (1L << (currPosition - 6))) == 0 || activePieces[currPosition - 6]->color != currColor))
                {
                    legalOptions.push_back(std::pair<short, short>(currPosition, currPosition - 6));
                }
                if (currRow > 2 && currCol < 8 && currPosition >= 15 && ((bitBoard & (1L << (currPosition - 15))) == 0 || activePieces[currPosition - 15]->color != currColor))
                {
                    legalOptions.push_back(std::pair<short, short>(currPosition, currPosition - 15));
                }
                break;
            }
            case BISHOP:
            {
                short leftCol = currCol - 1;
                short upRow = currRow + 1;
                short upLeftIdx = 8 * (upRow - 1) + leftCol - 1;
                while (leftCol > 0 && upRow <= 8 && (((bitBoard & (1L << (upLeftIdx - 1))) == 0L) || activePieces[upLeftIdx]->color != turn))
                {
                    legalOptions.push_back(std::pair<short, short>(currPosition, upLeftIdx));
                    leftCol--;
                    upRow++;
                }
                short rightCol = currCol + 1;
                upRow = currRow + 1;
                short upRightIdx = 8 * (upRow - 1) + rightCol - 1;
                while (rightCol <= 8 && upRow <= 8 && (((bitBoard & (1L << (upRightIdx - 1))) == 0L) || activePieces[upRightIdx]->color != turn))
                {
                    legalOptions.push_back(std::pair<short, short>(currPosition, upRightIdx));
                    rightCol++;
                    upRow++;
                }
                rightCol = currCol + 1;
                short downRow = currRow - 1;
                short downRightIdx = 8 * (downRow - 1) + rightCol - 1;
                while (rightCol <= 8 && downRow > 0 && (((bitBoard & (1L << (downRightIdx - 1))) == 0L) || activePieces[downRightIdx]->color != turn))
                {
                    legalOptions.push_back(std::pair<short, short>(currPosition, downRightIdx));
                    rightCol++;
                    downRow--;
                }
                leftCol = currCol - 1;
                downRow = currRow - 1;
                short leftDownIdx = 8 * (downRow - 1) + leftCol - 1;
                while (leftCol > 0 && downRow > 0 && (((bitBoard & (1L << (leftDownIdx - 1))) == 0L) || activePieces[leftDownIdx]->color != turn))
                {
                    legalOptions.push_back(std::pair<short, short>(currPosition, leftDownIdx));
                    leftCol--;
                    downRow--;
                }
                break;
            }
            case ROOK:
            {
                if (currRow < 8) {
                    for (short up = currPosition + 8; up < 64 && (((bitBoard & (1L << up - 1)) == 0L) || activePieces[up]->color != turn); up += 8)
                    {
                        legalOptions.push_back(std::pair<short, short>(currPosition, up));
                        if ((bitBoard & (1L << up)) != 0) {
                            break;
                        }
                    }
                }
                if (currRow > 1) {
                    for (short down = currPosition - 8; down >= 0 && (((bitBoard & (1L << down - 1)) == 0L) || activePieces[down]->color != turn); down -= 8)
                    {
                        legalOptions.push_back(std::pair<short, short>(currPosition, down));
                        if ((bitBoard & (1L << down)) != 0) {
                            break;
                        }
                    }
                }
                if (currCol < 8) {
                    for (short right = currPosition + 1; (right % 8) != 0 && (((bitBoard & (1L << right - 1)) == 0L) || activePieces[right]->color != turn); right++)
                    {
                        legalOptions.push_back(std::pair<short, short>(currPosition, right));
                        if ((bitBoard & (1L << right)) != 0) {
                            break;
                        }
                    }
                }
                if (currCol > 1) {
                    for (short left = currPosition - 1; (left % 8 != 7) && (((bitBoard & (1L << left - 1)) == 0L) || activePieces[left]->color != turn); left--)
                    {
                        legalOptions.push_back(std::pair<short, short>(currPosition, left));
                        if ((bitBoard & (1L << left)) != 0) {
                            break;
                        }
                    }
                }
                break;
            }
            case QUEEN:
            {
                short leftCol = currCol - 1;
                short upRow = currRow + 1;
                short upLeftIdx = 8 * (upRow - 1) + leftCol - 1;
                while (leftCol > 0 && upRow <= 8 && (((bitBoard & (1L << (upLeftIdx - 1))) == 0L) || activePieces[upLeftIdx]->color != turn))
                {
                    legalOptions.push_back(std::pair<short, short>(currPosition, upLeftIdx));
                    leftCol--;
                    upRow++;
                }
                short rightCol = currCol + 1;
                upRow = currRow + 1;
                short upRightIdx = 8 * (upRow - 1) + rightCol - 1;
                while (rightCol <= 8 && upRow <= 8 && (((bitBoard & (1L << (upRightIdx - 1))) == 0L) || activePieces[upRightIdx]->color != turn))
                {
                    legalOptions.push_back(std::pair<short, short>(currPosition, upRightIdx));
                    rightCol++;
                    upRow++;
                }
                rightCol = currCol + 1;
                short downRow = currRow - 1;
                short downRightIdx = 8 * (downRow - 1) + rightCol - 1;
                while (rightCol <= 8 && downRow > 0 && (((bitBoard & (1L << (downRightIdx - 1))) == 0L) || activePieces[downRightIdx]->color != turn))
                {
                    legalOptions.push_back(std::pair<short, short>(currPosition, downRightIdx));
                    rightCol++;
                    downRow--;
                }
                leftCol = currCol - 1;
                downRow = currRow - 1;
                short leftDownIdx = 8 * (downRow - 1) + leftCol - 1;
                while (leftCol > 0 && downRow > 0 && (((bitBoard & (1L << (leftDownIdx - 1))) == 0L) || activePieces[leftDownIdx]->color != turn))
                {
                    legalOptions.push_back(std::pair<short, short>(currPosition, leftDownIdx));
                    leftCol--;
                    downRow--;
                }
                if (currRow < 8) {
                    for (short up = currPosition + 8; up < 64 && (((bitBoard & (1L << up - 1)) == 0L) || activePieces[up]->color != turn); up += 8)
                    {
                        legalOptions.push_back(std::pair<short, short>(currPosition, up));
                        if ((bitBoard & (1L << up)) != 0) {
                            break;
                        }
                    }
                }
                if (currRow > 1) {
                    for (short down = currPosition - 8; down >= 0 && (((bitBoard & (1L << down - 1)) == 0L) || activePieces[down]->color != turn); down -= 8)
                    {
                        legalOptions.push_back(std::pair<short, short>(currPosition, down));
                        if ((bitBoard & (1L << down)) != 0) {
                            break;
                        }
                    }
                }
                if (currCol < 8) {
                    for (short right = currPosition + 1; (right % 8) != 0 && (((bitBoard & (1L << right - 1)) == 0L) || activePieces[right]->color != turn); right++)
                    {
                        legalOptions.push_back(std::pair<short, short>(currPosition, right));
                        if ((bitBoard & (1L << right)) != 0) {
                            break;
                        }
                    }
                }
                if (currCol > 1) {
                    for (short left = currPosition - 1; (left % 8 != 7) && (((bitBoard & (1L << left - 1)) == 0L) || activePieces[left]->color != turn); left--)
                    {
                        legalOptions.push_back(std::pair<short, short>(currPosition, left));
                        if ((bitBoard & (1L << left)) != 0) {
                            break;
                        }
                    }
                }
                break;
            }
            case KING:
            {
                // if (currRow < 8 && currCol < 8)
                // {
                //     legalOptions.push_back(std::pair<short, short>(currPosition, currPosition + 9));
                //     legalOptions.push_back(std::pair<short, short>(currPosition, currPosition + 8));
                //     legalOptions.push_back(std::pair<short, short>(currPosition, currPosition + 1));
                // }
                // else if (currCol < 8)
                // {
                //     legalOptions.push_back(std::pair<short, short>(currPosition, currPosition + 1));
                // }
                // else if (currRow < 8)
                // {
                //     legalOptions.push_back(std::pair<short, short>(currPosition, currPosition + 8));
                // }
                // if (currRow > 0 && currCol > 0)
                // {
                //     legalOptions.push_back(std::pair<short, short>(currPosition, currPosition - 9));
                //     legalOptions.push_back(std::pair<short, short>(currPosition, currPosition - 8));
                //     legalOptions.push_back(std::pair<short, short>(currPosition, currPosition - 1));
                // }
                // else if (currRow > 0)
                // {
                //     legalOptions.push_back(std::pair<short, short>(currPosition, currPosition - 8));
                // }
                // else if (currCol > 0)
                // {
                //     legalOptions.push_back(std::pair<short, short>(currPosition, currPosition - 1));
                // }
                // if (currRow > 0 && currCol < 8)
                // {
                //     legalOptions.push_back(std::pair<short, short>(currPosition, currPosition - 7));
                // }
                // if (currRow < 8 && currCol > 0)
                // {
                //     legalOptions.push_back(std::pair<short, short>(currPosition, currPosition + 7));
                // }
                break;
            }
            };
        }
    }
    return legalOptions;
}
