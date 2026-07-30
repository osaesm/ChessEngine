// chess.cpp
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <latch>
#include <vector>

#include "chess.hpp"

// Static move tables
uint64_t Chess::PAWN_TAKES[64][2] = {};
uint64_t Chess::KNIGHT_MOVES[64] = {};
uint64_t Chess::KING_MOVES[64] = {};
uint64_t Chess::BISHOP_MOVES[64][4096] = {};
uint64_t Chess::ROOK_MOVES[64][4096] = {};
Move::Promotion Chess::promotions[4] = {
    Move::Promotion::QUEEN, Move::Promotion::ROOK, Move::Promotion::KNIGHT,
    Move::Promotion::BISHOP};

// Zobrist key tables
uint64_t Chess::zobristPiece[64][12] = {};
uint64_t Chess::zobristEnPassant[8] = {};
uint64_t Chess::zobristCastle[4] = {};
uint64_t Chess::zobristBlackToMove = 0;

static thread_local RepetitionTracker repTracker;

void Add(MoveCategories &mC, PackedMove m) {
  if (m.check == Move::DOUBLE_CHECK) {
    mC.doubleChecks.push_back(m);
  } else if (m.check == Move::CHECK) {
    mC.checks.push_back(m);
  } else if (m.capture != Move::NONE) {
    mC.captures.push_back(m);
  } else {
    mC.etc.push_back(m);
  }
}

// ------------------------------------------------------------------
// Zobrist initialisation
// ------------------------------------------------------------------
void Chess::InitializeZobrist() {
  uint64_t seed = 1234567890123456789ULL;
  uint64_t s[4];
  s[0] = splitmix64(seed);
  s[1] = splitmix64(seed);
  s[2] = splitmix64(seed);
  s[3] = splitmix64(seed);

  auto next = [&]() -> uint64_t { return xoshiro256pp(s); };

  for (int sq = 0; sq < 64; ++sq) {
    for (int p = 0; p < 12; ++p) {
      zobristPiece[sq][p] = next();
    }
  }

  for (int f = 0; f < 8; ++f) {
    zobristEnPassant[f] = next();
  }

  for (int c = 0; c < 4; ++c) {
    zobristCastle[c] = next();
  }

  zobristBlackToMove = next();
}

// Hard-coded piece moves
void Chess::Initialize() {
  InitializeZobrist();

  for (auto row = 0; row < 8; ++row) {
    for (auto col = 0; col < 8; ++col) {
      auto idx = (row * 8) + col;
      for (int left = 0; left <= col; ++left) {
        for (int up = 0; (row + up) < 8; ++up) {
          for (int right = 0; (right + col) < 8; ++right) {
            for (int down = 0; down <= row; ++down) {
              int hash = (left << 9) + (up << 6) + (right << 3) + down;
              ROOK_MOVES[idx][hash] = 0ULL;
              for (auto x = idx - (down * 8); x < idx; x += 8) {
                set_bit(ROOK_MOVES[idx][hash], x);
              }
              for (auto x = idx - left; x < idx; ++x) {
                set_bit(ROOK_MOVES[idx][hash], x);
              }
              for (auto x = idx + right; x > idx; --x) {
                set_bit(ROOK_MOVES[idx][hash], x);
              }
              for (auto x = idx + (up * 8); x > idx; x -= 8) {
                set_bit(ROOK_MOVES[idx][hash], x);
              }
            }
          }
        }
      }
      for (int upLeft = 0; upLeft <= col && (upLeft + row) < 8; ++upLeft) {
        for (int upRight = 0; (upRight + col) < 8 && (row + upRight) < 8;
             ++upRight) {
          for (int downRight = 0; (downRight + col) < 8 && downRight <= row;
               ++downRight) {
            for (int downLeft = 0; downLeft <= col && downLeft <= row;
                 ++downLeft) {
              int hash =
                  (upLeft << 9) + (upRight << 6) + (downRight << 3) + downLeft;
              BISHOP_MOVES[idx][hash] = 0ULL;
              for (auto x = idx - (downRight * 7); x < idx; x += 7) {
                set_bit(BISHOP_MOVES[idx][hash], x);
              }
              for (auto x = idx - (downLeft * 9); x < idx; x += 9) {
                set_bit(BISHOP_MOVES[idx][hash], x);
              }
              for (auto x = idx + (upLeft * 7); x > idx; x -= 7) {
                set_bit(BISHOP_MOVES[idx][hash], x);
              }
              for (auto x = idx + (upRight * 9); x > idx; x -= 9) {
                set_bit(BISHOP_MOVES[idx][hash], x);
              }
            }
          }
        }
      }
    }
  }

  for (uint64_t i = 0ULL, iMask = 1ULL; i < 64; ++i, iMask <<= 1) {
    PAWN_TAKES[i][0] = 0ULL;
    PAWN_TAKES[i][0] = up_left(iMask) | up_right(iMask);
    PAWN_TAKES[i][1] = 0ULL;
    PAWN_TAKES[i][1] = down_left(iMask) | down_right(iMask);
    KNIGHT_MOVES[i] = 0ULL;
    KNIGHT_MOVES[i] = down(down_left(iMask)) | down(down_right(iMask)) |
                      left(down_left(iMask)) | right(down_right(iMask)) |
                      up(up_left(iMask)) | up(up_right(iMask)) |
                      left(up_left(iMask)) | right(up_right(iMask));
    KING_MOVES[i] = 0ULL;
    KING_MOVES[i] = down_left(iMask) | down(iMask) | down_right(iMask) |
                    left(iMask) | right(iMask) | up_left(iMask) | up(iMask) |
                    up_right(iMask);
  }
}

Chess::Chess(const std::string &fenString) {
  // First part
  auto currSquare = 56;
  auto idx = 0;
  this->wPawns = 0ULL;
  this->bPawns = 0ULL;
  this->wKnights = 0ULL;
  this->bKnights = 0ULL;
  this->wBishops = 0ULL;
  this->bBishops = 0ULL;
  this->wRooks = 0ULL;
  this->bRooks = 0ULL;
  this->wQueens = 0ULL;
  this->bQueens = 0ULL;
  this->wKing = 0ULL;
  this->bKing = 0ULL;
  while (fenString[idx] != ' ') {
    switch (fenString[idx]) {
    case 'P':
      set_bit(this->wPawns, currSquare);
      ++currSquare;
      break;
    case 'p':
      set_bit(this->bPawns, currSquare);
      ++currSquare;
      break;
    case 'N':
      set_bit(this->wKnights, currSquare);
      ++currSquare;
      break;
    case 'n':
      set_bit(this->bKnights, currSquare);
      ++currSquare;
      break;
    case 'B':
      set_bit(this->wBishops, currSquare);
      ++currSquare;
      break;
    case 'b':
      set_bit(this->bBishops, currSquare);
      ++currSquare;
      break;
    case 'R':
      set_bit(this->wRooks, currSquare);
      ++currSquare;
      break;
    case 'r':
      set_bit(this->bRooks, currSquare);
      ++currSquare;
      break;
    case 'Q':
      set_bit(this->wQueens, currSquare);
      ++currSquare;
      break;
    case 'q':
      set_bit(this->bQueens, currSquare);
      ++currSquare;
      break;
    case 'K':
      set_bit(this->wKing, currSquare);
      ++currSquare;
      break;
    case 'k':
      set_bit(this->bKing, currSquare);
      ++currSquare;
      break;
    case '/':
      currSquare -= 16;
      break;
    default:
      currSquare += (fenString[idx] - '0');
      break;
    }
    ++idx;
  }
  ++idx;

  // Second Part
  this->turn = (fenString[idx] == 'w') ? Color::WHITE : Color::BLACK;

  // Third Part
  idx += 2;
  this->wCastle = false;
  this->wQueenCastle = false;
  this->bCastle = false;
  this->bQueenCastle = false;
  if (fenString[idx] == 'K') {
    this->wCastle = true;
    ++idx;
  }
  if (fenString[idx] == 'Q') {
    this->wQueenCastle = true;
    ++idx;
  }
  if (fenString[idx] == 'k') {
    this->bCastle = true;
    ++idx;
  }
  if (fenString[idx] == 'q') {
    this->bQueenCastle = true;
    ++idx;
  }
  if (!this->wCastle && !this->wQueenCastle && !this->bCastle &&
      !this->bQueenCastle) {
    ++idx;
  }
  // Fourth Part
  ++idx;
  this->enPassantIdx = -1;
  if (fenString[idx] != '-') {
    this->enPassantIdx = (fenString[idx] - 'a');
    ++idx;
    this->enPassantIdx += (fenString[idx] == '3') ? 16 : 40;
  }

  // Fifth Part
  idx += 2;
  this->lastPawnOrTake = 0;
  while (fenString[idx] != ' ') {
    this->lastPawnOrTake *= 10;
    this->lastPawnOrTake += (fenString[idx] - '0');
    ++idx;
  }

  // Sixth Part
  ++idx;
  this->fullTurns = 0;
  while (idx < fenString.length() && fenString[idx] != ' ') {
    this->fullTurns *= 10;
    this->fullTurns += (fenString[idx] - '0');
    ++idx;
  }

  // Compute initial Zobrist hash
  uint64_t h = 0;
  for (int sq = 0; sq < 64; ++sq) {
    if (get_bit(wPawns, sq)) {
      h ^= zobristPiece[sq][0];
    }
    if (get_bit(wKnights, sq)) {
      h ^= zobristPiece[sq][1];
    }
    if (get_bit(wBishops, sq)) {
      h ^= zobristPiece[sq][2];
    }
    if (get_bit(wRooks, sq)) {
      h ^= zobristPiece[sq][3];
    }
    if (get_bit(wQueens, sq)) {
      h ^= zobristPiece[sq][4];
    }
    if (get_bit(wKing, sq)) {
      h ^= zobristPiece[sq][5];
    }
    if (get_bit(bPawns, sq)) {
      h ^= zobristPiece[sq][6];
    }
    if (get_bit(bKnights, sq)) {
      h ^= zobristPiece[sq][7];
    }
    if (get_bit(bBishops, sq)) {
      h ^= zobristPiece[sq][8];
    }
    if (get_bit(bRooks, sq)) {
      h ^= zobristPiece[sq][9];
    }
    if (get_bit(bQueens, sq)) {
      h ^= zobristPiece[sq][10];
    }
    if (get_bit(bKing, sq)) {
      h ^= zobristPiece[sq][11];
    }
  }
  if (wCastle) {
    h ^= zobristCastle[0];
  }
  if (wQueenCastle) {
    h ^= zobristCastle[1];
  }
  if (bCastle) {
    h ^= zobristCastle[2];
  }
  if (bQueenCastle) {
    h ^= zobristCastle[3];
  }
  if (enPassantIdx != -1) {
    h ^= zobristEnPassant[enPassantIdx % 8];
  }
  if (turn == Color::BLACK) {
    h ^= zobristBlackToMove;
  }
  this->hash = h;
}

Chess::Chess(const Chess &x) {
  this->wPawns = x.wPawns;
  this->bPawns = x.bPawns;
  this->wKnights = x.wKnights;
  this->bKnights = x.bKnights;
  this->wBishops = x.wBishops;
  this->bBishops = x.bBishops;
  this->wRooks = x.wRooks;
  this->bRooks = x.bRooks;
  this->wQueens = x.wQueens;
  this->bQueens = x.bQueens;
  this->wKing = x.wKing;
  this->bKing = x.bKing;
  this->turn = x.turn;
  this->wCastle = x.wCastle;
  this->wQueenCastle = x.wQueenCastle;
  this->bCastle = x.bCastle;
  this->bQueenCastle = x.bQueenCastle;
  this->enPassantIdx = x.enPassantIdx;
  this->lastPawnOrTake = x.lastPawnOrTake;
  this->fullTurns = x.fullTurns;
  this->hash = x.hash;
}

const std::string Chess::BoardIdx() {
  Chess gameCopy(Chess(*this));
  char board[64];
  for (auto x = 0; x < 64; ++x) {
    board[x] = ' ';
  }
  while (gameCopy.wPawns) {
    board[pop_lsb(gameCopy.wPawns)] = 'P';
  }
  while (gameCopy.bPawns) {
    board[pop_lsb(gameCopy.bPawns)] = 'p';
  }
  while (gameCopy.wKnights) {
    board[pop_lsb(gameCopy.wKnights)] = 'N';
  }
  while (gameCopy.bKnights) {
    board[pop_lsb(gameCopy.bKnights)] = 'n';
  }
  while (gameCopy.wBishops) {
    board[pop_lsb(gameCopy.wBishops)] = 'B';
  }
  while (gameCopy.bBishops) {
    board[pop_lsb(gameCopy.bBishops)] = 'b';
  }
  while (gameCopy.wRooks) {
    board[pop_lsb(gameCopy.wRooks)] = 'R';
  }
  while (gameCopy.bRooks) {
    board[pop_lsb(gameCopy.bRooks)] = 'r';
  }
  while (gameCopy.wQueens) {
    board[pop_lsb(gameCopy.wQueens)] = 'Q';
  }
  while (gameCopy.bQueens) {
    board[pop_lsb(gameCopy.bQueens)] = 'q';
  }
  while (gameCopy.wKing) {
    board[pop_lsb(gameCopy.wKing)] = 'K';
  }
  while (gameCopy.bKing) {
    board[pop_lsb(gameCopy.bKing)] = 'k';
  }

  std::string boardHash = "";
  for (int rowStart = 56; rowStart >= 0; rowStart -= 8) {
    int numSpaces = 0;
    for (int idx = rowStart; idx < (rowStart + 8); ++idx) {
      if (board[idx] != ' ') {
        if (numSpaces != 0) {
          boardHash += std::to_string(numSpaces);
        }
        boardHash += board[idx];
        numSpaces = 0;
      } else {
        ++numSpaces;
      }
    }
    if (numSpaces != 0) {
      boardHash += std::to_string(numSpaces);
    }
    if (rowStart > 0) {
      boardHash += "/";
    }
  }

  if (this->turn == Color::WHITE) {
    boardHash += " w ";
  } else {
    boardHash += " b ";
  }

  if (this->wCastle) {
    boardHash += "K";
  }
  if (this->wQueenCastle) {
    boardHash += "Q";
  }
  if (this->bCastle) {
    boardHash += "k";
  }
  if (this->bQueenCastle) {
    boardHash += "q";
  } else if (!this->wCastle && !this->wQueenCastle && !this->bCastle &&
             !this->bQueenCastle) {
    boardHash += "-";
  }

  if (this->enPassantIdx == -1) {
    boardHash += " -";
  } else {
    char file = 'a' + (this->enPassantIdx % 8);
    int rank = (this->enPassantIdx / 8) + 1;
    boardHash += " " + std::string(1, file) + std::to_string(rank);
  }
  return boardHash;
}

const std::string Chess::ConvertToFEN() {
  return this->BoardIdx() + " " + std::to_string(this->lastPawnOrTake) + " " +
         std::to_string(this->fullTurns);
}

const Move::Check Chess::InChecks(const Color kingColor,
                                  const uint64_t kingBoard) const {
  uint64_t currEmpties = this->empties();
  if (kingColor == Color::WHITE && kingBoard != this->wKing) {
    currEmpties = (currEmpties | this->wKing) & ~kingBoard;
  } else if (kingColor == Color::BLACK && kingBoard != this->bKing) {
    currEmpties = (currEmpties | this->bKing) & ~kingBoard;
  }
  uint64_t opponent =
      (kingColor == Color::WHITE) ? this->blacks() : this->whites();
  uint64_t oppRooks = (kingColor == Color::WHITE) ? this->bRooks : this->wRooks;
  uint64_t oppBishops =
      (kingColor == Color::WHITE) ? this->bBishops : this->wBishops;
  uint64_t oppKnights =
      (kingColor == Color::WHITE) ? this->bKnights : this->wKnights;
  uint64_t oppQueens =
      (kingColor == Color::WHITE) ? this->bQueens : this->wQueens;
  uint64_t oppPawns = (kingColor == Color::WHITE) ? this->bPawns : this->wPawns;
  uint64_t oppKing = (kingColor == Color::WHITE) ? this->bKing : this->wKing;

  const uint64_t kingIdx = std::countr_zero(kingBoard);
  uint64_t checkMasks[5] = {
      (BISHOP_MOVES[kingIdx][BishopHash(kingIdx, currEmpties, opponent)] &
       (oppBishops | oppQueens)),
      (ROOK_MOVES[kingIdx][RookHash(kingIdx, currEmpties, opponent)] &
       (oppRooks | oppQueens)),
      (KNIGHT_MOVES[kingIdx] & oppKnights), (KING_MOVES[kingIdx] & oppKing),
      (PAWN_TAKES[kingIdx][(kingColor == Color::WHITE) ? 0 : 1] & oppPawns)};
  Move::Check checkType = Move::NO_CHECK;
  for (int i = 0; i < 5; ++i) {
    while (checkMasks[i]) {
      pop_lsb(checkMasks[i]);
      if (checkType == Move::NO_CHECK) {
        checkType = Move::CHECK;
      } else if (checkType == Move::CHECK) {
        return Move::DOUBLE_CHECK;
      }
    }
  }
  return checkType;
}

bool Chess::isRepetition() const { return repTracker.isRepetition(this->hash); }

void Chess::MakeMove(PackedMove m, const bool tracking) {
  int start = m.start;
  int end = m.end;
  Move::Piece pieceType = static_cast<Move::Piece>(m.piece);
  Move::Piece captureType = static_cast<Move::Piece>(m.capture);
  Move::Promotion promo = static_cast<Move::Promotion>(m.promo);
  bool enPassant = (m.enPassant != 0);

  if (this->enPassantIdx != -1) {
    this->hash ^= zobristEnPassant[this->enPassantIdx % 8];
  }
  this->enPassantIdx = -1;

  if (this->turn != Color::WHITE) {
    ++this->fullTurns;
  }
  if (tracking) {
    ++this->lastPawnOrTake;
  }

  bool irreversible = false;
  if (pieceType == Move::W_PAWN || pieceType == Move::B_PAWN ||
      captureType != Move::NONE || enPassant) {
    irreversible = true;
  }

  switch (pieceType) {
  case Move::W_PAWN:
    this->hash ^= zobristPiece[start][0];
    clear_bit(this->wPawns, start);
    switch (promo) {
    case Move::QUEEN:
      set_bit(this->wQueens, end);
      this->hash ^= zobristPiece[end][4];
      break;
    case Move::ROOK:
      set_bit(this->wRooks, end);
      this->hash ^= zobristPiece[end][3];
      break;
    case Move::KNIGHT:
      set_bit(this->wKnights, end);
      this->hash ^= zobristPiece[end][1];
      break;
    case Move::BISHOP:
      set_bit(this->wBishops, end);
      this->hash ^= zobristPiece[end][2];
      break;
    default:
      if (end - start == 16) {
        this->enPassantIdx = start + 8;
        this->hash ^= zobristEnPassant[this->enPassantIdx % 8];
      }
      set_bit(this->wPawns, end);
      this->hash ^= zobristPiece[end][0];
      break;
    }
    if (tracking) {
      this->lastPawnOrTake = 0;
    }
    break;
  case Move::W_KNIGHT:
    this->hash ^= zobristPiece[start][1];
    clear_bit(this->wKnights, start);
    set_bit(this->wKnights, end);
    this->hash ^= zobristPiece[end][1];
    break;
  case Move::W_BISHOP:
    this->hash ^= zobristPiece[start][2];
    clear_bit(this->wBishops, start);
    set_bit(this->wBishops, end);
    this->hash ^= zobristPiece[end][2];
    break;
  case Move::W_ROOK:
    this->hash ^= zobristPiece[start][3];
    clear_bit(this->wRooks, start);
    set_bit(this->wRooks, end);
    this->hash ^= zobristPiece[end][3];
    if (this->wCastle && start == 7) {
      this->hash ^= zobristCastle[0];
      this->wCastle = false;
    } else if (this->wQueenCastle && start == 0) {
      this->hash ^= zobristCastle[1];
      this->wQueenCastle = false;
    }
    break;
  case Move::W_QUEEN:
    this->hash ^= zobristPiece[start][4];
    clear_bit(this->wQueens, start);
    set_bit(this->wQueens, end);
    this->hash ^= zobristPiece[end][4];
    break;
  case Move::W_KING:
    this->hash ^= zobristPiece[start][5];
    clear_bit(this->wKing, start);
    set_bit(this->wKing, end);
    this->hash ^= zobristPiece[end][5];
    if (this->wCastle) {
      this->hash ^= zobristCastle[0];
      this->wCastle = false;
    }
    if (this->wQueenCastle) {
      this->hash ^= zobristCastle[1];
      this->wQueenCastle = false;
    }
    if (start == 4 && end == 6) {
      clear_bit(this->wRooks, 7);
      set_bit(this->wRooks, 5);
      this->hash ^= zobristPiece[7][3];
      this->hash ^= zobristPiece[5][3];
    } else if (start == 4 && end == 2) {
      clear_bit(this->wRooks, 0);
      set_bit(this->wRooks, 3);
      this->hash ^= zobristPiece[0][3];
      this->hash ^= zobristPiece[3][3];
    }
    break;
  case Move::B_PAWN:
    this->hash ^= zobristPiece[start][6];
    clear_bit(this->bPawns, start);
    switch (promo) {
    case Move::QUEEN:
      set_bit(this->bQueens, end);
      this->hash ^= zobristPiece[end][10];
      break;
    case Move::ROOK:
      set_bit(this->bRooks, end);
      this->hash ^= zobristPiece[end][9];
      break;
    case Move::KNIGHT:
      set_bit(this->bKnights, end);
      this->hash ^= zobristPiece[end][7];
      break;
    case Move::BISHOP:
      set_bit(this->bBishops, end);
      this->hash ^= zobristPiece[end][8];
      break;
    default:
      if (start - end == 16) {
        this->enPassantIdx = start - 8;
        this->hash ^= zobristEnPassant[this->enPassantIdx % 8];
      }
      set_bit(this->bPawns, end);
      this->hash ^= zobristPiece[end][6];
      break;
    }
    if (tracking) {
      this->lastPawnOrTake = 0;
    }
    break;
  case Move::B_KNIGHT:
    this->hash ^= zobristPiece[start][7];
    clear_bit(this->bKnights, start);
    set_bit(this->bKnights, end);
    this->hash ^= zobristPiece[end][7];
    break;
  case Move::B_BISHOP:
    this->hash ^= zobristPiece[start][8];
    clear_bit(this->bBishops, start);
    set_bit(this->bBishops, end);
    this->hash ^= zobristPiece[end][8];
    break;
  case Move::B_ROOK:
    this->hash ^= zobristPiece[start][9];
    clear_bit(this->bRooks, start);
    set_bit(this->bRooks, end);
    this->hash ^= zobristPiece[end][9];
    if (this->bCastle && start == 63) {
      this->hash ^= zobristCastle[2];
      this->bCastle = false;
    } else if (this->bQueenCastle && start == 56) {
      this->hash ^= zobristCastle[3];
      this->bQueenCastle = false;
    }
    break;
  case Move::B_QUEEN:
    this->hash ^= zobristPiece[start][10];
    clear_bit(this->bQueens, start);
    set_bit(this->bQueens, end);
    this->hash ^= zobristPiece[end][10];
    break;
  case Move::B_KING:
    this->hash ^= zobristPiece[start][11];
    clear_bit(this->bKing, start);
    set_bit(this->bKing, end);
    this->hash ^= zobristPiece[end][11];
    if (this->bCastle) {
      this->hash ^= zobristCastle[2];
      this->bCastle = false;
    }
    if (this->bQueenCastle) {
      this->hash ^= zobristCastle[3];
      this->bQueenCastle = false;
    }
    if (start == 60 && end == 62) {
      clear_bit(this->bRooks, 63);
      set_bit(this->bRooks, 61);
      this->hash ^= zobristPiece[63][9];
      this->hash ^= zobristPiece[61][9];
    } else if (start == 60 && end == 58) {
      clear_bit(this->bRooks, 56);
      set_bit(this->bRooks, 59);
      this->hash ^= zobristPiece[56][9];
      this->hash ^= zobristPiece[59][9];
    }
    break;
  default:
    break;
  }

  if (enPassant) {
    if (pieceType == Move::W_PAWN) {
      clear_bit(this->bPawns, end - 8);
      this->hash ^= zobristPiece[end - 8][6];
    } else {
      clear_bit(this->wPawns, end + 8);
      this->hash ^= zobristPiece[end + 8][0];
    }
  } else if (captureType != Move::NONE) {
    if (tracking) {
      this->lastPawnOrTake = 0;
    }
    switch (captureType) {
    case Move::W_PAWN:
      clear_bit(this->wPawns, end);
      this->hash ^= zobristPiece[end][0];
      break;
    case Move::W_KNIGHT:
      clear_bit(this->wKnights, end);
      this->hash ^= zobristPiece[end][1];
      break;
    case Move::W_BISHOP:
      clear_bit(this->wBishops, end);
      this->hash ^= zobristPiece[end][2];
      break;
    case Move::W_ROOK:
      clear_bit(this->wRooks, end);
      this->hash ^= zobristPiece[end][3];
      if (end == 0 && this->wQueenCastle) {
        this->hash ^= zobristCastle[1];
        this->wQueenCastle = false;
      } else if (end == 7 && this->wCastle) {
        this->hash ^= zobristCastle[0];
        this->wCastle = false;
      }
      break;
    case Move::W_QUEEN:
      clear_bit(this->wQueens, end);
      this->hash ^= zobristPiece[end][4];
      break;
    case Move::W_KING:
      clear_bit(this->wKing, end);
      this->hash ^= zobristPiece[end][5];
      break;
    case Move::B_PAWN:
      clear_bit(this->bPawns, end);
      this->hash ^= zobristPiece[end][6];
      break;
    case Move::B_KNIGHT:
      clear_bit(this->bKnights, end);
      this->hash ^= zobristPiece[end][7];
      break;
    case Move::B_BISHOP:
      clear_bit(this->bBishops, end);
      this->hash ^= zobristPiece[end][8];
      break;
    case Move::B_ROOK:
      clear_bit(this->bRooks, end);
      this->hash ^= zobristPiece[end][9];
      if (end == 56 && this->bQueenCastle) {
        this->hash ^= zobristCastle[3];
        this->bQueenCastle = false;
      } else if (end == 63 && this->bCastle) {
        this->hash ^= zobristCastle[2];
        this->bCastle = false;
      }
      break;
    case Move::B_QUEEN:
      clear_bit(this->bQueens, end);
      this->hash ^= zobristPiece[end][10];
      break;
    case Move::B_KING:
      clear_bit(this->bKing, end);
      this->hash ^= zobristPiece[end][11];
      break;
    default:
      break;
    }
  }

  this->hash ^= zobristBlackToMove;
  this->turn = (this->turn == Color::WHITE) ? Color::BLACK : Color::WHITE;

  if (tracking) {
    repTracker.push(this->hash, irreversible);
  }
}

void Chess::UnMakeMove(PackedMove m, const BoardState &bs,
                       const bool tracking) {
  int start = m.start;
  int end = m.end;
  Move::Piece pieceType = static_cast<Move::Piece>(m.piece);
  Move::Piece captureType = static_cast<Move::Piece>(m.capture);
  Move::Promotion promo = static_cast<Move::Promotion>(m.promo);
  bool enPassant = (m.enPassant != 0);

  switch (pieceType) {
  case Move::W_PAWN:
    switch (promo) {
    case Move::QUEEN:
      clear_bit(this->wQueens, end);
      break;
    case Move::ROOK:
      clear_bit(this->wRooks, end);
      break;
    case Move::KNIGHT:
      clear_bit(this->wKnights, end);
      break;
    case Move::BISHOP:
      clear_bit(this->wBishops, end);
      break;
    default:
      clear_bit(this->wPawns, end);
      break;
    }
    set_bit(this->wPawns, start);
    break;
  case Move::W_KNIGHT:
    clear_bit(this->wKnights, end);
    set_bit(this->wKnights, start);
    break;
  case Move::W_BISHOP:
    clear_bit(this->wBishops, end);
    set_bit(this->wBishops, start);
    break;
  case Move::W_ROOK:
    clear_bit(this->wRooks, end);
    set_bit(this->wRooks, start);
    break;
  case Move::W_QUEEN:
    clear_bit(this->wQueens, end);
    set_bit(this->wQueens, start);
    break;
  case Move::W_KING:
    clear_bit(this->wKing, end);
    set_bit(this->wKing, start);
    if (start == 4 && end == 6) {
      clear_bit(this->wRooks, 5);
      set_bit(this->wRooks, 7);
    } else if (start == 4 && end == 2) {
      clear_bit(this->wRooks, 3);
      set_bit(this->wRooks, 0);
    }
    break;
  case Move::B_PAWN:
    switch (promo) {
    case Move::QUEEN:
      clear_bit(this->bQueens, end);
      break;
    case Move::ROOK:
      clear_bit(this->bRooks, end);
      break;
    case Move::KNIGHT:
      clear_bit(this->bKnights, end);
      break;
    case Move::BISHOP:
      clear_bit(this->bBishops, end);
      break;
    default:
      clear_bit(this->bPawns, end);
      break;
    }
    set_bit(this->bPawns, start);
    break;
  case Move::B_KNIGHT:
    clear_bit(this->bKnights, end);
    set_bit(this->bKnights, start);
    break;
  case Move::B_BISHOP:
    clear_bit(this->bBishops, end);
    set_bit(this->bBishops, start);
    break;
  case Move::B_ROOK:
    clear_bit(this->bRooks, end);
    set_bit(this->bRooks, start);
    break;
  case Move::B_QUEEN:
    clear_bit(this->bQueens, end);
    set_bit(this->bQueens, start);
    break;
  case Move::B_KING:
    clear_bit(this->bKing, end);
    set_bit(this->bKing, start);
    if (start == 60 && end == 62) {
      clear_bit(this->bRooks, 61);
      set_bit(this->bRooks, 63);
    } else if (start == 60 && end == 58) {
      clear_bit(this->bRooks, 59);
      set_bit(this->bRooks, 56);
    }
    break;
  default:
    std::cout << "Unmoving a NONE piece??" << std::endl;
  }

  if (enPassant) {
    if (pieceType == Move::W_PAWN) {
      set_bit(this->bPawns, end - 8);
    } else {
      set_bit(this->wPawns, end + 8);
    }
  } else if (captureType != Move::NONE) {
    switch (captureType) {
    case Move::W_PAWN:
      set_bit(this->wPawns, end);
      break;
    case Move::W_KNIGHT:
      set_bit(this->wKnights, end);
      break;
    case Move::W_BISHOP:
      set_bit(this->wBishops, end);
      break;
    case Move::W_ROOK:
      set_bit(this->wRooks, end);
      break;
    case Move::W_QUEEN:
      set_bit(this->wQueens, end);
      break;
    case Move::W_KING:
      set_bit(this->wKing, end);
      break;
    case Move::B_PAWN:
      set_bit(this->bPawns, end);
      break;
    case Move::B_KNIGHT:
      set_bit(this->bKnights, end);
      break;
    case Move::B_BISHOP:
      set_bit(this->bBishops, end);
      break;
    case Move::B_ROOK:
      set_bit(this->bRooks, end);
      break;
    case Move::B_QUEEN:
      set_bit(this->bQueens, end);
      break;
    case Move::B_KING:
      set_bit(this->bKing, end);
      break;
    default:
      break;
    }
  }

  this->turn = (this->turn == Color::WHITE) ? Color::BLACK : Color::WHITE;
  this->wCastle = bs.wCastle;
  this->wQueenCastle = bs.wQueenCastle;
  this->bCastle = bs.bCastle;
  this->bQueenCastle = bs.bQueenCastle;
  this->enPassantIdx = bs.enPassantIdx;
  this->fullTurns = bs.fullTurns;
  this->hash = bs.hash;
  if (tracking) {
    this->lastPawnOrTake = bs.lastPawnOrTake;
    repTracker.moveCounter = bs.moveCounter;
    repTracker.repetitionStart = bs.repetitionStart;
    repTracker.pop();
  }
}

// ------------------------------------------------------------------
// computeCheckAfterMove – raw parameters, no packing
// ------------------------------------------------------------------
Move::Check Chess::computeCheckAfterMove(int start, int end, bool enPassant,
                                         Move::Piece piece,
                                         Move::Promotion promo,
                                         Move::Piece capture) const {
  Chess sim(*this);

  // Remove moving piece
  switch (piece) {
  case Move::W_PAWN:
    clear_bit(sim.wPawns, start);
    break;
  case Move::B_PAWN:
    clear_bit(sim.bPawns, start);
    break;
  case Move::W_KNIGHT:
    clear_bit(sim.wKnights, start);
    break;
  case Move::B_KNIGHT:
    clear_bit(sim.bKnights, start);
    break;
  case Move::W_BISHOP:
    clear_bit(sim.wBishops, start);
    break;
  case Move::B_BISHOP:
    clear_bit(sim.bBishops, start);
    break;
  case Move::W_ROOK:
    clear_bit(sim.wRooks, start);
    break;
  case Move::B_ROOK:
    clear_bit(sim.bRooks, start);
    break;
  case Move::W_QUEEN:
    clear_bit(sim.wQueens, start);
    break;
  case Move::B_QUEEN:
    clear_bit(sim.bQueens, start);
    break;
  case Move::W_KING:
    clear_bit(sim.wKing, start);
    break;
  case Move::B_KING:
    clear_bit(sim.bKing, start);
    break;
  default:
    break;
  }

  Move::Piece finalPiece = piece;
  if (promo != Move::NA) {
    if (turn == Color::WHITE) {
      switch (promo) {
      case Move::QUEEN:
        finalPiece = Move::W_QUEEN;
        break;
      case Move::ROOK:
        finalPiece = Move::W_ROOK;
        break;
      case Move::KNIGHT:
        finalPiece = Move::W_KNIGHT;
        break;
      case Move::BISHOP:
        finalPiece = Move::W_BISHOP;
        break;
      default:
        break;
      }
    } else {
      switch (promo) {
      case Move::QUEEN:
        finalPiece = Move::B_QUEEN;
        break;
      case Move::ROOK:
        finalPiece = Move::B_ROOK;
        break;
      case Move::KNIGHT:
        finalPiece = Move::B_KNIGHT;
        break;
      case Move::BISHOP:
        finalPiece = Move::B_BISHOP;
        break;
      default:
        break;
      }
    }
  }

  switch (finalPiece) {
  case Move::W_PAWN:
    set_bit(sim.wPawns, end);
    break;
  case Move::B_PAWN:
    set_bit(sim.bPawns, end);
    break;
  case Move::W_KNIGHT:
    set_bit(sim.wKnights, end);
    break;
  case Move::B_KNIGHT:
    set_bit(sim.bKnights, end);
    break;
  case Move::W_BISHOP:
    set_bit(sim.wBishops, end);
    break;
  case Move::B_BISHOP:
    set_bit(sim.bBishops, end);
    break;
  case Move::W_ROOK:
    set_bit(sim.wRooks, end);
    break;
  case Move::B_ROOK:
    set_bit(sim.bRooks, end);
    break;
  case Move::W_QUEEN:
    set_bit(sim.wQueens, end);
    break;
  case Move::B_QUEEN:
    set_bit(sim.bQueens, end);
    break;
  case Move::W_KING:
    set_bit(sim.wKing, end);
    break;
  case Move::B_KING:
    set_bit(sim.bKing, end);
    break;
  default:
    break;
  }

  if (enPassant) {
    if (turn == Color::WHITE) {
      clear_bit(sim.bPawns, end - 8);
    } else {
      clear_bit(sim.wPawns, end + 8);
    }
  } else if (capture != Move::NONE) {
    switch (capture) {
    case Move::W_PAWN:
      clear_bit(sim.wPawns, end);
      break;
    case Move::B_PAWN:
      clear_bit(sim.bPawns, end);
      break;
    case Move::W_KNIGHT:
      clear_bit(sim.wKnights, end);
      break;
    case Move::B_KNIGHT:
      clear_bit(sim.bKnights, end);
      break;
    case Move::W_BISHOP:
      clear_bit(sim.wBishops, end);
      break;
    case Move::B_BISHOP:
      clear_bit(sim.bBishops, end);
      break;
    case Move::W_ROOK:
      clear_bit(sim.wRooks, end);
      break;
    case Move::B_ROOK:
      clear_bit(sim.bRooks, end);
      break;
    case Move::W_QUEEN:
      clear_bit(sim.wQueens, end);
      break;
    case Move::B_QUEEN:
      clear_bit(sim.bQueens, end);
      break;
    case Move::W_KING:
      clear_bit(sim.wKing, end);
      break;
    case Move::B_KING:
      clear_bit(sim.bKing, end);
      break;
    default:
      break;
    }
  }

  // Castling rook movement for check detection
  if (piece == Move::W_KING) {
    if (start == 4 && end == 6) {
      clear_bit(sim.wRooks, 7);
      set_bit(sim.wRooks, 5);
    } else if (start == 4 && end == 2) {
      clear_bit(sim.wRooks, 0);
      set_bit(sim.wRooks, 3);
    }
  } else if (piece == Move::B_KING) {
    if (start == 60 && end == 62) {
      clear_bit(sim.bRooks, 63);
      set_bit(sim.bRooks, 61);
    } else if (start == 60 && end == 58) {
      clear_bit(sim.bRooks, 56);
      set_bit(sim.bRooks, 59);
    }
  }

  Color nextToMove = (turn == Color::WHITE) ? Color::BLACK : Color::WHITE;
  return sim.InChecks(nextToMove,
                      (nextToMove == Color::WHITE) ? sim.wKing : sim.bKing);
}

// ------------------------------------------------------------------
// Move generation template
// ------------------------------------------------------------------
template <Color C>
void Chess::generatePseudoLegalMoves(const Move::Check checkStatus,
                                     MoveCategories &moves) const {
  Chess gameCopy(*this);
  uint64_t currMoves = 0ULL;
  const uint64_t enPassantMask =
      (enPassantIdx == -1) ? 0ULL : (1ULL << enPassantIdx);

  constexpr int forwardStep = (C == Color::WHITE) ? 8 : -8;
  constexpr uint64_t startRank = (C == Color::WHITE) ? RANK_2 : RANK_7;
  constexpr uint64_t promoRank = (C == Color::WHITE) ? RANK_8 : RANK_1;

  constexpr Move::Piece piecePawn =
      (C == Color::WHITE) ? Move::W_PAWN : Move::B_PAWN;
  constexpr Move::Piece pieceKnight =
      (C == Color::WHITE) ? Move::W_KNIGHT : Move::B_KNIGHT;
  constexpr Move::Piece pieceBishop =
      (C == Color::WHITE) ? Move::W_BISHOP : Move::B_BISHOP;
  constexpr Move::Piece pieceRook =
      (C == Color::WHITE) ? Move::W_ROOK : Move::B_ROOK;
  constexpr Move::Piece pieceQueen =
      (C == Color::WHITE) ? Move::W_QUEEN : Move::B_QUEEN;
  constexpr Move::Piece pieceKing =
      (C == Color::WHITE) ? Move::W_KING : Move::B_KING;

  uint64_t &myPawns = (C == Color::WHITE) ? gameCopy.wPawns : gameCopy.bPawns;
  uint64_t &myKnights =
      (C == Color::WHITE) ? gameCopy.wKnights : gameCopy.bKnights;
  uint64_t &myBishops =
      (C == Color::WHITE) ? gameCopy.wBishops : gameCopy.bBishops;
  uint64_t &myRooks = (C == Color::WHITE) ? gameCopy.wRooks : gameCopy.bRooks;
  uint64_t &myQueens =
      (C == Color::WHITE) ? gameCopy.wQueens : gameCopy.bQueens;
  uint64_t &myKing = (C == Color::WHITE) ? gameCopy.wKing : gameCopy.bKing;

  uint64_t opponentBB = (C == Color::WHITE) ? blacks() : whites();
  uint64_t ownPieces = (C == Color::WHITE) ? whites() : blacks();
  uint64_t emptiesBB = empties();

  auto getCapture = [&](int endSquare, bool ep) -> Move::Piece {
    if (ep) {
      return (C == Color::WHITE) ? Move::B_PAWN : Move::W_PAWN;
    }
    uint64_t mask = 1ULL << endSquare;
    if constexpr (C == Color::WHITE) {
      if (mask & bPawns) {
        return Move::B_PAWN;
      }
      if (mask & bKnights) {
        return Move::B_KNIGHT;
      }
      if (mask & bBishops) {
        return Move::B_BISHOP;
      }
      if (mask & bRooks) {
        return Move::B_ROOK;
      }
      if (mask & bQueens) {
        return Move::B_QUEEN;
      }
      if (mask & bKing) {
        return Move::B_KING;
      }
    } else {
      if (mask & wPawns) {
        return Move::W_PAWN;
      }
      if (mask & wKnights) {
        return Move::W_KNIGHT;
      }
      if (mask & wBishops) {
        return Move::W_BISHOP;
      }
      if (mask & wRooks) {
        return Move::W_ROOK;
      }
      if (mask & wQueens) {
        return Move::W_QUEEN;
      }
      if (mask & wKing) {
        return Move::W_KING;
      }
    }
    return Move::NONE;
  };

  auto forward = [](uint64_t b) constexpr {
    if constexpr (C == Color::WHITE) {
      return up(b);
    } else {
      return down(b);
    }
  };

  auto captureLeft = [](uint64_t b) constexpr {
    if constexpr (C == Color::WHITE) {
      return up_left(b);
    } else {
      return down_left(b);
    }
  };

  auto captureRight = [](uint64_t b) constexpr {
    if constexpr (C == Color::WHITE) {
      return up_right(b);
    } else {
      return down_right(b);
    }
  };

  // Helper that computes check and packs the move in one step.
  auto addMove = [&](int start, int end, bool ep, Move::Piece piece,
                     Move::Promotion promo, Move::Piece capture) {
    Move::Check check =
        computeCheckAfterMove(start, end, ep, piece, promo, capture);
    PackedMove m = makePackedMove(start, end, ep, piece, promo, capture, check);
    Add(moves, m);
  };

  if (checkStatus != Move::DOUBLE_CHECK) {
    // ---------- Pawn moves ----------
    currMoves = forward(forward(myPawns & startRank) & emptiesBB) & emptiesBB;
    while (currMoves) {
      int endIdx = pop_lsb(currMoves);
      int startIdx = endIdx - 2 * forwardStep;
      Move::Piece capture = getCapture(endIdx, false);
      addMove(startIdx, endIdx, false, piecePawn, Move::NA, capture);
    }

    currMoves = forward(myPawns) & emptiesBB;
    if constexpr (C == Color::WHITE) {
      while (currMoves & ~promoRank) {
        int endIdx = pop_lsb(currMoves);
        Move::Piece capture = getCapture(endIdx, false);
        addMove(endIdx - forwardStep, endIdx, false, piecePawn, Move::NA,
                capture);
      }
      while (currMoves) {
        int endIdx = pop_lsb(currMoves);
        for (Move::Promotion p : promotions) {
          Move::Piece capture = getCapture(endIdx, false);
          addMove(endIdx - forwardStep, endIdx, false, piecePawn, p, capture);
        }
      }
    } else {
      while (currMoves & promoRank) {
        int endIdx = pop_lsb(currMoves);
        for (Move::Promotion p : promotions) {
          Move::Piece capture = getCapture(endIdx, false);
          addMove(endIdx - forwardStep, endIdx, false, piecePawn, p, capture);
        }
      }
      while (currMoves) {
        int endIdx = pop_lsb(currMoves);
        Move::Piece capture = getCapture(endIdx, false);
        addMove(endIdx - forwardStep, endIdx, false, piecePawn, Move::NA,
                capture);
      }
    }

    currMoves = captureLeft(myPawns) & (opponentBB | enPassantMask);
    if constexpr (C == Color::WHITE) {
      while (currMoves & ~promoRank) {
        int endIdx = pop_lsb(currMoves);
        int startIdx = endIdx - (forwardStep - 1);
        bool ep = (endIdx == enPassantIdx);
        Move::Piece capture = getCapture(endIdx, ep);
        addMove(startIdx, endIdx, ep, piecePawn, Move::NA, capture);
      }
      while (currMoves) {
        int endIdx = pop_lsb(currMoves);
        for (Move::Promotion p : promotions) {
          Move::Piece capture = getCapture(endIdx, false);
          addMove(endIdx - (forwardStep - 1), endIdx, false, piecePawn, p,
                  capture);
        }
      }
    } else {
      while (currMoves & promoRank) {
        int endIdx = pop_lsb(currMoves);
        for (Move::Promotion p : promotions) {
          Move::Piece capture = getCapture(endIdx, false);
          addMove(endIdx - (forwardStep - 1), endIdx, false, piecePawn, p,
                  capture);
        }
      }
      while (currMoves) {
        int endIdx = pop_lsb(currMoves);
        int startIdx = endIdx - (forwardStep - 1);
        bool ep = (endIdx == enPassantIdx);
        Move::Piece capture = getCapture(endIdx, ep);
        addMove(startIdx, endIdx, ep, piecePawn, Move::NA, capture);
      }
    }

    currMoves = captureRight(myPawns) & (opponentBB | enPassantMask);
    if constexpr (C == Color::WHITE) {
      while (currMoves & ~promoRank) {
        int endIdx = pop_lsb(currMoves);
        int startIdx = endIdx - (forwardStep + 1);
        bool ep = (endIdx == enPassantIdx);
        Move::Piece capture = getCapture(endIdx, ep);
        addMove(startIdx, endIdx, ep, piecePawn, Move::NA, capture);
      }
      while (currMoves) {
        int endIdx = pop_lsb(currMoves);
        for (Move::Promotion p : promotions) {
          Move::Piece capture = getCapture(endIdx, false);
          addMove(endIdx - (forwardStep + 1), endIdx, false, piecePawn, p,
                  capture);
        }
      }
    } else {
      while (currMoves & promoRank) {
        int endIdx = pop_lsb(currMoves);
        for (Move::Promotion p : promotions) {
          Move::Piece capture = getCapture(endIdx, false);
          addMove(endIdx - (forwardStep + 1), endIdx, false, piecePawn, p,
                  capture);
        }
      }
      while (currMoves) {
        int endIdx = pop_lsb(currMoves);
        int startIdx = endIdx - (forwardStep + 1);
        bool ep = (endIdx == enPassantIdx);
        Move::Piece capture = getCapture(endIdx, ep);
        addMove(startIdx, endIdx, ep, piecePawn, Move::NA, capture);
      }
    }

    // ---------- Knights ----------
    while (myKnights) {
      int idx = pop_lsb(myKnights);
      currMoves = KNIGHT_MOVES[idx] & ~ownPieces;
      while (currMoves) {
        int endIdx = pop_lsb(currMoves);
        Move::Piece capture = getCapture(endIdx, false);
        addMove(idx, endIdx, false, pieceKnight, Move::NA, capture);
      }
    }

    // ---------- Bishops ----------
    while (myBishops) {
      int idx = pop_lsb(myBishops);
      currMoves = BISHOP_MOVES[idx][BishopHash(idx, emptiesBB, opponentBB)];
      while (currMoves) {
        int endIdx = pop_lsb(currMoves);
        Move::Piece capture = getCapture(endIdx, false);
        addMove(idx, endIdx, false, pieceBishop, Move::NA, capture);
      }
    }

    // ---------- Rooks ----------
    while (myRooks) {
      int idx = pop_lsb(myRooks);
      currMoves = ROOK_MOVES[idx][RookHash(idx, emptiesBB, opponentBB)];
      while (currMoves) {
        int endIdx = pop_lsb(currMoves);
        Move::Piece capture = getCapture(endIdx, false);
        addMove(idx, endIdx, false, pieceRook, Move::NA, capture);
      }
    }

    // ---------- Queens ----------
    while (myQueens) {
      int idx = pop_lsb(myQueens);
      currMoves = BISHOP_MOVES[idx][BishopHash(idx, emptiesBB, opponentBB)] |
                  ROOK_MOVES[idx][RookHash(idx, emptiesBB, opponentBB)];
      while (currMoves) {
        int endIdx = pop_lsb(currMoves);
        Move::Piece capture = getCapture(endIdx, false);
        addMove(idx, endIdx, false, pieceQueen, Move::NA, capture);
      }
    }
  }

  // ---------- King moves ----------
  int kingIdx = pop_lsb(myKing);
  currMoves = KING_MOVES[kingIdx] & ~ownPieces;
  while (currMoves) {
    int endIdx = pop_lsb(currMoves);
    Move::Piece capture = getCapture(endIdx, false);
    addMove(kingIdx, endIdx, false, pieceKing, Move::NA, capture);
  }

  // ---------- Castling ----------
  if constexpr (C == Color::WHITE) {
    if (wCastle && checkStatus == Move::NO_CHECK &&
        ((emptiesBB & 0x0000000000000060) == 0x0000000000000060) &&
        (InChecks(Color::WHITE, 0x0000000000000020) == Move::NO_CHECK) &&
        (InChecks(Color::WHITE, 0x0000000000000040) == Move::NO_CHECK)) {
      addMove(4, 6, false, pieceKing, Move::NA, Move::NONE);
    }
    if (wQueenCastle && checkStatus == Move::NO_CHECK &&
        ((emptiesBB & 0x000000000000000E) == 0x000000000000000E) &&
        (InChecks(Color::WHITE, 0x0000000000000008) == Move::NO_CHECK) &&
        (InChecks(Color::WHITE, 0x0000000000000004) == Move::NO_CHECK)) {
      addMove(4, 2, false, pieceKing, Move::NA, Move::NONE);
    }
  } else {
    if (bCastle && checkStatus == Move::NO_CHECK &&
        ((emptiesBB & 0x6000000000000000) == 0x6000000000000000) &&
        (InChecks(Color::BLACK, 0x2000000000000000) == Move::NO_CHECK) &&
        (InChecks(Color::BLACK, 0x4000000000000000) == Move::NO_CHECK)) {
      addMove(60, 62, false, pieceKing, Move::NA, Move::NONE);
    }
    if (bQueenCastle && checkStatus == Move::NO_CHECK &&
        ((emptiesBB & 0x0E00000000000000) == 0x0E00000000000000) &&
        (InChecks(Color::BLACK, 0x0800000000000000) == Move::NO_CHECK) &&
        (InChecks(Color::BLACK, 0x0400000000000000) == Move::NO_CHECK)) {
      addMove(60, 58, false, pieceKing, Move::NA, Move::NONE);
    }
  }
}

// Explicit instantiations
template void
Chess::generatePseudoLegalMoves<Color::WHITE>(const Move::Check,
                                              MoveCategories &) const;
template void
Chess::generatePseudoLegalMoves<Color::BLACK>(const Move::Check,
                                              MoveCategories &) const;

// ------------------------------------------------------------------
// Public interfaces
// ------------------------------------------------------------------
MoveCategories Chess::PseudoLegalMoves(const Move::Check checkStatus) {
  MoveCategories moves;
  if (turn == Color::WHITE) {
    generatePseudoLegalMoves<Color::WHITE>(checkStatus, moves);
  } else {
    generatePseudoLegalMoves<Color::BLACK>(checkStatus, moves);
  }
  return moves;
}

thread_local PerftCache perftResults;

uint64_t Chess::perft(int depth, Move::Check checkType) {
  if ((turn == Color::WHITE &&
       InChecks(Color::BLACK, bKing) != Move::NO_CHECK) ||
      (turn != Color::WHITE &&
       InChecks(Color::WHITE, wKing) != Move::NO_CHECK)) {
    return 0;
  }
  if (depth == 0) {
    return 1;
  }

  uint64_t currHash = this->hash;
  uint64_t nodes = 0ULL;
  if (perftResults.get(currHash, depth, nodes)) {
    return nodes;
  }

  MoveCategories pMoves = PseudoLegalMoves(checkType);
  BoardState bs(wCastle, wQueenCastle, bCastle, bQueenCastle, enPassantIdx,
                lastPawnOrTake, fullTurns, this->hash, repTracker.moveCounter,
                repTracker.repetitionStart);

  for (PackedMove &m : pMoves.doubleChecks) {
    MakeMove(m, false);
    nodes += perft(depth - 1, Move::DOUBLE_CHECK);
    UnMakeMove(m, bs, false);
  }
  for (PackedMove &m : pMoves.checks) {
    MakeMove(m, false);
    nodes += perft(depth - 1, Move::CHECK);
    UnMakeMove(m, bs, false);
  }
  for (PackedMove &m : pMoves.captures) {
    MakeMove(m, false);
    nodes += perft(depth - 1, Move::NO_CHECK);
    UnMakeMove(m, bs, false);
  }
  for (PackedMove &m : pMoves.etc) {
    MakeMove(m, false);
    nodes += perft(depth - 1, Move::NO_CHECK);
    UnMakeMove(m, bs, false);
  }
  perftResults.insert(currHash, depth, nodes);
  return nodes;
}

ThreadPool &Chess::getThreadPool() {
  static ThreadPool pool(NUM_THREADS);
  return pool;
}

uint64_t Chess::perftRecurse(int depth, Move::Check checkType) {
  if (NUM_THREADS == 1) {
    return perft(depth, checkType);
  }

  MoveCategories m = PseudoLegalMoves(checkType);
  size_t numMoves = m.numMoves();
  std::vector<PackedMove> allMoves;
  allMoves.reserve(numMoves);
  allMoves.insert(allMoves.end(), m.doubleChecks.begin(), m.doubleChecks.end());
  allMoves.insert(allMoves.end(), m.checks.begin(), m.checks.end());
  allMoves.insert(allMoves.end(), m.captures.begin(), m.captures.end());
  allMoves.insert(allMoves.end(), m.etc.begin(), m.etc.end());

  std::atomic<uint64_t> totalNodes(0);
  std::latch done(NUM_THREADS);
  ThreadPool &pool = getThreadPool();

  for (int i = 0; i < NUM_THREADS; ++i) {
    std::vector<PackedMove> currMoves;
    for (int k = i; k < static_cast<int>(numMoves); k += NUM_THREADS) {
      currMoves.push_back(allMoves[k]);
    }

    pool.enqueue([this, currMoves = std::move(currMoves), depth, checkType,
                  &totalNodes, &done]() {
      Chess currGame(*this);
      uint64_t localNodes = 0;
      for (PackedMove move : currMoves) {
        BoardState bs(currGame.wCastle, currGame.wQueenCastle, currGame.bCastle,
                      currGame.bQueenCastle, currGame.enPassantIdx,
                      currGame.lastPawnOrTake, currGame.fullTurns,
                      currGame.hash, repTracker.moveCounter,
                      repTracker.repetitionStart);
        currGame.MakeMove(move, false);
        localNodes +=
            currGame.perft(depth - 1, static_cast<Move::Check>(move.check));
        currGame.UnMakeMove(move, bs, false);
      }
      totalNodes += localNodes;
      done.count_down();
    });
  }

  done.wait();
  return totalNodes.load();
}
