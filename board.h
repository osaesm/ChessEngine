#ifndef BOARD_H
#include <unordered_map>
#include <vector>
enum TURN_COLORS{WHITE, BLACK};
enum PIECES{PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING};

struct Piece
{
    TURN_COLORS color;
    PIECES pieceType;
    // short position;
    Piece(TURN_COLORS c, PIECES p, short x) {
        color = c;
        pieceType = p;
        // position = x;
    }
};

class Board
{
private:
    TURN_COLORS turn;
    bool whiteCastle;
    bool whiteLongCastle;
    bool blackCastle;
    bool blackLongCastle;
    // A1 = 0, A2 = 1, ... H8 = 63.
    unsigned long bitBoard = 0b1111111111111111000000000000000000000000000000001111111111111111;
    std::unordered_map<short, Piece*> activePieces;
    std::vector<std::pair<short, short>> LegalMoves();
public:
    Board();
    ~Board();
    TURN_COLORS Start();
};
#define BOARD_H
#endif
