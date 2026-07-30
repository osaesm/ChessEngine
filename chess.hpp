// chess.hpp
#ifndef CHESS_H
#define CHESS_H

#include <bit>
#include <cstddef>
#include <cstdint>
#include <map>
#include <string>
#include <unordered_map>
#include <vector>

#include "thread_pool.hpp"

// ------------------------------------------------------------------
// Refactored bit operations: macros replaced with constexpr functions
// ------------------------------------------------------------------
inline constexpr void set_bit(uint64_t &b, int i) noexcept { b |= (1ULL << i); }

inline constexpr bool get_bit(uint64_t b, int i) noexcept {
  return (b >> i) & 1ULL;
}

inline constexpr void clear_bit(uint64_t &b, int i) noexcept {
  b &= ~(1ULL << i);
}

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

enum class Color { WHITE, BLACK };

// ------------------------------------------------------------------
// Move representation: packed into a single 32‑bit integer (using bit‑fields)
// ------------------------------------------------------------------
namespace Move {
enum Check { DOUBLE_CHECK, CHECK, NO_CHECK };
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
  NONE
};
enum Promotion { QUEEN, ROOK, KNIGHT, BISHOP, NA };
} // namespace Move

union PackedMove {
  uint32_t raw;
  struct {
    uint32_t start : 6;
    uint32_t end : 6;
    uint32_t piece : 4; // Move::Piece
    uint32_t capture : 4;
    uint32_t promo : 3; // Move::Promotion
    uint32_t enPassant : 1;
    uint32_t check : 2; // Move::Check
  };
};

static inline PackedMove
makePackedMove(int start, int end, bool enPassant, Move::Piece piece,
               Move::Promotion promo, Move::Piece capture, Move::Check check) {
  PackedMove m;
  m.start = start;
  m.end = end;
  m.piece = static_cast<uint32_t>(piece);
  m.capture = static_cast<uint32_t>(capture);
  m.promo = static_cast<uint32_t>(promo);
  m.enPassant = enPassant ? 1u : 0u;
  m.check = static_cast<uint32_t>(check);
  return m;
}

// ------------------------------------------------------------------
// Random number generators for Zobrist keys
// ------------------------------------------------------------------
inline uint64_t splitmix64(uint64_t &state) {
  state += 0x9e3779b97f4a7c15ULL;
  uint64_t z = state;
  z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9ULL;
  z = (z ^ (z >> 27)) * 0x94d049bb133111ebULL;
  return z ^ (z >> 31);
}

inline uint64_t xoshiro256pp(uint64_t s[4]) {
  const uint64_t result = std::rotl(s[0] + s[3], 23) + s[0];
  const uint64_t t = s[1] << 17;
  s[2] ^= s[0];
  s[3] ^= s[1];
  s[1] ^= s[2];
  s[0] ^= s[3];
  s[2] ^= t;
  s[3] = std::rotl(s[3], 45);
  return result;
}

// Thread-local Perft cache
class PerftCache {
private:
  std::unordered_map<uint64_t, std::map<int, uint64_t>> perftResults;

public:
  void insert(uint64_t key, int depth, uint64_t val) {
    perftResults[key][depth] = val;
  }

  bool get(uint64_t key, int depth, uint64_t &val) {
    auto it = perftResults.find(key);
    if (it != perftResults.end()) {
      auto it2 = it->second.find(depth);
      if (it2 != it->second.end()) {
        val = it2->second;
        return true;
      }
    }
    return false;
  }
};

// ------------------------------------------------------------------
// Repetition tracker – thread‑local, not copied with Chess
// ------------------------------------------------------------------
struct RepetitionTracker {
  std::unordered_map<uint64_t, std::pair<int, int>> occurrences;
  std::vector<std::tuple<uint64_t, int, int>> undoStack;
  int moveCounter = 0;
  int repetitionStart = 0;

  void push(uint64_t hash, bool irreversible) {
    ++moveCounter;
    if (irreversible) {
      repetitionStart = moveCounter;
    }
    auto &entry = occurrences[hash];
    int old1 = entry.first;
    int old2 = entry.second;
    undoStack.emplace_back(hash, old1, old2);
    entry.second = old1;
    entry.first = moveCounter;
  }

  void pop() {
    auto [hash, old1, old2] = undoStack.back();
    undoStack.pop_back();
    auto it = occurrences.find(hash);
    if (it != occurrences.end()) {
      it->second = {old1, old2};
      if (old1 == 0 && old2 == 0) {
        occurrences.erase(it);
      }
    }
  }

  bool isRepetition(uint64_t hash) const {
    auto it = occurrences.find(hash);
    if (it == occurrences.end()) {
      return false;
    }
    return it->second.second >= repetitionStart;
  }

  void reset() {
    occurrences.clear();
    undoStack.clear();
    moveCounter = 0;
    repetitionStart = 0;
  }
};

struct BoardState {
  bool wCastle, wQueenCastle, bCastle, bQueenCastle;
  int enPassantIdx;
  short lastPawnOrTake;
  int fullTurns;
  uint64_t hash;
  int moveCounter;
  int repetitionStart;
  BoardState(bool wC, bool wQC, bool bC, bool bQC, int ePI, short lPOT, int fT,
             uint64_t h, int mc, int rs)
      : wCastle(wC), wQueenCastle(wQC), bCastle(bC), bQueenCastle(bQC),
        enPassantIdx(ePI), lastPawnOrTake(lPOT), fullTurns(fT), hash(h),
        moveCounter(mc), repetitionStart(rs) {}
};

struct MoveCategories {
  std::vector<PackedMove> doubleChecks, checks, captures, etc;
  size_t numMoves() const {
    return doubleChecks.size() + checks.size() + captures.size() + etc.size();
  }
};

void Add(MoveCategories &mC, PackedMove m); // forward declaration

class Chess {
protected:
  uint64_t wPawns, bPawns, wKnights, bKnights, wBishops, bBishops, wRooks,
      bRooks, wQueens, bQueens, wKing, bKing;
  Color turn;
  bool wCastle, wQueenCastle, bCastle, bQueenCastle;
  int enPassantIdx;
  short lastPawnOrTake;
  int fullTurns;
  uint64_t hash;

  static uint64_t PAWN_TAKES[64][2];
  static uint64_t KNIGHT_MOVES[64];
  static uint64_t KING_MOVES[64];
  static uint64_t ROOK_MOVES[64][4096];
  static uint64_t BISHOP_MOVES[64][4096];
  static Move::Promotion promotions[4];

  static uint64_t zobristPiece[64][12];
  static uint64_t zobristEnPassant[8];
  static uint64_t zobristCastle[4];
  static uint64_t zobristBlackToMove;

  constexpr uint64_t whites() const {
    return (wPawns | wKnights | wBishops | wRooks | wQueens | wKing);
  }
  constexpr uint64_t blacks() const {
    return (bPawns | bKnights | bBishops | bRooks | bQueens | bKing);
  }
  constexpr uint64_t empties() const { return ~(whites() | blacks()); }

public:
  Chess(const std::string &fenString);
  Chess(const Chess &x);
  static void Initialize();
  const std::string BoardIdx();
  const std::string ConvertToFEN();
  MoveCategories PseudoLegalMoves(const Move::Check checkStatus);
  const Move::Check InChecks(const Color kingColor,
                             const uint64_t kingBoard) const;
  void MakeMove(PackedMove m, const bool tracking);
  void UnMakeMove(PackedMove m, const BoardState &bs, const bool tracking);
  uint64_t perft(int depth, Move::Check checkType);
  uint64_t perftRecurse(int depth, Move::Check checkType);

  bool isRepetition() const;
  static ThreadPool &getThreadPool();

private:
  // Compute check after a move given its raw components (does not pack)
  Move::Check computeCheckAfterMove(int start, int end, bool enPassant,
                                    Move::Piece piece, Move::Promotion promo,
                                    Move::Piece capture) const;
  static void InitializeZobrist();

  template <Color C>
  void generatePseudoLegalMoves(const Move::Check checkStatus,
                                MoveCategories &moves) const;
};

#endif
