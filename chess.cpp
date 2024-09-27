#include "chess.hpp"
#include <iostream>
#include <bitset>

// 0: white takes, 1: black takes
// Does not include two forward special case
uint64_t Chess::PAWN_TAKES[64][2] = {};
// Presaved knight moves
uint64_t Chess::KNIGHT_MOVES[64] = {};
// Presaved king moves
uint64_t Chess::KING_MOVES[64] = {};
// Hashed Bishop moves
uint64_t Chess::BISHOP_MOVES[64][4096] = {};
// Hashed Rook moves
uint64_t Chess::ROOK_MOVES[64][4096] = {};
// Hash Queen moves
// uint64_t Chess::QUEEN_MOVES[64][4096 * 4096] = {};
Move::Promotion Chess::promotions[4] = {Move::Promotion::QUEEN, Move::Promotion::ROOK, Move::Promotion::KNIGHT, Move::Promotion::BISHOP};

void Add(MoveCategories &mC, Move &m)
{
  if (m.checkType == Move::Check::DOUBLE_CHECK)
  {
    mC.doubleChecks.push_back(m);
  }
  else if (m.checkType == Move::Check::CHECK)
  {
    mC.checks.push_back(m);
  }
  else if (m.captureType != Move::Piece::NONE)
  {
    mC.captures.push_back(m);
  }
  else
  {
    mC.etc.push_back(m);
  }
}

// Hard-coded piece moves
void Chess::Initialize()
{
  // Initialize the sliding piece static boards
  for (auto row = 0; row < 8; ++row)
  {
    for (auto col = 0; col < 8; ++col)
    {
      auto idx = (row * 8) + col;
      for (int left = 0; left <= col; ++left)
      {
        for (int up = 0; (row + up) < 8; ++up)
        {
          for (int right = 0; (right + col) < 8; ++right)
          {
            for (int down = 0; down <= row; ++down)
            {
              int hash = (left << 9) + (up << 6) + (right << 3) + down;
              ROOK_MOVES[idx][hash] = 0ULL;
              for (auto x = idx - (down * 8); x < idx; x += 8)
              {
                set_bit(ROOK_MOVES[idx][hash], x);
              }
              for (auto x = idx - left; x < idx; ++x)
              {
                set_bit(ROOK_MOVES[idx][hash], x);
              }
              for (auto x = idx + right; x > idx; --x)
              {
                set_bit(ROOK_MOVES[idx][hash], x);
              }
              for (auto x = idx + (up * 8); x > idx; x -= 8)
              {
                set_bit(ROOK_MOVES[idx][hash], x);
              }
            }
          }
        }
      }
      for (int upLeft = 0; upLeft <= col && (upLeft + row) < 8; ++upLeft)
      {
        for (int upRight = 0; (upRight + col) < 8 && (row + upRight) < 8; ++upRight)
        {
          for (int downRight = 0; (downRight + col) < 8 && downRight <= row; ++downRight)
          {
            for (int downLeft = 0; downLeft <= col && downLeft <= row; ++downLeft)
            {
              int hash = (upLeft << 9) + (upRight << 6) + (downRight << 3) + downLeft;
              BISHOP_MOVES[idx][hash] = 0ULL;
              for (auto x = idx - (downRight * 7); x < idx; x += 7)
              {
                set_bit(BISHOP_MOVES[idx][hash], x);
              }
              for (auto x = idx - (downLeft * 9); x < idx; x += 9)
              {
                set_bit(BISHOP_MOVES[idx][hash], x);
              }
              for (auto x = idx + (upLeft * 7); x > idx; x -= 7)
              {
                set_bit(BISHOP_MOVES[idx][hash], x);
              }
              for (auto x = idx + (upRight * 9); x > idx; x -= 9)
              {
                set_bit(BISHOP_MOVES[idx][hash], x);
              }
            }
          }
        }
      }
    }
  }
  // Initialize pawn, knight, king moves
  for (uint64_t i = 0ULL, iMask = 1ULL; i < 64; ++i, iMask <<= 1)
  {
    PAWN_TAKES[i][0] = 0ULL;
    PAWN_TAKES[i][0] = up_left(iMask) | up_right(iMask);
    PAWN_TAKES[i][1] = 0ULL;
    PAWN_TAKES[i][1] = down_left(iMask) | down_right(iMask);
    KNIGHT_MOVES[i] = 0ULL;
    KNIGHT_MOVES[i] = down(down_left(iMask)) | down(down_right(iMask)) | left(down_left(iMask)) | right(down_right(iMask)) | up(up_left(iMask)) | up(up_right(iMask)) | left(up_left(iMask)) | right(up_right(iMask));
    KING_MOVES[i] = 0ULL;
    KING_MOVES[i] = down_left(iMask) | down(iMask) | down_right(iMask) | left(iMask) | right(iMask) | up_left(iMask) | up(iMask) | up_right(iMask);
  }
}

Chess::Chess(const std::string &fenString)
{
  this->firstOccurrence.clear();
  this->secondOccurrence.clear();
  this->thirdOccurrence = false;
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
  while (fenString[idx] != ' ')
  {
    switch (fenString[idx])
    {
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
  if (fenString[idx] == 'K')
  {
    this->wCastle = true;
    ++idx;
  }
  if (fenString[idx] == 'Q')
  {
    this->wQueenCastle = true;
    ++idx;
  }
  if (fenString[idx] == 'k')
  {
    this->bCastle = true;
    ++idx;
  }
  if (fenString[idx] == 'q')
  {
    this->bQueenCastle = true;
    ++idx;
  }
  if (!this->wCastle && !this->wQueenCastle && !this->bCastle && !this->bQueenCastle)
    ++idx;
  // Fourth Part
  ++idx;
  this->enPassantIdx = -1;
  if (fenString[idx] != '-')
  {
    this->enPassantIdx = (fenString[idx] - 'a');
    ++idx;
    this->enPassantIdx += (fenString[idx] == '3') ? 16 : 40;
  }

  // Fifth Part
  idx += 2;
  this->lastPawnOrTake = 0;
  while (fenString[idx] != ' ')
  {
    this->lastPawnOrTake *= 10;
    this->lastPawnOrTake += (fenString[idx] - '0');
    ++idx;
  }

  // Sixth Part
  ++idx;
  this->fullTurns = 0;
  while (idx < fenString.length() && fenString[idx] != ' ')
  {
    this->fullTurns *= 10;
    this->fullTurns += (fenString[idx] - '0');
    ++idx;
  }
}

Chess::Chess(const Chess &x)
{
  // Bitboards
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

  // Turn
  this->turn = x.turn;

  // Castling
  this->wCastle = x.wCastle;
  this->wQueenCastle = x.wQueenCastle;
  this->bCastle = x.bCastle;
  this->bQueenCastle = x.bQueenCastle;

  // En Passant
  this->enPassantIdx = x.enPassantIdx;

  // Last Pawn or Take
  this->lastPawnOrTake = x.lastPawnOrTake;

  // Full Turns
  this->fullTurns = x.fullTurns;

  // First/Second/Third Occurrence
  this->firstOccurrence.clear();
  for (const auto &k : x.firstOccurrence)
  {
    this->firstOccurrence.push_back(k);
  }
  this->secondOccurrence.clear();
  for (const auto &k : x.secondOccurrence)
  {
    this->secondOccurrence.push_back(k);
  }
  this->thirdOccurrence = x.thirdOccurrence;
}

std::string Chess::BoardIdx()
{
  // Creating gameCopy to pop pieces off of
  Chess *gameCopy = new Chess(*this);

  // First Part
  char board[64];
  for (auto x = 0; x < 64; ++x)
  {
    board[x] = ' ';
  }
  // Iterate over each piece
  // White Pawns
  while (gameCopy->wPawns)
  {
    board[pop_lsb(gameCopy->wPawns)] = 'P';
  }
  // Black Pawns
  while (gameCopy->bPawns)
  {
    board[pop_lsb(gameCopy->bPawns)] = 'p';
  }
  // White Knights
  while (gameCopy->wKnights)
  {
    board[pop_lsb(gameCopy->wKnights)] = 'N';
  }
  // Black Knights
  while (gameCopy->bKnights)
  {
    board[pop_lsb(gameCopy->bKnights)] = 'n';
  }
  // White Bishops
  while (gameCopy->wBishops)
  {
    board[pop_lsb(gameCopy->wBishops)] = 'B';
  }
  // Black Bishops
  while (gameCopy->bBishops)
  {
    board[pop_lsb(gameCopy->bBishops)] = 'b';
  }
  // White Rooks
  while (gameCopy->wRooks)
  {
    board[pop_lsb(gameCopy->wRooks)] = 'R';
  }
  // Black Rooks
  while (gameCopy->bRooks)
  {
    board[pop_lsb(gameCopy->bRooks)] = 'r';
  }
  // White Queens
  while (gameCopy->wQueens)
  {
    board[pop_lsb(gameCopy->wQueens)] = 'Q';
  }
  // Black Queens
  while (gameCopy->bQueens)
  {
    board[pop_lsb(gameCopy->bQueens)] = 'q';
  }
  // White King
  while (gameCopy->wKing)
  {
    board[pop_lsb(gameCopy->wKing)] = 'K';
  }
  // Black King
  while (gameCopy->bKing)
  {
    board[pop_lsb(gameCopy->bKing)] = 'k';
  }

  delete gameCopy;

  std::string boardHash = "";
  for (int rowStart = 56; rowStart >= 0; rowStart -= 8)
  {
    int numSpaces = 0;
    for (int idx = rowStart; idx < (rowStart + 8); ++idx)
    {
      if (board[idx] != ' ')
      {
        if (numSpaces != 0)
        {
          boardHash += std::to_string(numSpaces);
        }
        boardHash += board[idx];
        numSpaces = 0;
      }
      else
      {
        ++numSpaces;
      }
    }
    if (numSpaces != 0)
    {
      boardHash += std::to_string(numSpaces);
    }
    if (rowStart > 0)
    {
      boardHash += "/";
    }
  }

  // Second Part
  if (this->turn == Color::WHITE)
  {
    boardHash += " w ";
  }
  else
  {
    boardHash += " b ";
  }

  // Third Part
  if (this->wCastle)
  {
    boardHash += "K";
  }
  if (this->wQueenCastle)
  {
    boardHash += "Q";
  }
  if (this->bCastle)
  {
    boardHash += "k";
  }
  if (this->bQueenCastle)
  {
    boardHash += "q";
  }
  else if (!this->wCastle && !this->wQueenCastle && !this->bCastle && !this->bQueenCastle)
  {
    boardHash += "-";
  }

  // Fourth Part
  if (this->enPassantIdx == -1)
  {
    boardHash += " -";
  }
  else
  {
    switch (this->enPassantIdx % 8)
    {
    case 0:
      boardHash += " a" + std::to_string((this->enPassantIdx / 8) + 1);
      break;
    case 1:
      boardHash += " b" + std::to_string((this->enPassantIdx / 8) + 1);
      break;
    case 2:
      boardHash += " c" + std::to_string((this->enPassantIdx / 8) + 1);
      break;
    case 3:
      boardHash += " d" + std::to_string((this->enPassantIdx / 8) + 1);
      break;
    case 4:
      boardHash += " e" + std::to_string((this->enPassantIdx / 8) + 1);
      break;
    case 5:
      boardHash += " f" + std::to_string((this->enPassantIdx / 8) + 1);
      break;
    case 6:
      boardHash += " g" + std::to_string((this->enPassantIdx / 8) + 1);
      break;
    default:
      boardHash += " h" + std::to_string((this->enPassantIdx / 8) + 1);
      break;
    }
  }
  return boardHash;
}

std::string Chess::ConvertToFEN()
{
  return this->BoardIdx() + " " + std::to_string(this->lastPawnOrTake) + " " + std::to_string(this->fullTurns);
}

Move::Check Chess::InChecks(const Color kingColor, const uint64_t kingBoard)
{
  uint64_t allPieces = (this->whites() | this->blacks() | kingBoard) & ~(kingColor ? this->wKing : this->bKing);
  uint64_t opponent = kingColor ? this->blacks() : this->whites();
  uint64_t oppRooks = kingColor ? this->bRooks : this->wRooks;
  uint64_t oppBishops = kingColor ? this->bBishops : this->wBishops;
  uint64_t oppKnights = kingColor ? this->bKnights : this->wKnights;
  uint64_t oppQueens = kingColor ? this->bQueens : this->wQueens;
  uint64_t oppPawns = kingColor ? this->bPawns : this->wPawns;
  uint64_t oppKing = kingColor ? this->bKing : this->wKing;

  const uint64_t kingIdx = std::countr_zero(kingBoard);
  uint64_t checkMasks[5] = {
      (BISHOP_MOVES[kingIdx][BishopHash(kingIdx, ~allPieces, opponent)] & (oppBishops | oppQueens)),
      (ROOK_MOVES[kingIdx][RookHash(kingIdx, ~allPieces, opponent)] & (oppRooks | oppQueens)),
      (KNIGHT_MOVES[kingIdx] & oppKnights),
      (KING_MOVES[kingIdx] & oppKing),
      (PAWN_TAKES[kingIdx][kingColor ? 0 : 1] & oppPawns)};
  Move::Check checkType = Move::Check::NO_CHECK;
  for (int i = 0; i < 5; ++i)
  {
    while (checkMasks[i])
    {
      pop_lsb(checkMasks[i]);
      if (checkType == Move::Check::NO_CHECK)
      {
        checkType = Move::Check::CHECK;
      }
      else if (checkType == Move::Check::CHECK)
      {
        return Move::Check::DOUBLE_CHECK;
      }
    }
  }
  return checkType;
}

void Chess::MakeMove(Move &m)
{
  this->enPassantIdx = -1;
  if (!this->turn)
  {
    ++this->fullTurns;
  }
  ++this->lastPawnOrTake;
  this->turn = (Color)(!this->turn);
  // Clear start square and fill end square
  // Promote Pawns
  // Castle
  // Update castling rights when moving
  // Updating enPassantIdx when pushing a pawn 2 squares
  switch (m.pieceType)
  {
  case Move::Piece::W_PAWN:
    clear_bit(this->wPawns, m.start);
    switch (m.promotionType)
    {
    case Move::Promotion::QUEEN:
      set_bit(this->wQueens, m.end);
      break;
    case Move::Promotion::ROOK:
      set_bit(this->wRooks, m.end);
      break;
    case Move::Promotion::KNIGHT:
      set_bit(this->wKnights, m.end);
      break;
    case Move::Promotion::BISHOP:
      set_bit(this->wBishops, m.end);
      break;
    case Move::Promotion::NA:
    default:
      if (m.end - m.start == 16)
      {
        this->enPassantIdx = m.start + 8;
      }
      set_bit(this->wPawns, m.end);
      break;
    }
    this->firstOccurrence.clear();
    this->secondOccurrence.clear();
    this->lastPawnOrTake = 0;
    break;
  case Move::Piece::W_KNIGHT:
    clear_bit(this->wKnights, m.start);
    set_bit(this->wKnights, m.end);
    break;
  case Move::Piece::W_BISHOP:
    clear_bit(this->wBishops, m.start);
    set_bit(this->wBishops, m.end);
    break;
  case Move::Piece::W_ROOK:
    clear_bit(this->wRooks, m.start);
    set_bit(this->wRooks, m.end);
    // Castling rights
    if (this->wCastle && m.start == 7)
    {
      this->wCastle = false;
      this->firstOccurrence.clear();
      this->secondOccurrence.clear();
    }
    else if (this->wQueenCastle && m.start == 0)
    {
      this->wQueenCastle = false;
      this->firstOccurrence.clear();
      this->secondOccurrence.clear();
    }
    break;
  case Move::Piece::W_QUEEN:
    clear_bit(this->wQueens, m.start);
    set_bit(this->wQueens, m.end);
    break;
  case Move::Piece::W_KING:
    clear_bit(this->wKing, m.start);
    set_bit(this->wKing, m.end);
    if (this->wCastle)
    {
      this->wCastle = false;
      this->firstOccurrence.clear();
      this->secondOccurrence.clear();
    }
    if (this->wQueenCastle)
    {
      this->wQueenCastle = false;
      this->firstOccurrence.clear();
      this->secondOccurrence.clear();
    }
    if (m.start == 4 && m.end == 6)
    {
      clear_bit(this->wRooks, 7);
      set_bit(this->wRooks, 5);
    }
    else if (m.start == 4 && m.end == 2)
    {
      clear_bit(this->wRooks, 0);
      set_bit(this->wRooks, 3);
    }
    break;
  case Move::Piece::B_PAWN:
    clear_bit(this->bPawns, m.start);
    switch (m.promotionType)
    {
    case Move::Promotion::QUEEN:
      set_bit(this->bQueens, m.end);
      break;
    case Move::Promotion::ROOK:
      set_bit(this->bRooks, m.end);
      break;
    case Move::Promotion::KNIGHT:
      set_bit(this->bKnights, m.end);
      break;
    case Move::Promotion::BISHOP:
      set_bit(this->bBishops, m.end);
      break;
    case Move::Promotion::NA:
    default:
      if (m.start - m.end == 16)
      {
        this->enPassantIdx = m.start - 8;
      }
      set_bit(this->bPawns, m.end);
      break;
    }
    this->firstOccurrence.clear();
    this->secondOccurrence.clear();
    this->lastPawnOrTake = 0;
    break;
  case Move::Piece::B_KNIGHT:
    clear_bit(this->bKnights, m.start);
    set_bit(this->bKnights, m.end);
    break;
  case Move::Piece::B_BISHOP:
    clear_bit(this->bBishops, m.start);
    set_bit(this->bBishops, m.end);
    break;
  case Move::Piece::B_ROOK:
    clear_bit(this->bRooks, m.start);
    set_bit(this->bRooks, m.end);
    if (this->bCastle && m.start == 63)
    {
      this->bCastle = false;
      this->firstOccurrence.clear();
      this->secondOccurrence.clear();
    }
    else if (this->bQueenCastle && m.start == 56)
    {
      this->bQueenCastle = false;
      this->firstOccurrence.clear();
      this->secondOccurrence.clear();
    }
    break;
  case Move::Piece::B_QUEEN:
    clear_bit(this->bQueens, m.start);
    set_bit(this->bQueens, m.end);
    break;
  case Move::Piece::B_KING:
    clear_bit(this->bKing, m.start);
    set_bit(this->bKing, m.end);
    if (this->bCastle)
    {
      this->bCastle = false;
      this->firstOccurrence.clear();
      this->secondOccurrence.clear();
    }
    if (this->bQueenCastle)
    {
      this->bQueenCastle = false;
      this->firstOccurrence.clear();
      this->secondOccurrence.clear();
    }
    if (m.start == 60 && m.end == 62)
    {
      clear_bit(this->bRooks, 63);
      set_bit(this->bRooks, 61);
    }
    else if (m.start == 60 && m.end == 58)
    {
      clear_bit(this->bRooks, 56);
      set_bit(this->bRooks, 59);
    }
    break;
  default:
    break;
  }

  // Empty captured squares
  // Update castling rights when taking rooks
  if (m.enPassant)
  {
    if (m.pieceType == Move::Piece::W_PAWN)
    {
      clear_bit(this->bPawns, m.end - 8);
    }
    else
    {
      clear_bit(this->wPawns, m.end + 8);
    }
  }
  else if (m.pieceType < Move::Piece::B_PAWN)
  {
    if (get_bit(this->bPawns, m.end))
    {
      m.captureType = Move::Piece::B_PAWN;
      clear_bit(this->bPawns, m.end);
    }
    else if (get_bit(this->bKnights, m.end))
    {
      m.captureType = Move::Piece::B_KNIGHT;
      clear_bit(this->bKnights, m.end);
    }
    else if (get_bit(this->bBishops, m.end))
    {
      m.captureType = Move::Piece::B_BISHOP;
      clear_bit(this->bBishops, m.end);
    }
    else if (get_bit(this->bRooks, m.end))
    {
      m.captureType = Move::Piece::B_ROOK;
      clear_bit(this->bRooks, m.end);
      if (m.end == 56 && this->bQueenCastle)
      {
        this->bQueenCastle = false;
      }
      else if (m.end == 63 && this->bCastle)
      {
        this->bCastle = false;
      }
    }
    else if (get_bit(this->bQueens, m.end))
    {
      m.captureType = Move::Piece::B_QUEEN;
      clear_bit(this->bQueens, m.end);
    }
    else if (get_bit(this->bKing, m.end))
    {
      m.captureType = Move::Piece::B_KING;
      clear_bit(this->bKing, m.end);
    }
  }
  else
  {
    if (get_bit(this->wPawns, m.end))
    {
      m.captureType = Move::Piece::W_PAWN;
      clear_bit(this->wPawns, m.end);
    }
    else if (get_bit(this->wKnights, m.end))
    {
      m.captureType = Move::Piece::W_KNIGHT;
      clear_bit(this->wKnights, m.end);
    }
    else if (get_bit(this->wBishops, m.end))
    {
      m.captureType = Move::Piece::W_BISHOP;
      clear_bit(this->wBishops, m.end);
    }
    else if (get_bit(this->wRooks, m.end))
    {
      m.captureType = Move::Piece::W_ROOK;
      clear_bit(this->wRooks, m.end);
      if (m.end == 0 && this->wQueenCastle)
      {
        this->wQueenCastle = false;
      }
      else if (m.end == 7 && this->wCastle)
      {
        this->wCastle = false;
      }
    }
    else if (get_bit(this->wQueens, m.end))
    {
      m.captureType = Move::Piece::W_QUEEN;
      clear_bit(this->wQueens, m.end);
    }
    else if (get_bit(this->wKing, m.end))
    {
      m.captureType = Move::Piece::W_KING;
      clear_bit(this->wKing, m.end);
    }
  }
  if (m.pieceType < Move::Piece::B_PAWN)
  {
    m.checkType = this->InChecks(Color::BLACK, this->bKing);
  }
  else
  {
    m.checkType = this->InChecks(Color::WHITE, this->wKing);
  }
}

void Chess::UnMakeMove(const Move &m, const BoardState &bs)
{
  // Move the piece back
  switch (m.pieceType)
  {
  case Move::Piece::W_PAWN:
    switch (m.promotionType)
    {
    case Move::Promotion::QUEEN:
      clear_bit(this->wQueens, m.end);
      break;
    case Move::Promotion::ROOK:
      clear_bit(this->wRooks, m.end);
      break;
    case Move::Promotion::KNIGHT:
      clear_bit(this->wKnights, m.end);
      break;
    case Move::Promotion::BISHOP:
      clear_bit(this->wBishops, m.end);
      break;
    case Move::Promotion::NA:
    default:
      clear_bit(this->wPawns, m.end);
      break;
    }
    set_bit(this->wPawns, m.start);
    break;
  case Move::Piece::W_KNIGHT:
    clear_bit(this->wKnights, m.end);
    set_bit(this->wKnights, m.start);
    break;
  case Move::Piece::W_BISHOP:
    clear_bit(this->wBishops, m.end);
    set_bit(this->wBishops, m.start);
    break;
  case Move::Piece::W_ROOK:
    clear_bit(this->wRooks, m.end);
    set_bit(this->wRooks, m.start);
    break;
  case Move::Piece::W_QUEEN:
    clear_bit(this->wQueens, m.end);
    set_bit(this->wQueens, m.start);
    break;
  case Move::Piece::W_KING:
    clear_bit(this->wKing, m.end);
    set_bit(this->wKing, m.start);
    if (m.start == 4 && m.end == 6)
    {
      clear_bit(this->wRooks, 5);
      set_bit(this->wRooks, 7);
    }
    else if (m.start == 4 && m.end == 2)
    {
      clear_bit(this->wRooks, 3);
      set_bit(this->wRooks, 0);
    }
    break;
  case Move::Piece::B_PAWN:
    switch (m.promotionType)
    {
    case Move::Promotion::QUEEN:
      clear_bit(this->bQueens, m.end);
      break;
    case Move::Promotion::ROOK:
      clear_bit(this->bRooks, m.end);
      break;
    case Move::Promotion::KNIGHT:
      clear_bit(this->bKnights, m.end);
      break;
    case Move::Promotion::BISHOP:
      clear_bit(this->bBishops, m.end);
      break;
    case Move::Promotion::NA:
    default:
      clear_bit(this->bPawns, m.end);
      break;
    }
    set_bit(this->bPawns, m.start);
    break;
  case Move::Piece::B_KNIGHT:
    clear_bit(this->bKnights, m.end);
    set_bit(this->bKnights, m.start);
    break;
  case Move::Piece::B_BISHOP:
    clear_bit(this->bBishops, m.end);
    set_bit(this->bBishops, m.start);
    break;
  case Move::Piece::B_ROOK:
    clear_bit(this->bRooks, m.end);
    set_bit(this->bRooks, m.start);
    break;
  case Move::Piece::B_QUEEN:
    clear_bit(this->bQueens, m.end);
    set_bit(this->bQueens, m.start);
    break;
  case Move::Piece::B_KING:
    clear_bit(this->bKing, m.end);
    set_bit(this->bKing, m.start);
    if (m.start == 60 && m.end == 62)
    {
      clear_bit(this->bRooks, 61);
      set_bit(this->bRooks, 63);
    }
    else if (m.start == 60 && m.end == 58)
    {
      clear_bit(this->bRooks, 59);
      set_bit(this->bRooks, 56);
    }
    break;
  }
  // Undo the capture
  switch (m.captureType)
  {
  case Move::Piece::W_PAWN:
    if (m.enPassant)
    {
      set_bit(this->wPawns, m.end + 8);
    }
    else
    {
      set_bit(this->wPawns, m.end);
    }
    break;
  case Move::Piece::W_KNIGHT:
    set_bit(this->wKnights, m.end);
    break;
  case Move::Piece::W_BISHOP:
    set_bit(this->wBishops, m.end);
    break;
  case Move::Piece::W_ROOK:
    set_bit(this->wRooks, m.end);
    break;
  case Move::Piece::W_QUEEN:
    set_bit(this->wQueens, m.end);
    break;
  case Move::Piece::W_KING:
    set_bit(this->wKing, m.end);
    break;
  case Move::Piece::B_PAWN:
    if (m.enPassant)
    {
      set_bit(this->bPawns, m.end - 8);
    }
    else
    {
      set_bit(this->bPawns, m.end);
    }
    break;
  case Move::Piece::B_KNIGHT:
    set_bit(this->bKnights, m.end);
    break;
  case Move::Piece::B_BISHOP:
    set_bit(this->bBishops, m.end);
    break;
  case Move::Piece::B_ROOK:
    set_bit(this->bRooks, m.end);
    break;
  case Move::Piece::B_QUEEN:
    set_bit(this->bQueens, m.end);
    break;
  case Move::Piece::B_KING:
    set_bit(this->bKing, m.end);
    break;
  }
  // Reset the Board State (Castling rights, en passant index, etc.)
  this->turn = (Color)(!this->turn);
  this->wCastle = bs.wCastle;
  this->wQueenCastle = bs.wQueenCastle;
  this->bCastle = bs.bCastle;
  this->bQueenCastle = bs.bQueenCastle;
  this->enPassantIdx = bs.enPassantIdx;
  this->lastPawnOrTake = bs.lastPawnOrTake;
  this->fullTurns = bs.fullTurns;
  this->firstOccurrence = bs.firstOccurrence;
  this->secondOccurrence = bs.secondOccurrence;
  this->thirdOccurrence = bs.thirdOccurrence;
}

MoveCategories Chess::PseudoLegalMoves(const Move::Check checkStatus)
{
  MoveCategories moves;
  Chess gameCopy(*this);
  uint64_t currMoves = 0ULL;
  uint64_t enPassantMask = this->enPassantIdx == -1 ? 0ULL : (1ULL << this->enPassantIdx);
  const BoardState bs(this->wCastle, this->wQueenCastle, this->bCastle, this->bQueenCastle, this->enPassantIdx, this->lastPawnOrTake, this->fullTurns, this->firstOccurrence, this->secondOccurrence, this->thirdOccurrence);
  if (this->turn)
  {
    if (checkStatus != Move::Check::DOUBLE_CHECK)
    {
      // Advance white pawn two squares
      currMoves = up(up(this->wPawns & RANK_2) & this->empties()) & this->empties();
      while (currMoves)
      {
        int endIdx = pop_lsb(currMoves);
        Move m(endIdx - 16, endIdx, false, Move::Piece::W_PAWN, Move::Promotion::NA);
        this->MakeMove(m);
        this->UnMakeMove(m, bs);
        // IDK if double check is possible here
        Add(moves, m);
      }
      // Advance white pawn 1 square
      currMoves = up(this->wPawns) & this->empties();
      while (currMoves & ~RANK_8)
      {
        int endIdx = pop_lsb(currMoves);
        Move m(endIdx - 8, endIdx, false, Move::Piece::W_PAWN, Move::Promotion::NA);
        this->MakeMove(m);
        this->UnMakeMove(m, bs);
        Add(moves, m);
      }
      while (currMoves)
      {
        int endIdx = pop_lsb(currMoves);
        for (Move::Promotion p : promotions)
        {
          Move m(endIdx - 8, endIdx, false, Move::Piece::W_PAWN, p);
          this->MakeMove(m);
          this->UnMakeMove(m, bs);
          Add(moves, m);
        }
      }
      // Take with white pawn on the left
      currMoves = up_left(this->wPawns) & (this->blacks() | enPassantMask);
      while (currMoves & ~RANK_8)
      {
        int endIdx = pop_lsb(currMoves);
        Move m(endIdx - 7, endIdx, endIdx == this->enPassantIdx, Move::Piece::W_PAWN, Move::Promotion::NA);
        this->MakeMove(m);
        this->UnMakeMove(m, bs);
        Add(moves, m);
      }
      while (currMoves)
      {
        int endIdx = pop_lsb(currMoves);
        for (Move::Promotion p : promotions)
        {
          Move m(endIdx - 7, endIdx, false, Move::Piece::W_PAWN, p);
          this->MakeMove(m);
          this->UnMakeMove(m, bs);
          Add(moves, m);
        }
      }
      // Take with white pawn on the right
      currMoves = up_right(this->wPawns) & (this->blacks() | enPassantMask);
      while (currMoves & ~RANK_8)
      {
        int endIdx = pop_lsb(currMoves);
        Move m(endIdx - 9, endIdx, endIdx == this->enPassantIdx, Move::Piece::W_PAWN, Move::Promotion::NA);
        this->MakeMove(m);
        this->UnMakeMove(m, bs);
        Add(moves, m);
      }
      while (currMoves)
      {
        int endIdx = pop_lsb(currMoves);
        for (Move::Promotion p : promotions)
        {
          Move m(endIdx - 9, endIdx, false, Move::Piece::W_PAWN, p);
          this->MakeMove(m);
          this->UnMakeMove(m, bs);
          Add(moves, m);
        }
      }

      // White Knights
      while (gameCopy.wKnights)
      {
        int idx = pop_lsb(gameCopy.wKnights);
        currMoves = KNIGHT_MOVES[idx] & (~this->whites());
        while (currMoves)
        {
          int endIdx = pop_lsb(currMoves);
          Move m(idx, endIdx, false, Move::Piece::W_KNIGHT, Move::Promotion::NA);
          this->MakeMove(m);
          this->UnMakeMove(m, bs);
          Add(moves, m);
        }
      }

      // White Bishops
      while (gameCopy.wBishops)
      {
        int idx = pop_lsb(gameCopy.wBishops);
        currMoves = BISHOP_MOVES[idx][BishopHash(idx, this->empties(), this->blacks())];
        while (currMoves)
        {
          int endIdx = pop_lsb(currMoves);
          Move m(idx, endIdx, false, Move::Piece::W_BISHOP, Move::Promotion::NA);
          this->MakeMove(m);
          this->UnMakeMove(m, bs);
          Add(moves, m);
        }
      }

      // White Rooks
      while (gameCopy.wRooks)
      {
        int idx = pop_lsb(gameCopy.wRooks);
        currMoves = ROOK_MOVES[idx][RookHash(idx, this->empties(), this->blacks())];
        while (currMoves)
        {
          int endIdx = pop_lsb(currMoves);
          Move m(idx, endIdx, false, Move::Piece::W_ROOK, Move::Promotion::NA);
          this->MakeMove(m);
          this->UnMakeMove(m, bs);
          Add(moves, m);
        }
      }

      // White Queens
      while (gameCopy.wQueens)
      {
        int idx = pop_lsb(gameCopy.wQueens);
        currMoves = BISHOP_MOVES[idx][BishopHash(idx, this->empties(), this->blacks())] | ROOK_MOVES[idx][RookHash(idx, this->empties(), this->blacks())];
        while (currMoves)
        {
          int endIdx = pop_lsb(currMoves);
          Move m(idx, endIdx, false, Move::Piece::W_QUEEN, Move::Promotion::NA);
          this->MakeMove(m);
          this->UnMakeMove(m, bs);
          Add(moves, m);
        }
      }
    }

    // White King
    int idx = pop_lsb(gameCopy.wKing);
    currMoves = KING_MOVES[idx] & (~this->whites());
    while (currMoves)
    {
      int endIdx = pop_lsb(currMoves);
      Move m(idx, endIdx, false, Move::Piece::W_KING, Move::Promotion::NA);
      this->MakeMove(m);
      this->UnMakeMove(m, bs);
      Add(moves, m);
    }
    if (this->wCastle && (checkStatus == Move::Check::NO_CHECK) && ((this->empties() & 0x0000000000000060) == 0x0000000000000060) && (this->InChecks(Color::WHITE, 0x0000000000000020) == Move::Check::NO_CHECK))
    {
      Move m(4, 6, false, Move::Piece::W_KING, Move::Promotion::NA);
      this->MakeMove(m);
      this->UnMakeMove(m, bs);
      Add(moves, m);
    }
    if (this->wQueenCastle && (checkStatus == Move::Check::NO_CHECK) && ((this->empties() & 0x000000000000000E) == 0x000000000000000E) && (this->InChecks(Color::WHITE, 0x0000000000000008) == Move::Check::NO_CHECK))
    {
      Move m(4, 2, false, Move::Piece::W_KING, Move::Promotion::NA);
      this->MakeMove(m);
      this->UnMakeMove(m, bs);
      Add(moves, m);
    }
  }
  else
  {
    if (checkStatus != Move::Check::DOUBLE_CHECK)
    {
      // Advance black pawn two squares
      currMoves = down(down(this->bPawns & RANK_7) & this->empties()) & this->empties();
      while (currMoves)
      {
        int endIdx = pop_lsb(currMoves);
        Move m(endIdx + 16, endIdx, false, Move::Piece::B_PAWN, Move::Promotion::NA);
        this->MakeMove(m);
        this->UnMakeMove(m, bs);
        // IDK if double check is possible here
        Add(moves, m);
      }
      // Advance black pawn 1 square
      currMoves = down(this->bPawns) & this->empties();
      while (currMoves & RANK_1)
      {
        int endIdx = pop_lsb(currMoves);
        for (Move::Promotion p : promotions)
        {
          Move m(endIdx + 8, endIdx, false, Move::Piece::B_PAWN, p);
          this->MakeMove(m);
          this->UnMakeMove(m, bs);
          Add(moves, m);
        }
      }
      while (currMoves)
      {
        int endIdx = pop_lsb(currMoves);
        Move m(endIdx + 8, endIdx, false, Move::Piece::B_PAWN, Move::Promotion::NA);
        this->MakeMove(m);
        this->UnMakeMove(m, bs);
        Add(moves, m);
      }
      // Take with black pawn on the right
      currMoves = down_right(this->bPawns) & (this->whites() | enPassantMask);
      while (currMoves & RANK_1)
      {
        int endIdx = pop_lsb(currMoves);
        for (Move::Promotion p : promotions)
        {
          Move m(endIdx + 7, endIdx, false, Move::Piece::B_PAWN, p);
          this->MakeMove(m);
          this->UnMakeMove(m, bs);
          Add(moves, m);
        }
      }
      while (currMoves)
      {
        int endIdx = pop_lsb(currMoves);
        Move m(endIdx + 7, endIdx, endIdx == this->enPassantIdx, Move::Piece::B_PAWN, Move::Promotion::NA);
        this->MakeMove(m);
        this->UnMakeMove(m, bs);
        Add(moves, m);
      }
      // Take with black pawn on the left
      currMoves = down_left(this->bPawns) & (this->whites() | enPassantMask);
      while (currMoves & RANK_1)
      {
        int endIdx = pop_lsb(currMoves);
        for (Move::Promotion p : promotions)
        {
          Move m(endIdx + 9, endIdx, false, Move::Piece::B_PAWN, p);
          this->MakeMove(m);
          this->UnMakeMove(m, bs);
          Add(moves, m);
        }
      }
      while (currMoves)
      {
        int endIdx = pop_lsb(currMoves);
        Move m(endIdx + 9, endIdx, endIdx == this->enPassantIdx, Move::Piece::B_PAWN, Move::Promotion::NA);
        this->MakeMove(m);
        this->UnMakeMove(m, bs);
        Add(moves, m);
      }

      // Black Knights
      while (gameCopy.bKnights)
      {
        int idx = pop_lsb(gameCopy.bKnights);
        currMoves = KNIGHT_MOVES[idx] & (~this->blacks());
        while (currMoves)
        {
          int endIdx = pop_lsb(currMoves);
          Move m(idx, endIdx, false, Move::Piece::B_KNIGHT, Move::Promotion::NA);
          this->MakeMove(m);
          this->UnMakeMove(m, bs);
          Add(moves, m);
        }
      }

      // Black Bishops
      while (gameCopy.bBishops)
      {
        int idx = pop_lsb(gameCopy.bBishops);
        currMoves = BISHOP_MOVES[idx][BishopHash(idx, this->empties(), this->whites())];
        while (currMoves)
        {
          int endIdx = pop_lsb(currMoves);
          Move m(idx, endIdx, false, Move::Piece::B_BISHOP, Move::Promotion::NA);
          this->MakeMove(m);
          this->UnMakeMove(m, bs);
          Add(moves, m);
        }
      }

      // Black Rooks
      while (gameCopy.bRooks)
      {
        int idx = pop_lsb(gameCopy.bRooks);
        currMoves = ROOK_MOVES[idx][RookHash(idx, this->empties(), this->whites())];
        while (currMoves)
        {
          int endIdx = pop_lsb(currMoves);
          Move m(idx, endIdx, false, Move::Piece::B_ROOK, Move::Promotion::NA);
          this->MakeMove(m);
          this->UnMakeMove(m, bs);
          Add(moves, m);
        }
      }

      // Black Queens
      while (gameCopy.bQueens)
      {
        int idx = pop_lsb(gameCopy.bQueens);
        currMoves = BISHOP_MOVES[idx][BishopHash(idx, this->empties(), this->whites())] | ROOK_MOVES[idx][RookHash(idx, this->empties(), this->whites())];
        while (currMoves)
        {
          int endIdx = pop_lsb(currMoves);
          Move m(idx, endIdx, false, Move::Piece::B_QUEEN, Move::Promotion::NA);
          this->MakeMove(m);
          this->UnMakeMove(m, bs);
          Add(moves, m);
        }
      }
    }

    // Black King
    int idx = pop_lsb(gameCopy.bKing);
    currMoves = KING_MOVES[idx] & (~this->blacks());
    while (currMoves)
    {
      int endIdx = pop_lsb(currMoves);
      Move m(idx, endIdx, false, Move::Piece::B_KING, Move::Promotion::NA);
      this->MakeMove(m);
      this->UnMakeMove(m, bs);
      Add(moves, m);
    }
    if (this->bCastle && (checkStatus == Move::Check::NO_CHECK) && ((this->empties() & 0x6000000000000000) == 0x6000000000000000) && (this->InChecks(Color::BLACK, 0x2000000000000000) == Move::Check::NO_CHECK))
    {
      Move m(60, 62, false, Move::Piece::B_KING, Move::Promotion::NA);
      this->MakeMove(m);
      this->UnMakeMove(m, bs);
      Add(moves, m);
    }
    if (this->bQueenCastle && (checkStatus == Move::Check::NO_CHECK) && ((this->empties() & 0x0E00000000000000) == 0x0E00000000000000) && (this->InChecks(Color::BLACK, 0x0800000000000000) == Move::Check::NO_CHECK))
    {
      Move m(60, 58, false, Move::Piece::B_KING, Move::Promotion::NA);
      this->MakeMove(m);
      this->UnMakeMove(m, bs);
      Add(moves, m);
    }
  }
  return moves;
}

// New Idea:
// Perft:
// * Check depth & legality etc like right now
// * Call this->PseudoLegalMoves() to get std::tuple<std::vector<Move>, std::vector<Move>, std::vector<Move>, std::vector<Move> >
// * Each iteration goes:
//   * this->MakeMove(m); this->perft(depth - 1); this->unMakeMove(m, bs);
// * Iterate over double checks first
// * Iterate over checks next
// * Then iterate over captures (fewer pieces -> fewer nodes later)
// * Then iterate over all other moves

uint64_t Chess::perft(int depth, Move::Check checkType)
{
  // Was the last move legal?
  if ((this->turn && (this->InChecks(Color::BLACK, this->bKing) != Move::Check::NO_CHECK)) || (!this->turn && (this->InChecks(Color::WHITE, this->wKing) != Move::Check::NO_CHECK)))
  {
    return 0;
  }

  // Base Case
  if (depth == 0)
  {
    // std::cout << this->ConvertToFEN() << std::endl;
    return 1;
  }

  if (this->thirdOccurrence)
  {
    return 0;
  }

  MoveCategories pMoves = this->PseudoLegalMoves(checkType);
  uint64_t nodes = 0ULL;
  const BoardState bs(this->wCastle, this->wQueenCastle, this->bCastle, this->bQueenCastle, this->enPassantIdx, this->lastPawnOrTake, this->fullTurns, this->firstOccurrence, this->secondOccurrence, this->thirdOccurrence);
  for (Move &m : pMoves.doubleChecks)
  {
    this->MakeMove(m);
    nodes += this->perft(depth - 1, Move::DOUBLE_CHECK);
    this->UnMakeMove(m, bs);
  }
  for (Move &m : pMoves.checks)
  {
    this->MakeMove(m);
    nodes += this->perft(depth - 1, Move::CHECK);
    this->UnMakeMove(m, bs);
  }
  for (Move &m : pMoves.captures)
  {
    this->MakeMove(m);
    nodes += this->perft(depth - 1, Move::NO_CHECK);
    this->UnMakeMove(m, bs);
  }
  for (Move &m : pMoves.etc)
  {
    this->MakeMove(m);
    nodes += this->perft(depth - 1, Move::NO_CHECK);
    this->UnMakeMove(m, bs);
  }

  return nodes;
}