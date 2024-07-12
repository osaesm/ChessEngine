#ifndef CHESS_H
#define CHESS_H

#include <cstdint>
#include <string>
#include <vector>

#include "extern/unordered_dense/include/ankerl/unordered_dense.h"

#define set_bit(b, i) ((b) |= (1ULL << i))
#define get_bit(b, i) ((b) & (1ULL << i))
#define clear_bit(b, i) ((b) &= ~(1ULL << i))
#define get_lsb(b) (__builtin_ctzll(b))

inline int pop_lsb(uint64_t &b)
{
  int i = get_lsb(b);
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

constexpr uint64_t upleft(uint64_t board) { return (board & ~RANK_8 & ~FILE_A) << 7; }
constexpr uint64_t upright(uint64_t board) { return (board & ~RANK_8 & ~FILE_H) << 9; }
constexpr uint64_t downleft(uint64_t board) { return (board & ~RANK_1 & ~FILE_A) >> 9; }
constexpr uint64_t downright(uint64_t board) { return (board & ~RANK_1 & ~FILE_H) >> 7; }

constexpr int RookHash(short idx, uint64_t blockers)
{
  int leftSide = 0;
  while (leftSide < 8 && ~(get_bit(blockers, idx - leftSide)))
  {
    ++leftSide;
  }
  leftSide--;
  int rightSide = 0;
  while (rightSide < 8 && ~(get_bit(blockers, idx + rightSide)))
  {
    ++rightSide;
  }
  rightSide--;
  int upSide = 0;
  while (upSide < 8 && ~(get_bit(blockers, idx + (upSide * 8))))
  {
    ++upSide;
  }
  upSide--;
  int downSide = 0;
  while (downSide < 8 && ~(get_bit(blockers, idx - (downSide * 8))))
  {
    ++downSide;
  }
  downSide--;
  return (leftSide << 9) + (upSide << 6) + (rightSide << 3) + downSide;
}

constexpr int BishopHash(short idx, uint64_t blockers)
{
  int upLeftSide = 0;
  while (upLeftSide < 8 && ~(get_bit(blockers, 7 * (idx + upLeftSide))))
  {
    ++upLeftSide;
  }
  upLeftSide--;
  int upRightSide = 0;
  while (upRightSide < 8 && ~(get_bit(blockers, 9 * (idx + upRightSide))))
  {
    ++upRightSide;
  }
  upRightSide--;
  int downRightSide = 0;
  while (downRightSide < 8 && ~(get_bit(blockers, 7 * (idx - downRightSide))))
  {
    ++downRightSide;
  }
  downRightSide--;
  int downLeftSide = 0;
  while (downLeftSide < 8 && ~(get_bit(blockers, 9 * (idx - downLeftSide))))
  {
    ++downLeftSide;
  }
  downLeftSide--;
  return (upLeftSide << 9) + (upRightSide << 6) + (downRightSide << 3) + downLeftSide;
}

constexpr int QueenHash(short idx, uint64_t blockers)
{
  int leftSide = 0;
  while (leftSide < 8 && ~(get_bit(blockers, idx - leftSide)))
  {
    ++leftSide;
  }
  leftSide--;
  int rightSide = 0;
  while (rightSide < 8 && ~(get_bit(blockers, idx + rightSide)))
  {
    ++rightSide;
  }
  rightSide--;
  int upSide = 0;
  while (upSide < 8 && ~(get_bit(blockers, idx + (upSide * 8))))
  {
    ++upSide;
  }
  upSide--;
  int downSide = 0;
  while (downSide < 8 && ~(get_bit(blockers, idx - (downSide * 8))))
  {
    ++downSide;
  }
  downSide--;
  int upLeftSide = 0;
  while (upLeftSide < 8 && ~(get_bit(blockers, 7 * (idx + upLeftSide))))
  {
    ++upLeftSide;
  }
  upLeftSide--;
  int upRightSide = 0;
  while (upRightSide < 8 && ~(get_bit(blockers, 9 * (idx + upRightSide))))
  {
    ++upRightSide;
  }
  upRightSide--;
  int downRightSide = 0;
  while (downRightSide < 8 && ~(get_bit(blockers, 7 * (idx - downRightSide))))
  {
    ++downRightSide;
  }
  downRightSide--;
  int downLeftSide = 0;
  while (downLeftSide < 8 && ~(get_bit(blockers, 9 * (idx - downLeftSide))))
  {
    ++downLeftSide;
  }
  downLeftSide--;
  return (leftSide << 21) + (upLeftSide << 18) + (upSide << 15) + (upRightSide << 12) + (rightSide << 9) + (downRightSide << 6) + (downSide << 3) + downLeftSide;
}

class Chess
{
protected:
  // 0: white forward, 1: white takes, 2: black forward, 3: black takes
  static uint64_t PAWN_MOVES[64][4];
  static uint64_t KNIGHT_MOVES[64];
  static uint64_t KING_MOVES[64];
  static ankerl::unordered_dense::map<int, uint64_t> ROOK_MOVES[64];
  static ankerl::unordered_dense::map<int, uint64_t> BISHOP_MOVES[64];
  static ankerl::unordered_dense::map<int, uint64_t> QUEEN_MOVES[64];
  uint64_t wPawns, bPawns, wKnights, bKnights, wBishops, bBishops, wRooks, bRooks, wQueens, bQueens, wKing, bKing;
  enum Color : bool
  {
    WHITE = true,
    BLACK = false
  } turn;
  bool wCastle, wQueenCastle, bCastle, bQueenCastle;
  short enPassantIdx;
  short lastPawnOrTake;
  int fullTurns;
  std::vector<std::string> firstOccurrence;
  std::vector<std::string> secondOccurrence;
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
  void PromotePawn(const char pieceType, const int start, const int end);
  uint64_t perft(int depth);
  uint64_t perft2(int depth);
};

#endif