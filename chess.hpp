#ifndef CHESS_H
#define CHESS_H

#include <bit>
#include <bitset>
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

// #include "extern/unordered_dense/include/ankerl/unordered_dense.h"

#define set_bit(b, i) ((b) |= (1ULL << i))
#define get_bit(b, i) ((b >> i) & (1ULL))
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

constexpr uint64_t up(uint64_t board) { return ((board & ~RANK_8) << 8); }
constexpr uint64_t down(uint64_t board) { return ((board & ~RANK_1) >> 8); }
constexpr uint64_t left(uint64_t board) { return ((board & ~FILE_A) >> 1); }
constexpr uint64_t right(uint64_t board) { return ((board & ~FILE_H) << 1); }

constexpr uint64_t up_left(uint64_t board) { return ((board & ~RANK_8 & ~FILE_A) << 7); }
constexpr uint64_t up_right(uint64_t board) { return ((board & ~RANK_8 & ~FILE_H) << 9); }
constexpr uint64_t down_left(uint64_t board) { return ((board & ~RANK_1 & ~FILE_A) >> 9); }
constexpr uint64_t down_right(uint64_t board) { return ((board & ~RANK_1 & ~FILE_H) >> 7); }

constexpr int RookHash(short idx, uint64_t empties, uint64_t opponent)
{
  uint64_t leftMask = left(1ULL << idx);
  int leftCount = 1;
  while (leftMask & empties) {
    leftMask = left(leftMask);
    ++leftCount;
  }
  if (!(leftMask & opponent)) {
    --leftCount;
  }

  uint64_t upMask = up(1ULL << idx);
  int upCount = 1;
  while (upMask & empties) {
    upMask = up(upMask);
    ++upCount;
  }
  if (!(upMask & opponent)) {
    --upCount;
  }

  uint64_t rightMask = right(1ULL << idx);
  int rightCount = 1;
  while (rightMask & empties) {
    rightMask = right(rightMask);
    ++rightCount;
  }
  if (!(rightMask & opponent)) {
    --rightCount;
  }

  uint64_t downMask = down(1ULL << idx);
  int downCount = 1;
  while (downMask & empties) {
    downMask = down(downMask);
    ++downCount;
  }
  if (!(downMask & opponent)) {
    --downCount;
  }

  return (leftCount << 9) + (upCount << 6) + (rightCount << 3) + downCount;
}

constexpr int BishopHash(short idx, uint64_t empties, uint64_t opponent)
{
  uint64_t upLeftMask = up_left(1ULL << idx);
  int upLeftCount = 1;
  while (upLeftMask & empties) {
    upLeftMask = up_left(upLeftMask);
    ++upLeftCount;
  }
  if (!(upLeftMask & opponent)) {
    --upLeftCount;
  }

  uint64_t upRightMask = up_right(1ULL << idx);
  int upRightCount = 1;
  while (upRightMask & empties) {
    upRightMask = up_right(upRightMask);
    ++upRightCount;
  }
  if (!(upRightMask & opponent)) {
    --upRightCount;
  }

  uint64_t downRightMask = down_right(1ULL << idx);
  int downRightCount = 1;
  while (downRightMask & empties) {
    downRightMask = down_right(downRightMask);
    ++downRightCount;
  }
  if (!(downRightMask & opponent)) {
    --downRightCount;
  }

  uint64_t downLeftMask = down_left(1ULL << idx);
  int downLeftCount = 1;
  while (downLeftMask & empties) {
    downLeftMask = down_left(downLeftMask);
    ++downLeftCount;
  }
  if (!(downLeftMask & opponent)) {
    --downLeftCount;
  }

  return (upLeftCount << 9) + (upRightCount << 6) + (downRightCount << 3) + downLeftCount;
}

constexpr int QueenHash(short idx, uint64_t empties, uint64_t opponent)
{
  int straightHash = RookHash(idx, empties, opponent);
  int diagonalHash = BishopHash(idx, empties, opponent);
  int queenHash = diagonalHash % 8; // downLeftCount
  queenHash += ((straightHash % 8) << 3); // downCount
  diagonalHash /= 8;
  straightHash /= 8;
  queenHash += ((diagonalHash % 8) << 6); // downRightCount
  queenHash += ((straightHash % 8) << 9); // rightCount
  diagonalHash /= 8;
  straightHash /= 8;
  queenHash += ((diagonalHash % 8) << 12); // upRightCount
  queenHash += ((straightHash % 8) << 15); // upCount
  diagonalHash /= 8;
  straightHash /= 8;
  queenHash += ((diagonalHash % 8) << 18); // upLeftCount
  queenHash += ((straightHash % 8) << 21); // leftCount
  return queenHash;
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
  static uint64_t ROOK_MOVES[64][4096];
  static uint64_t BISHOP_MOVES[64][4096];
  static uint64_t QUEEN_MOVES[64][4096*4096];
  // static ankerl::unordered_dense::map<int, uint64_t> ROOK_MOVES[64];
  // static ankerl::unordered_dense::map<int, uint64_t> BISHOP_MOVES[64];
  // static ankerl::unordered_dense::map<int, uint64_t> QUEEN_MOVES[64];
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