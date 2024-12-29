#ifndef CHESS_H
#define CHESS_H

#include <bit>
#include <cstdint>
#include <string>
#include <vector>

#define set_bit(b, i) (b |= (1ULL << (i)))
#define get_bit(b, i) (((b) >> (i)) & 1ULL)
#define clear_bit(b, i) (b &= ~(1ULL << (i)))

inline int pop_lsb(uint64_t &b) {
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

constexpr uint64_t up_left(uint64_t board) {
  return ((board & ~RANK_8 & ~FILE_A) << 7);
}
constexpr uint64_t up_right(uint64_t board) {
  return ((board & ~RANK_8 & ~FILE_H) << 9);
}
constexpr uint64_t down_left(uint64_t board) {
  return ((board & ~RANK_1 & ~FILE_A) >> 9);
}
constexpr uint64_t down_right(uint64_t board) {
  return ((board & ~RANK_1 & ~FILE_H) >> 7);
}

constexpr int RookHash(short idx, uint64_t empties, uint64_t opponent) {
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

constexpr int BishopHash(short idx, uint64_t empties, uint64_t opponent) {
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

  return (upLeftCount << 9) + (upRightCount << 6) + (downRightCount << 3) +
         downLeftCount;
}

enum Color : bool { WHITE = true, BLACK = false };

struct Move {
  int start, end;
  bool enPassant;
  enum Check {
    DOUBLE_CHECK,
    CHECK,
    NO_CHECK,
  } checkType;
  enum Piece {
    W_PAWN,
    W_KNIGHT,
    W_BISHOP,
    W_ROOK,
    W_QUEEN,
    W_KING,
    B_PAWN,
    B_KNIGHT,
    B_BISHOP,
    B_ROOK,
    B_QUEEN,
    B_KING,
    NONE // for captures
  } pieceType;
  Piece captureType;
  enum Promotion { QUEEN, ROOK, KNIGHT, BISHOP, NA } promotionType;
  Move(int s, int e, bool eP, Piece pT, Promotion prT)
      : start(s), end(e), enPassant(eP), checkType(NO_CHECK), pieceType(pT),
        captureType(NONE), promotionType(prT){};
};

struct BoardState {
  bool wCastle, wQueenCastle, bCastle, bQueenCastle;
  int enPassantIdx;
  short lastPawnOrTake;
  int fullTurns;
  std::vector<std::string> firstOccurrence, secondOccurrence;
  bool thirdOccurrence;
  BoardState(bool wC, bool wQC, bool bC, bool bQC, int ePI, short lPOT, int fT,
             std::vector<std::string> &fO, std::vector<std::string> &sO,
             bool tO)
      : wCastle(wC), wQueenCastle(wQC), bCastle(bC), bQueenCastle(bQC),
        enPassantIdx(ePI), lastPawnOrTake(lPOT), fullTurns(fT),
        firstOccurrence(fO), secondOccurrence(sO), thirdOccurrence(tO){};
};

struct MoveCategories {
  std::vector<Move> doubleChecks, checks, captures, etc;
};
class Chess {
protected:
  uint64_t wPawns, bPawns, wKnights, bKnights, wBishops, bBishops, wRooks,
      bRooks, wQueens, bQueens, wKing, bKing;
  Color turn;
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
  static Move::Promotion promotions[4];

  constexpr uint64_t whites() {
    return (wPawns | wKnights | wBishops | wRooks | wQueens | wKing);
  };
  constexpr uint64_t blacks() {
    return (bPawns | bKnights | bBishops | bRooks | bQueens | bKing);
  };
  constexpr uint64_t empties() { return ~(whites() | blacks()); };

public:
  Chess(const std::string &fenString);
  Chess(const Chess &x);
  static void Initialize();
  std::string BoardIdx();
  std::string ConvertToFEN();
  MoveCategories PseudoLegalMoves(const Move::Check checkStatus, bool tracking);
  Move::Check InChecks(const Color kingColor, const uint64_t kingBoard);
  void MakeMove(Move &m, bool tracking);
  void UnMakeMove(const Move &m, const BoardState &bs, bool tracking);
  uint64_t perft(int depth, Move::Check checkType);
};

#endif