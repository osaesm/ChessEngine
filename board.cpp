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
    for (short i = 8; i < 16; i++) {
        activePieces[i] = new Piece(WHITE, PAWN);
    }

    // Initialize black pawns
    for (short i = 48; i < 56; i++) {
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
    for (auto piece : activePieces) {
        delete piece.second;
        activePieces.erase(piece.first);
    }
}

TURN_COLORS Board::Start()
{
    for (auto p : LegalMoves()) {
        std::cout << p.first << " to " << p.second << std::endl;
    }
    return WHITE;
}

std::vector<std::pair<short, short>> Board::LegalMoves() {
    std::vector<std::pair<short, short>> legalOptions;
    for (auto currPiece : activePieces) {
        auto currPosition = currPiece.first;
        auto currColor = currPiece.second->color;
        auto currType = currPiece.second->pieceType;

        short currRow = currPosition / 8 + 1;
        short currCol = currPosition + 8 - 8*currRow + 1;
        if (currColor == turn) {
            switch (currType)
            {
            case PAWN:
                if (turn == WHITE) {
                    bool blocked = (bitBoard & (1L << (currPosition+8))) != 0;
                    // Check if pawn can move 2 up
                    if (currRow == 2 && !blocked && (bitBoard & (1L << (currPosition+16)) != 0L)) {
                        legalOptions.push_back(std::pair<short, short>{currPosition, currPosition+8});
                        legalOptions.push_back(std::pair<short, short>{currPosition, currPosition+16});
                    } else if (currRow < 7 && !blocked) {
                        // If not 2 spaces, check for 1
                        legalOptions.push_back(std::pair<short, short>{currPosition, currPosition+8});
                    }
                    // Check left diagonal take
                    if (currCol > 1 && (bitBoard & (1L << (currPosition+7))) != 0L) {
                        legalOptions.push_back(std::pair<short, short>(currPosition, currPosition+7));
                    }
                    // Check right diagonal take
                    if (currCol < 8 && (bitBoard & (1L << (currPosition+9))) != 0L) {
                        legalOptions.push_back(std::pair<short, short>(currPosition, currPosition+9));
                    }
                }
                break;
            case KNIGHT:
                if (currRow > 2 && currCol > 1 && currPosition >= 17 && ((bitBoard & (1L << (currPosition - 17))) == 0 || activePieces[currPosition-17]->color != currColor)) {
                    legalOptions.push_back(std::pair<short, short>(currPosition, currPosition-17));
                }
                if (currRow > 1 && currCol > 2 && currPosition >= 10 && ((bitBoard & (1L << (currPosition - 10))) == 0 || activePieces[currPosition-10]->color != currColor)) {
                    legalOptions.push_back(std::pair<short, short>(currPosition, currPosition-10));
                }
                if (currRow < 8 && currCol > 2 && currPosition <= 57 &&((bitBoard & (1L << (currPosition + 6))) == 0 || activePieces[currPosition+6]->color != currColor)) {
                    legalOptions.push_back(std::pair<short, short>(currPosition, currPosition+6));
                }
                if (currRow < 7 && currCol > 1 && currPosition <= 48 && ((bitBoard & (1L << (currPosition + 15))) == 0 || activePieces[currPosition+15]->color != currColor)) {
                    legalOptions.push_back(std::pair<short, short>(currPosition, currPosition+15));
                }
                if (currRow < 7 && currCol < 8  && currPosition <= 46 && ((bitBoard & (1L << (currPosition + 17))) == 0 || activePieces[currPosition+17]->color != currColor)) {
                    legalOptions.push_back(std::pair<short, short>(currPosition, currPosition+17));
                }
                if (currRow < 8 && currCol < 7  && currPosition <= 53 && ((bitBoard & (1L << (currPosition + 10))) == 0 || activePieces[currPosition+10]->color != currColor)) {
                    legalOptions.push_back(std::pair<short, short>(currPosition, currPosition+10));
                }
                if (currRow > 1 && currCol < 7  && currPosition >= 6 && ((bitBoard & (1L << (currPosition - 6))) == 0 || activePieces[currPosition-6]->color != currColor)) {
                    legalOptions.push_back(std::pair<short, short>(currPosition, currPosition-6));
                }
                if (currRow > 2 && currCol < 8  && currPosition >= 15 && ((bitBoard & (1L << (currPosition - 15))) == 0 || activePieces[currPosition-15]->color != currColor)) {
                    legalOptions.push_back(std::pair<short, short>(currPosition, currPosition-15));
                }
                break;
            case BISHOP:
                break;
            case ROOK:
                break;
            case QUEEN:
                break;
            case KING:
                break;
            };
        }
    }
    return legalOptions;
}
