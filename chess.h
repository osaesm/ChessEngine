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
public:
    Chess(const std::string & fenString);
    Chess(const Chess & originalGame);
    Chess(Piece* newBoard[64], Piece::Color currTurn, bool wCastle, bool wQueenCastle, bool bCastle, bool bQueenCastle, short enPassantIdx, short lastPawnOrTake, int fullMoves, std::unordered_map<std::string, short> newOccurrences);
    std::vector<Chess*> LegalMoves();
    std::string BoardIdx();
    std::string ConvertToFEN();
    void PrintBoard();
};

#endif