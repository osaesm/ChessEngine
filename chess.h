#ifndef CHESS_H
#define CHESS_H

#include <format>
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

struct PerftResults
{
  long nodes;
  long captures;
  long enPassants;
  long castles;
  long promotions;
  long checks;

  PerftResults(long nodes = 0, long captures = 0, long enPassants = 0, long castles = 0, long promotions = 0, long checks = 0)
      : nodes(nodes), captures(captures), enPassants(enPassants), castles(castles), promotions(promotions), checks(checks)
  {
  }
  PerftResults operator+(const PerftResults &x) const
  {
    return PerftResults(nodes + x.nodes, captures + x.captures, enPassants + x.enPassants, castles + x.castles, promotions + x.promotions, checks + x.checks);
  }
  void operator+=(const PerftResults &x) {
    this->nodes += x.nodes;
    this->captures += x.captures;
    this->enPassants += x.enPassants;
    this->castles += x.castles;
    this->promotions += x.promotions;
    this->checks += x.checks;
  }
  bool operator==(const PerftResults &x) const {
    return (nodes == x.nodes
            && captures == x.captures
            && enPassants == x.enPassants
            && castles == x.castles
            && promotions == x.promotions
            && checks == x.checks);
  }
  std::string toString() const {
    return std::format("{{\n\tnodes:\t\t{},\n\tcaptures:\t{},\n\tenPassants:\t{},\n\tcastles:\t{},\n\tpromotions:\t{},\n\tchecks: {}\n}}", 
                        std::to_string(nodes),
                        std::to_string(captures),
                        std::to_string(enPassants),
                        std::to_string(castles),
                        std::to_string(promotions),
                        std::to_string(checks));
  }
};

class Chess
{
private:
  Piece *pieces[64];
  bool wCastle, wQueenCastle, bCastle, bQueenCastle;
  Piece::Color turn;
  short enPassantIdx;
  short lastPawnOrTake;
  int fullTurns;
  std::unordered_map<std::string, short> occurrences;

  Chess *MovePiece(const short start, const short end, const bool updateOccurrences);
  Chess *UpgradePawn(const short start, const short end, const Piece::Type option);

public:
  Chess(const std::string &fenString);
  Chess(const Chess &originalGame);
  std::string BoardIdx();
  std::string ConvertToFEN();
  bool InCheck(const short kingIdx, const Piece::Color kingColor);
  PerftResults Perft(const int depth);
  std::vector<Chess *> PseudolegalMoves();
  std::vector<Chess *> LegalMoves();
  void PrintBoard();
  void ClearPieces();
};

#endif