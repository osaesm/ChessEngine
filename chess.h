#ifndef CHESS_H
#define CHESS_H

#include <bit>
#include <bitset>
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

#include "extern/unordered_dense/include/ankerl/unordered_dense.h"

#define set_bit(b, i) ((b) |= (1ULL << i))
#define get_bit(b, i) ((b) & (1ULL << i))
#define clear_bit(b, i) ((b) &= ~(1ULL << i))

inline int pop_lsb(uint64_t &b)
{
  int i = std::countr_zero(b);
  b &= (b - 1);
  return i;
}

constexpr uint64_t FILE_H = 0x8080808080808080;
constexpr uint64_t FILE_G = 0x4040404040404040;
constexpr uint64_t FILE_F = 0x2020202020202020;
constexpr uint64_t FILE_E = 0x1010101010101010;
constexpr uint64_t FILE_D = 0x0808080808080808;
constexpr uint64_t FILE_C = 0x0404040404040404;
constexpr uint64_t FILE_B = 0x0202020202020202;
constexpr uint64_t FILE_A = 0x0101010101010101;

constexpr uint64_t RANK_8 = 0xFF00000000000000;
constexpr uint64_t RANK_7 = 0x00FF000000000000;
constexpr uint64_t RANK_6 = 0x0000FF0000000000;
constexpr uint64_t RANK_5 = 0x000000FF00000000;
constexpr uint64_t RANK_4 = 0x00000000FF000000;
constexpr uint64_t RANK_3 = 0x0000000000FF0000;
constexpr uint64_t RANK_2 = 0x000000000000FF00;
constexpr uint64_t RANK_1 = 0x00000000000000FF;

constexpr uint64_t up(uint64_t board) { return (board & ~RANK_8) << 8; }
constexpr uint64_t down(uint64_t board) { return (board & ~RANK_1) >> 8; }
constexpr uint64_t left(uint64_t board) { return (board & ~FILE_A) >> 1; }
constexpr uint64_t right(uint64_t board) { return (board & ~FILE_H) << 1; }

constexpr uint64_t up_left(uint64_t board) { return (board & ~RANK_8 & ~FILE_A) << 7; }
constexpr uint64_t up_right(uint64_t board) { return (board & ~RANK_8 & ~FILE_H) << 9; }
constexpr uint64_t down_left(uint64_t board) { return (board & ~RANK_1 & ~FILE_A) >> 9; }
constexpr uint64_t down_right(uint64_t board) { return (board & ~RANK_1 & ~FILE_H) >> 7; }

constexpr int RookHash(short idx, uint64_t empties, uint64_t opponent)
{
  int leftSide = 1;
  uint64_t leftMask = left(1ULL << idx);
  for (; leftMask & empties; ++leftSide, leftMask = left(leftMask))
  {
  }
  if (leftMask & opponent)
  {
  }
  else
  {
    leftSide--;
  }

  int upSide = 1;
  uint64_t upMask = up(1ULL << idx);
  for (; upMask & empties; ++upSide, upMask = up(upMask))
  {
  }
  if (upMask & opponent)
  {
  }
  else
  {
    upSide--;
  }

  int rightSide = 1;
  uint64_t rightMask = right(1ULL << idx);
  for (; rightMask & empties; ++rightSide, rightMask = right(rightMask))
  {
  }
  if (rightMask & opponent)
  {
  }
  else
  {
    rightSide--;
  }

  int downSide = 1;
  uint64_t downMask = down(1ULL << idx);
  for (; downMask & empties; ++downSide, downMask = down(downMask))
  {
  }
  if (downMask & opponent)
  {
  }
  else
  {
    downSide--;
  }
  return (leftSide << 9) + (upSide << 6) + (rightSide << 3) + downSide;
}

constexpr int BishopHash(short idx, uint64_t empties, uint64_t opponent)
{
  int upLeftSide = 1;
  uint64_t upLeftMask = up_left(1ULL << idx);
  for (; upLeftMask & empties; ++upLeftSide, upLeftMask = up_left(upLeftMask))
  {
  }
  if (upLeftMask & opponent)
  {
  }
  else
  {
    upLeftSide--;
  }

  int upRightSide = 1;
  uint64_t upRightMask = up_right(1ULL << idx);
  for (; upRightMask & empties; ++upRightSide, upRightMask = up_right(upRightMask))
  {
  }
  if (upRightMask & opponent)
  {
  }
  else
  {
    upRightSide--;
  }

  int downRightSide = 1;
  uint64_t downRightMask = down_right(1ULL << idx);
  for (; downRightMask & empties; ++downRightSide, downRightMask = down_right(downRightMask))
  {
  }
  if (downRightMask & opponent)
  {
  }
  else
  {
    downRightSide--;
  }

  int downLeftSide = 1;
  uint64_t downLeftMask = down_left(1ULL << idx);
  for (; downLeftMask & empties; ++downLeftSide, downLeftMask = down_left(downLeftMask))
  {
  }
  if (downLeftMask & opponent)
  {
  }
  else
  {
    downLeftSide--;
  }
  return (upLeftSide << 9) + (upRightSide << 6) + (downRightSide << 3) + downLeftSide;
}

constexpr int QueenHash(short idx, uint64_t empties, uint64_t opponent)
{
  int leftSide = 1;
  uint64_t leftMask = left(1ULL << idx);
  for (; leftMask & empties; ++leftSide, leftMask = left(leftMask))
  {
  }
  if (leftMask & opponent)
  {
  }
  else
  {
    leftSide--;
  }

  int upSide = 1;
  uint64_t upMask = up(1ULL << idx);
  for (; upMask & empties; ++upSide, upMask = up(upMask))
  {
  }
  if (upMask & opponent)
  {
  }
  else
  {
    upSide--;
  }

  int rightSide = 1;
  uint64_t rightMask = right(1ULL << idx);
  for (; rightMask & empties; ++rightSide, rightMask = right(rightMask))
  {
  }
  if (rightMask & opponent)
  {
  }
  else
  {
    rightSide--;
  }

  int downSide = 1;
  uint64_t downMask = down(1ULL << idx);
  for (; downMask & empties; ++downSide, downMask = down(downMask))
  {
  }
  if (downMask & opponent)
  {
  }
  else
  {
    downSide--;
  }

  int upLeftSide = 1;
  uint64_t upLeftMask = up_left(1ULL << idx);
  for (; upLeftMask & empties; ++upLeftSide, upLeftMask = up_left(upLeftMask))
  {
  }
  if (upLeftMask & opponent)
  {
  }
  else
  {
    upLeftSide--;
  }

  int upRightSide = 1;
  uint64_t upRightMask = up_right(1ULL << idx);
  for (; upRightMask & empties; ++upRightSide, upRightMask = up_right(upRightMask))
  {
  }
  if (upRightMask & opponent)
  {
  }
  else
  {
    upRightSide--;
  }

  int downRightSide = 1;
  uint64_t downRightMask = down_right(1ULL << idx);
  for (; downRightMask & empties; ++downRightSide, downRightMask = down_right(downRightMask))
  {
  }
  if (downRightMask & opponent)
  {
  }
  else
  {
    downRightSide--;
  }

  int downLeftSide = 1;
  uint64_t downLeftMask = down_left(1ULL << idx);
  for (; downLeftMask & empties; ++downLeftSide, downLeftMask = down_left(downLeftMask))
  {
  }
  if (downLeftMask & opponent)
  {
  }
  else
  {
    downLeftSide--;
  }

  return (leftSide << 21) + (upLeftSide << 18) + (upSide << 15) + (upRightSide << 12) + (rightSide << 9) + (downRightSide << 6) + (downSide << 3) + downLeftSide;
}

class Chess
{
protected:
  uint64_t wPawns, bPawns, wKnights, bKnights, wBishops, bBishops, wRooks, bRooks, wQueens, bQueens, wKing, bKing;
  enum Color : bool
  {
    WHITE = true,
    BLACK = false
  } turn;
  bool wCastle, wQueenCastle, bCastle, bQueenCastle;
  int enPassantIdx;
  short lastPawnOrTake;
  int fullTurns;
  std::vector<std::string> firstOccurrence;
  std::vector<std::string> secondOccurrence;
  bool thirdOccurrence;

  // 0: white takes, 1: black takes
  static uint64_t PAWN_TAKES[64][2];
  static uint64_t KNIGHT_MOVES[64];
  static uint64_t KING_MOVES[64];
  static ankerl::unordered_dense::map<int, uint64_t> ROOK_MOVES[64];
  static ankerl::unordered_dense::map<int, uint64_t> BISHOP_MOVES[64];
  static ankerl::unordered_dense::map<int, uint64_t> QUEEN_MOVES[64];
  static char promotions[4];

  constexpr uint64_t whites() { return (wPawns | wKnights | wBishops | wRooks | wQueens | wKing); };
  constexpr uint64_t blacks() { return (bPawns | bKnights | bBishops | bRooks | bQueens | bKing); };
  constexpr uint64_t empties() { return ~(whites() | blacks()); };

public:
  Chess(const std::string &fenString);
  Chess(const Chess &x);
  static void Initialize();
  std::string BoardIdx();
  std::string ConvertToFEN();
  std::vector<Chess *> LegalMoves();
  std::vector<Chess *> PseudoLegalMoves();
  bool InCheck(const Color kingColor, const uint64_t kingIdx);
  void MovePiece(const char pieceType, const int start, const int end, uint64_t opponent);
  void PromotePawn(const char pieceType, const int end);
  uint64_t perft(int depth);
};

#endif