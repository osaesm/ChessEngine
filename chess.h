#ifndef CHESS_H
#define CHESS_H

#include <string>
#include <unordered_map>
#include <vector>

class Piece
{
public:
    enum Color : bool
    {
        WHITE = true,
        BLACK = false
    } color;
    enum Type
    {
        PAWN,
        KNIGHT,
        BISHOP,
        ROOK,
        QUEEN,
        KING
    } type;
};

class Chess
{
private:
    Piece* pieces[64];
    bool wCastle, wQueenCastle, bCastle, bQueenCastle;
    Piece::Color turn;
    short enPassantIdx;
    short lastPawnOrTake;
    int fullTurns;
    std::unordered_map<std::string, short> occurrences;

    Chess* MovePiece(const short start, const short end, const bool updateOccurrences);
    Chess* UpgradePawn(const short start, const short end, const Piece::Type option);
public:
    Chess(const std::string & fenString);
    Chess(const Chess & originalGame);
    std::string BoardIdx();
    std::string ConvertToFEN();
    bool InCheck(const short kingIdx, const Piece::Color kingColor);
    int Perft(const int depth);
    std::vector<Chess*> PseudolegalMoves();
    std::vector<Chess*> LegalMoves();
    void PrintBoard();
    void ClearPieces();
};

#endif