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
    activePieces[0] = new Piece(WHITE, ROOK, 0);
    activePieces[1] = new Piece(WHITE, KNIGHT, 1);
    activePieces[2] = new Piece(WHITE, BISHOP, 2);    
    activePieces[3] = new Piece(WHITE, QUEEN, 3);
    activePieces[4] = new Piece(WHITE, KING, 4);
    activePieces[5] = new Piece(WHITE, BISHOP, 5);
    activePieces[6] = new Piece(WHITE, KNIGHT, 6);
    activePieces[7] = new Piece(WHITE, ROOK, 7);

    // Initialize white pawns
    for (short i = 8; i < 16; i++) {
        activePieces[i] = new Piece(WHITE, PAWN, i);
    }

    // Initialize black pawns
    for (short i = 48; i < 56; i++) {
        activePieces[i] = new Piece(BLACK, PAWN, i);
    }

    // Initialize black second row
    activePieces[56] = new Piece(BLACK, ROOK, 56);
    activePieces[57] = new Piece(BLACK, KNIGHT, 57);
    activePieces[58] = new Piece(BLACK, BISHOP, 58);    
    activePieces[59] = new Piece(BLACK, QUEEN, 59);
    activePieces[60] = new Piece(BLACK, KING, 60);
    activePieces[61] = new Piece(BLACK, BISHOP, 61);
    activePieces[62] = new Piece(BLACK, KNIGHT, 62);
    activePieces[63] = new Piece(BLACK, ROOK, 63);

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
        if (turn == WHITE && currColor == WHITE) {
            if (currType == PAWN && currPosition <= 16) {
                short currRow = currPosition / 8 + 1;
                short currCol = currPosition + 8 - 8*currRow + 1;
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
        } else if (turn == BLACK && currColor == BLACK) {
            // if (currType == PAWN && currPosition <= 16) {
            //     short currRow = currPosition / 8 + 1;
            //     short currCol = currPosition + 8 - 8*currRow + 1;
            //     bool blocked = (bitBoard & (1L << (currPosition+8))) != 0;
            //     // Check if pawn can move 2 up
            //     if (currRow == 2 && !blocked && (bitBoard & (1L << (currPosition+16)) != 0L)) {
            //         legalOptions.push_back(std::pair<short, short>{currPosition, currPosition+8});
            //         legalOptions.push_back(std::pair<short, short>{currPosition, currPosition+16});
            //     } else if (currRow < 7 && !blocked) {
            //         // If not 2 spaces, check for 1
            //         legalOptions.push_back(std::pair<short, short>{currPosition, currPosition+8});
            //     }
            //     // Check left diagonal take
            //     if (currCol > 1 && (bitBoard & (1L << (currPosition+7))) != 0L) {
            //         legalOptions.push_back(std::pair<short, short>(currPosition, currPosition+7));
            //     }
            //     // Check right diagonal take
            //     if (currCol < 8 && (bitBoard & (1L << (currPosition+9))) != 0L) {
            //         legalOptions.push_back(std::pair<short, short>(currPosition, currPosition+9));
            //     }
        }

    }
    return legalOptions;
}
