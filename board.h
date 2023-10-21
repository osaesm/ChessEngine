#ifndef BOARD_H
#include <unordered_map>
#include <vector>
enum TURN_COLORS{WHITE, BLACK};
enum PIECES{PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING};

#define isEmpty(x) ((bitBoard & (1L << (x))) == 0L)
struct Piece
{
    TURN_COLORS color;
    PIECES pieceType;
    Piece(TURN_COLORS c, PIECES p) {
        color = c;
        pieceType = p;
    }
};

class Board
{
protected:
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
    void EditBoard(const std::optional<TURN_COLORS>& newTurn,
                          const std::optional<bool>& canWhiteCastle,
                          const std::optional<bool>& canWhiteLongCastle,
                          const std::optional<bool>& canBlackCastle,
                          const std::optional<bool>& canBlackLongCastle,
                          const std::optional<unsigned long>& newBitBoard,
                          const std::optional<std::unordered_map<short, Piece *>>& newActivePieces);
};
#define BOARD_H
#endif
