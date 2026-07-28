#ifndef CHESS_H
#define CHESS_H

#include <atomic>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <map>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

// ------------------------------------------------------------------
// Refactored bit operations: macros replaced with constexpr functions
// ------------------------------------------------------------------
inline constexpr void set_bit(uint64_t &b, int i) noexcept {
    b |= (1ULL << i);
}

inline constexpr bool get_bit(uint64_t b, int i) noexcept {
    return (b >> i) & 1ULL;
}

inline constexpr void clear_bit(uint64_t &b, int i) noexcept {
    b &= ~(1ULL << i);
}

// ------------------------------------------------------------------

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

class PerftResultsThreaded {
private:
  std::unordered_map<std::string, std::map<int, uint64_t>> perftResults;
  mutable std::mutex mutex;

public:
  void insert(const std::string &key, const int depth, const uint64_t val) {
    std::lock_guard<std::mutex> lock(mutex);
    perftResults[key][depth] = val;
  }

  bool get(const std::string &key, const int depth, uint64_t &val) {
    std::lock_guard<std::mutex> lock(mutex);
    if (perftResults.contains(key) && perftResults[key].contains(depth)) {
      val = perftResults[key][depth];
      return true;
    }
    return false;
  }
};

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
  size_t numMoves() {
    return doubleChecks.size() + checks.size() + captures.size() + etc.size();
  };
};

void Add(MoveCategories &mC, Move &m);   // forward declaration

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
  const std::string BoardIdx();
  const std::string ConvertToFEN();
  MoveCategories PseudoLegalMoves(const Move::Check checkStatus,
                                  const bool tracking);
  const Move::Check InChecks(const Color kingColor, const uint64_t kingBoard);
  void MakeMove(Move &m, const bool tracking);
  void UnMakeMove(const Move &m, const BoardState &bs, const bool tracking);
  uint64_t perft(int depth, Move::Check checkType);
  static void perftWorker(Chess currGame, std::vector<Move> moves, int depth,
                          Move::Check checkType,
                          std::atomic<uint64_t> &totalNodes);
  uint64_t perftRecurse(int depth, Move::Check checkType);

private:
  // Colour‑aware helper to eliminate the previous 250‑line duplication.
template<Color C>
MoveCategories generatePseudoLegalMoves(const Move::Check checkStatus,
                                        bool tracking,
                                        const BoardState &bs) {
    MoveCategories moves;
    Chess gameCopy(*this);
    uint64_t currMoves = 0ULL;
    const uint64_t enPassantMask =
        (this->enPassantIdx == -1) ? 0ULL : (1ULL << this->enPassantIdx);

    // Colour‑dependent constants
    constexpr Color opponentColor = (C == Color::WHITE) ? Color::BLACK : Color::WHITE;
    constexpr int forwardStep = (C == Color::WHITE) ? 8 : -8;
    constexpr uint64_t startRank = (C == Color::WHITE) ? RANK_2 : RANK_7;
    constexpr uint64_t promoRank = (C == Color::WHITE) ? RANK_8 : RANK_1;

    // Piece enums
    constexpr Move::Piece piecePawn   = (C == Color::WHITE) ? Move::Piece::W_PAWN   : Move::Piece::B_PAWN;
    constexpr Move::Piece pieceKnight = (C == Color::WHITE) ? Move::Piece::W_KNIGHT : Move::Piece::B_KNIGHT;
    constexpr Move::Piece pieceBishop = (C == Color::WHITE) ? Move::Piece::W_BISHOP : Move::Piece::B_BISHOP;
    constexpr Move::Piece pieceRook   = (C == Color::WHITE) ? Move::Piece::W_ROOK   : Move::Piece::B_ROOK;
    constexpr Move::Piece pieceQueen  = (C == Color::WHITE) ? Move::Piece::W_QUEEN  : Move::Piece::B_QUEEN;
    constexpr Move::Piece pieceKing   = (C == Color::WHITE) ? Move::Piece::W_KING   : Move::Piece::B_KING;

    // References to our pieces in the copy (consumed by pop_lsb)
    uint64_t &myPawns   = (C == Color::WHITE) ? gameCopy.wPawns   : gameCopy.bPawns;
    uint64_t &myKnights = (C == Color::WHITE) ? gameCopy.wKnights : gameCopy.bKnights;
    uint64_t &myBishops = (C == Color::WHITE) ? gameCopy.wBishops : gameCopy.bBishops;
    uint64_t &myRooks   = (C == Color::WHITE) ? gameCopy.wRooks   : gameCopy.bRooks;
    uint64_t &myQueens  = (C == Color::WHITE) ? gameCopy.wQueens  : gameCopy.bQueens;
    uint64_t &myKing    = (C == Color::WHITE) ? gameCopy.wKing    : gameCopy.bKing;

    // Opponent / own bitboards (from the actual board, not the copy)
    uint64_t opponentBB = (C == Color::WHITE) ? this->blacks() : this->whites();
    uint64_t ownPieces  = (C == Color::WHITE) ? this->whites() : this->blacks();
    uint64_t emptiesBB  = this->empties();

    // Lambda helpers to choose direction
    auto forward = [](uint64_t b) constexpr {
        if constexpr (C == Color::WHITE) return up(b);
        else return down(b);
    };
    auto captureLeft = [](uint64_t b) constexpr {
        if constexpr (C == Color::WHITE) return up_left(b);
        else return down_left(b);
    };
    auto captureRight = [](uint64_t b) constexpr {
        if constexpr (C == Color::WHITE) return up_right(b);
        else return down_right(b);
    };

    if (checkStatus != Move::Check::DOUBLE_CHECK) {
        // ---------- Pawn moves ----------
        // Double push: intermediate and destination must be empty.
        // Original: up(up(wPawns & RANK_2) & empties()) & empties()
        currMoves = forward( forward(myPawns & startRank) & emptiesBB ) & emptiesBB;
        while (currMoves) {
            int endIdx = pop_lsb(currMoves);
            int startIdx = endIdx - 2 * forwardStep;
            Move m(startIdx, endIdx, false, piecePawn, Move::Promotion::NA);
            this->MakeMove(m, tracking);
            this->UnMakeMove(m, bs, tracking);
            Add(moves, m);
        }

        // Single push
        currMoves = forward(myPawns) & emptiesBB;
        if constexpr (C == Color::WHITE) {
            // Non‑promotions first
            while (currMoves & ~promoRank) {
                int endIdx = pop_lsb(currMoves);
                Move m(endIdx - forwardStep, endIdx, false, piecePawn,
                       Move::Promotion::NA);
                this->MakeMove(m, tracking);
                this->UnMakeMove(m, bs, tracking);
                Add(moves, m);
            }
            // Promotions
            while (currMoves) {
                int endIdx = pop_lsb(currMoves);
                for (Move::Promotion p : promotions) {
                    Move m(endIdx - forwardStep, endIdx, false, piecePawn, p);
                    this->MakeMove(m, tracking);
                    this->UnMakeMove(m, bs, tracking);
                    Add(moves, m);
                }
            }
        } else {
            // Black: promotions first
            while (currMoves & promoRank) {
                int endIdx = pop_lsb(currMoves);
                for (Move::Promotion p : promotions) {
                    Move m(endIdx - forwardStep, endIdx, false, piecePawn, p);
                    this->MakeMove(m, tracking);
                    this->UnMakeMove(m, bs, tracking);
                    Add(moves, m);
                }
            }
            // Non‑promotions
            while (currMoves) {
                int endIdx = pop_lsb(currMoves);
                Move m(endIdx - forwardStep, endIdx, false, piecePawn,
                       Move::Promotion::NA);
                this->MakeMove(m, tracking);
                this->UnMakeMove(m, bs, tracking);
                Add(moves, m);
            }
        }

        // Left captures
        currMoves = captureLeft(myPawns) & (opponentBB | enPassantMask);
        if constexpr (C == Color::WHITE) {
            while (currMoves & ~promoRank) {
                int endIdx = pop_lsb(currMoves);
                int startIdx = endIdx - (forwardStep - 1); // -7
                Move m(startIdx, endIdx, endIdx == this->enPassantIdx,
                       piecePawn, Move::Promotion::NA);
                this->MakeMove(m, tracking);
                this->UnMakeMove(m, bs, tracking);
                Add(moves, m);
            }
            while (currMoves) {
                int endIdx = pop_lsb(currMoves);
                for (Move::Promotion p : promotions) {
                    Move m(endIdx - (forwardStep - 1), endIdx, false, piecePawn, p);
                    this->MakeMove(m, tracking);
                    this->UnMakeMove(m, bs, tracking);
                    Add(moves, m);
                }
            }
        } else {
            // Black: promotions first
            while (currMoves & promoRank) {
                int endIdx = pop_lsb(currMoves);
                for (Move::Promotion p : promotions) {
                    Move m(endIdx - (forwardStep - 1), endIdx, false, piecePawn, p);
                    this->MakeMove(m, tracking);
                    this->UnMakeMove(m, bs, tracking);
                    Add(moves, m);
                }
            }
            while (currMoves) {
                int endIdx = pop_lsb(currMoves);
                int startIdx = endIdx - (forwardStep - 1);
                Move m(startIdx, endIdx, endIdx == this->enPassantIdx,
                       piecePawn, Move::Promotion::NA);
                this->MakeMove(m, tracking);
                this->UnMakeMove(m, bs, tracking);
                Add(moves, m);
            }
        }

        // Right captures
        currMoves = captureRight(myPawns) & (opponentBB | enPassantMask);
        if constexpr (C == Color::WHITE) {
            while (currMoves & ~promoRank) {
                int endIdx = pop_lsb(currMoves);
                int startIdx = endIdx - (forwardStep + 1); // -9
                Move m(startIdx, endIdx, endIdx == this->enPassantIdx,
                       piecePawn, Move::Promotion::NA);
                this->MakeMove(m, tracking);
                this->UnMakeMove(m, bs, tracking);
                Add(moves, m);
            }
            while (currMoves) {
                int endIdx = pop_lsb(currMoves);
                for (Move::Promotion p : promotions) {
                    Move m(endIdx - (forwardStep + 1), endIdx, false, piecePawn, p);
                    this->MakeMove(m, tracking);
                    this->UnMakeMove(m, bs, tracking);
                    Add(moves, m);
                }
            }
        } else {
            while (currMoves & promoRank) {
                int endIdx = pop_lsb(currMoves);
                for (Move::Promotion p : promotions) {
                    Move m(endIdx - (forwardStep + 1), endIdx, false, piecePawn, p);
                    this->MakeMove(m, tracking);
                    this->UnMakeMove(m, bs, tracking);
                    Add(moves, m);
                }
            }
            while (currMoves) {
                int endIdx = pop_lsb(currMoves);
                int startIdx = endIdx - (forwardStep + 1);
                Move m(startIdx, endIdx, endIdx == this->enPassantIdx,
                       piecePawn, Move::Promotion::NA);
                this->MakeMove(m, tracking);
                this->UnMakeMove(m, bs, tracking);
                Add(moves, m);
            }
        }

        // ---------- Knights ----------
        while (myKnights) {
            int idx = pop_lsb(myKnights);
            currMoves = KNIGHT_MOVES[idx] & ~ownPieces;
            while (currMoves) {
                int endIdx = pop_lsb(currMoves);
                Move m(idx, endIdx, false, pieceKnight, Move::Promotion::NA);
                this->MakeMove(m, tracking);
                this->UnMakeMove(m, bs, tracking);
                Add(moves, m);
            }
        }

        // ---------- Bishops ----------
        while (myBishops) {
            int idx = pop_lsb(myBishops);
            currMoves = BISHOP_MOVES[idx][BishopHash(idx, emptiesBB, opponentBB)];
            while (currMoves) {
                int endIdx = pop_lsb(currMoves);
                Move m(idx, endIdx, false, pieceBishop, Move::Promotion::NA);
                this->MakeMove(m, tracking);
                this->UnMakeMove(m, bs, tracking);
                Add(moves, m);
            }
        }

        // ---------- Rooks ----------
        while (myRooks) {
            int idx = pop_lsb(myRooks);
            currMoves = ROOK_MOVES[idx][RookHash(idx, emptiesBB, opponentBB)];
            while (currMoves) {
                int endIdx = pop_lsb(currMoves);
                Move m(idx, endIdx, false, pieceRook, Move::Promotion::NA);
                this->MakeMove(m, tracking);
                this->UnMakeMove(m, bs, tracking);
                Add(moves, m);
            }
        }

        // ---------- Queens ----------
        while (myQueens) {
            int idx = pop_lsb(myQueens);
            currMoves = BISHOP_MOVES[idx][BishopHash(idx, emptiesBB, opponentBB)] |
                        ROOK_MOVES[idx][RookHash(idx, emptiesBB, opponentBB)];
            while (currMoves) {
                int endIdx = pop_lsb(currMoves);
                Move m(idx, endIdx, false, pieceQueen, Move::Promotion::NA);
                this->MakeMove(m, tracking);
                this->UnMakeMove(m, bs, tracking);
                Add(moves, m);
            }
        }
    }

    // ---------- King moves (always generated) ----------
    int kingIdx = pop_lsb(myKing);
    currMoves = KING_MOVES[kingIdx] & ~ownPieces;
    while (currMoves) {
        int endIdx = pop_lsb(currMoves);
        Move m(kingIdx, endIdx, false, pieceKing, Move::Promotion::NA);
        this->MakeMove(m, tracking);
        this->UnMakeMove(m, bs, tracking);
        Add(moves, m);
    }

    // ---------- Castling ----------
    if constexpr (C == Color::WHITE) {
        if (this->wCastle && checkStatus == Move::Check::NO_CHECK &&
            ((emptiesBB & 0x0000000000000060) == 0x0000000000000060) &&
            (this->InChecks(Color::WHITE, 0x0000000000000020) == Move::Check::NO_CHECK)) {
            Move m(4, 6, false, pieceKing, Move::Promotion::NA);
            this->MakeMove(m, tracking);
            this->UnMakeMove(m, bs, tracking);
            Add(moves, m);
        }
        if (this->wQueenCastle && checkStatus == Move::Check::NO_CHECK &&
            ((emptiesBB & 0x000000000000000E) == 0x000000000000000E) &&
            (this->InChecks(Color::WHITE, 0x0000000000000008) == Move::Check::NO_CHECK)) {
            Move m(4, 2, false, pieceKing, Move::Promotion::NA);
            this->MakeMove(m, tracking);
            this->UnMakeMove(m, bs, tracking);
            Add(moves, m);
        }
    } else {
        if (this->bCastle && checkStatus == Move::Check::NO_CHECK &&
            ((emptiesBB & 0x6000000000000000) == 0x6000000000000000) &&
            (this->InChecks(Color::BLACK, 0x2000000000000000) == Move::Check::NO_CHECK)) {
            Move m(60, 62, false, pieceKing, Move::Promotion::NA);
            this->MakeMove(m, tracking);
            this->UnMakeMove(m, bs, tracking);
            Add(moves, m);
        }
        if (this->bQueenCastle && checkStatus == Move::Check::NO_CHECK &&
            ((emptiesBB & 0x0E00000000000000) == 0x0E00000000000000) &&
            (this->InChecks(Color::BLACK, 0x0800000000000000) == Move::Check::NO_CHECK)) {
            Move m(60, 58, false, pieceKing, Move::Promotion::NA);
            this->MakeMove(m, tracking);
            this->UnMakeMove(m, bs, tracking);
            Add(moves, m);
        }
    }
    return moves;
  }
};

#endif
