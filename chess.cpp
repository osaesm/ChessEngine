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
ankerl::unordered_dense::map<int, uint64_t> Chess::BISHOP_MOVES[64] = {};
// Hashed Rook moves
ankerl::unordered_dense::map<int, uint64_t> Chess::ROOK_MOVES[64] = {};
// Hash Queen moves
ankerl::unordered_dense::map<int, uint64_t> Chess::QUEEN_MOVES[64] = {};
char Chess::promotions[4] = {'q', 'r', 'n', 'b'};

// Hard-coded sliding piece moves
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
              for (int upLeft = 0; upLeft <= col && (upLeft + row) < 8; ++upLeft)
              {
                for (int upRight = 0; (upRight + col) < 8 && (upRight + row) < 8; ++upRight)
                {
                  for (int downRight = 0; (downRight + col) < 8 && downRight <= row; ++downRight)
                  {
                    for (int downLeft = 0; downLeft <= col && downLeft <= row; ++downLeft)
                    {
                      int queenHash = (left << 21) + (upLeft << 18) + (up << 15) + (upRight << 12) + (right << 9) + (downRight << 6) + (down << 3) + downLeft;
                      QUEEN_MOVES[idx][queenHash] = 0ULL;
                      for (auto x = idx - (down * 8); x < idx; x += 8)
                      {
                        set_bit(QUEEN_MOVES[idx][queenHash], x);
                      }
                      for (auto x = idx - left; x < idx; ++x)
                      {
                        set_bit(QUEEN_MOVES[idx][queenHash], x);
                      }
                      for (auto x = idx + right; x > idx; --x)
                      {
                        set_bit(QUEEN_MOVES[idx][queenHash], x);
                      }
                      for (auto x = idx + (up * 8); x > idx; x -= 8)
                      {
                        set_bit(QUEEN_MOVES[idx][queenHash], x);
                      }
                      for (auto x = idx - (downRight * 7); x < idx; x += 7)
                      {
                        set_bit(QUEEN_MOVES[idx][queenHash], x);
                      }
                      for (auto x = idx - (downLeft * 9); x < idx; x += 9)
                      {
                        set_bit(QUEEN_MOVES[idx][queenHash], x);
                      }
                      for (auto x = idx + (upLeft * 7); x > idx; x -= 7)
                      {
                        set_bit(QUEEN_MOVES[idx][queenHash], x);
                      }
                      for (auto x = idx + (upRight * 9); x > idx; x -= 9)
                      {
                        set_bit(QUEEN_MOVES[idx][queenHash], x);
                      }
                    }
                  }
                }
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
  for (uint64_t i = 0, idx = 1; i < 64; ++i, idx <<= 1)
  {
    PAWN_TAKES[i][0] = 0ULL;
    PAWN_TAKES[i][0] = up_left(idx) | up_right(idx);
    PAWN_TAKES[i][1] = 0ULL;
    PAWN_TAKES[i][1] = down_left(idx) | down_right(idx);
    KNIGHT_MOVES[i] = 0ULL;
    KNIGHT_MOVES[i] = down(down_left(idx)) | down(down_right(idx)) | left(down_left(idx)) | right(down_right(idx)) | up(up_left(idx)) | up(up_right(idx)) | left(up_left(idx)) | right(up_right(idx));
    KING_MOVES[i] = 0ULL;
    KING_MOVES[i] = down_left(idx) | down(idx) | down_right(idx) | left(idx) | right(idx) | up_left(idx) | up(idx) | up_right(idx);
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

bool Chess::InCheck(const Color kingColor, const uint64_t kingBoard)
{
  uint64_t allPieces = (this->whites() | this->blacks()) ^ (kingColor ? this->wKing : this->bKing);
  uint64_t opponent = kingColor ? this->blacks() : this->whites();
  uint64_t oppRooks = kingColor ? this->bRooks : this->wRooks;
  uint64_t oppBishops = kingColor ? this->bBishops : this->wBishops;
  uint64_t oppKnights = kingColor ? this->bKnights : this->wKnights;
  uint64_t oppQueens = kingColor ? this->bQueens : this->wQueens;
  uint64_t oppPawns = kingColor ? this->bPawns : this->wPawns;
  uint64_t oppKing = kingColor ? this->bKing : this->wKing;

  const uint64_t kingIdx = std::countr_zero(kingBoard);
  return ((BISHOP_MOVES[kingIdx][BishopHash(kingIdx, ~allPieces, opponent)] & (oppBishops | oppQueens))
   || (ROOK_MOVES[kingIdx][RookHash(kingIdx, ~allPieces, opponent)] & (oppRooks | oppQueens))
   || (KNIGHT_MOVES[kingIdx] & oppKnights)
   || (KING_MOVES[kingIdx] & oppKing)
   || (PAWN_TAKES[kingIdx][kingColor ? 0 : 1] & oppPawns));
}

std::vector<Chess *> Chess::LegalMoves()
{
  std::vector<Chess *> legalMoves;
  if (this->turn == Color::WHITE)
  {
    for (auto const &game : this->PseudoLegalMoves())
    {
      if (!game->InCheck(Color::WHITE, game->wKing))
      {
        legalMoves.push_back(game);
      }
    }
  }
  else
  {
    for (auto const &game : this->PseudoLegalMoves())
    {
      if (!game->InCheck(Color::BLACK, game->bKing))
      {
        legalMoves.push_back(game);
      }
    }
  }
  return legalMoves;
}

/**
 * This function:
 * - Checks for normal capture
 * - Then checks for en passant capture
 * - Updates bitboards
 * - Updates enPassantIdx to -1 unless moving a pawn 2 squares
 * - Updates castling rights when moving a rook or king
 * - Updates castling rights when a rook is captured
 * - Resets firstOccurrence / secondOccurrence with irreversible moves (ie pawn move or capture)
 * - Increments fullTurns when black moves
 * - Advances the turns between white/black
 * - Increments lastPawnOrTake unless taking or moving a pawn (lol)
 * - Slots new boardHash into occurrence vectors after executing the move
 */
void Chess::MovePiece(const char pieceType, const int start, const int end, uint64_t opponent)
{
  // If we take (not en passant), set opponent bit to 0 and lastPawnOrTake to 0
  if (get_bit(opponent, end))
  {
    clear_bit(opponent, end);
    this->lastPawnOrTake = 0;
    // Updating the other colors to remove the taken piece
    if (this->turn)
    {
      clear_bit(this->bPawns, end);
      clear_bit(this->bKnights, end);
      clear_bit(this->bBishops, end);
      clear_bit(this->bRooks, end);
      clear_bit(this->bQueens, end);
      // do we need to clear the bKing?
      clear_bit(this->bKing, end);
      // Updating castling rights when taking rooks
      if (this->bCastle && end == 63)
      {
        this->bCastle = false;
      }
      else if (this->bQueenCastle && end == 56)
      {
        this->bQueenCastle = false;
      }
    }
    else
    {
      clear_bit(this->wPawns, end);
      clear_bit(this->wKnights, end);
      clear_bit(this->wBishops, end);
      clear_bit(this->wRooks, end);
      clear_bit(this->wQueens, end);
      // do we need to clear the wKing?
      clear_bit(this->wKing, end);
      // Updating castling rights when taking rooks
      if (this->wCastle && end == 7)
      {
        this->wCastle = false;
      }
      else if (this->wQueenCastle && end == 0)
      {
        this->wQueenCastle = false;
      }
    }
    this->firstOccurrence.clear();
    this->secondOccurrence.clear();
  }
  // Check if en passant capture
  else if (this->enPassantIdx == end)
  {
    if (end > 32)
    {
      clear_bit(this->bPawns, end - 8);
    }
    else
    {
      clear_bit(this->wPawns, end + 8);
    }
    this->firstOccurrence.clear();
    this->secondOccurrence.clear();
  }
  // All non-capture moves
  else
  {
    ++this->lastPawnOrTake;
  }

  // Set en passant to -1
  this->enPassantIdx = -1;

  // Updating bitboards of the piece that's moving
  // Setting lastPawnOrTake to 0 if moving a pawn
  // Updating castling variables when moving rooks / kings
  // Updating enPassantIdx if moving a pawn two squares
  // Resetting firstOccurrence / secondOccurrence with pawn moves, captures, and castling (irreversible)
  switch (pieceType)
  {
  case 'P':
    set_bit(this->wPawns, end);
    clear_bit(this->wPawns, start);
    this->lastPawnOrTake = 0;
    if (end - start == 16)
    {
      this->enPassantIdx = start + 8;
    }
    this->firstOccurrence.clear();
    this->secondOccurrence.clear();
    break;
  case 'N':
    set_bit(this->wKnights, end);
    clear_bit(this->wKnights, start);
    break;
  case 'B':
    set_bit(this->wBishops, end);
    clear_bit(this->wBishops, start);
    break;
  case 'R':
    set_bit(this->wRooks, end);
    clear_bit(this->wRooks, start);
    if (this->wQueenCastle && (start == 0))
    {
      this->firstOccurrence.clear();
      this->secondOccurrence.clear();
      this->wQueenCastle = false;
    }
    else if (this->wCastle && (start == 7))
    {
      this->firstOccurrence.clear();
      this->secondOccurrence.clear();
      this->wCastle = false;
    }
    break;
  case 'Q':
    set_bit(this->wQueens, end);
    clear_bit(this->wQueens, start);
    break;
  case 'K':
    set_bit(this->wKing, end);
    clear_bit(this->wKing, start);
    if (this->wQueenCastle && (start == 4) && (end == 2))
    {
      clear_bit(this->wRooks, 0);
      set_bit(this->wRooks, 3);
    }
    else if (this->wCastle && (start == 4) && (end == 6))
    {
      clear_bit(this->wRooks, 7);
      set_bit(this->wRooks, 5);
    }
    if (this->wCastle || this->wQueenCastle)
    {
      this->wCastle = false;
      this->wQueenCastle = false;
      this->firstOccurrence.clear();
      this->secondOccurrence.clear();
    }
    break;
  case 'p':
    set_bit(this->bPawns, end);
    clear_bit(this->bPawns, start);
    this->lastPawnOrTake = 0;
    if (start - end == 16)
    {
      this->enPassantIdx = start - 8;
    }
    this->firstOccurrence.clear();
    this->secondOccurrence.clear();
    break;
  case 'n':
    set_bit(this->bKnights, end);
    clear_bit(this->bKnights, start);
    break;
  case 'b':
    set_bit(this->bBishops, end);
    clear_bit(this->bBishops, start);
    break;
  case 'r':
    set_bit(this->bRooks, end);
    clear_bit(this->bRooks, start);
    if (this->bQueenCastle && (start == 56))
    {
      this->firstOccurrence.clear();
      this->secondOccurrence.clear();
      this->bQueenCastle = false;
    }
    else if (this->bCastle && (start == 63))
    {
      this->firstOccurrence.clear();
      this->secondOccurrence.clear();
      this->bCastle = false;
    }
    break;
  case 'q':
    set_bit(this->bQueens, end);
    clear_bit(this->bQueens, start);
    break;
  default:
    set_bit(this->bKing, end);
    clear_bit(this->bKing, start);
    if (this->bQueenCastle && (start == 60) && (end == 58))
    {
      clear_bit(this->bRooks, 56);
      set_bit(this->bRooks, 59);
    }
    else if (this->bCastle && (start == 60) && (end == 62))
    {
      clear_bit(this->bRooks, 63);
      set_bit(this->bRooks, 61);
    }
    if (this->bCastle || this->bQueenCastle)
    {
      this->bCastle = false;
      this->bQueenCastle = false;
      this->firstOccurrence.clear();
      this->secondOccurrence.clear();
    }
    break;
  }

  // Incrementing Full Turns
  if (this->turn == Color::BLACK)
  {
    ++this->fullTurns;
  }

  // Advance Turn
  this->turn = (Color) !this->turn;

  std::string boardHash = this->BoardIdx();
  // Updating firstOccurrence / secondOccurrence
  for (const auto &x : this->firstOccurrence)
  {
    if (!boardHash.compare(x))
    {
      for (const auto &y : this->secondOccurrence)
      {
        if (!boardHash.compare(y))
        {
          this->thirdOccurrence = true;
          return;
        }
      }
      this->secondOccurrence.push_back(boardHash);
      return;
    }
  }
  this->firstOccurrence.push_back(boardHash);
  return;
}

/**
 * This function:
 * - Updates the appropriate pawn bitboard
 * - Updates the promotion choice bitboard
 */
void Chess::PromotePawn(const char pieceType, const int end)
{
  if (end > 8)
  {
    // White Pawn
    clear_bit(this->wPawns, end);
    switch (pieceType)
    {
    case 'q':
      set_bit(this->wQueens, end);
      break;
    case 'r':
      set_bit(this->wRooks, end);
      break;
    case 'n':
      set_bit(this->wKnights, end);
      break;
    default:
      set_bit(this->wBishops, end);
      break;
    }
  }
  else
  {
    // Black Pawn
    clear_bit(this->bPawns, end);
    switch (pieceType)
    {
    case 'q':
      set_bit(this->bQueens, end);
      break;
    case 'r':
      set_bit(this->bRooks, end);
      break;
    case 'n':
      set_bit(this->bKnights, end);
      break;
    default:
      set_bit(this->bBishops, end);
      break;
    }
  }
  this->firstOccurrence.clear();
  this->secondOccurrence.clear();
  std::string boardHash = this->BoardIdx();
  this->firstOccurrence.push_back(boardHash);
}

std::vector<Chess *> Chess::PseudoLegalMoves()
{
  std::vector<Chess *> pseudolegalMoves;
  return pseudolegalMoves;
}

uint64_t Chess::perft(int depth)
{
  // Was the last move legal?
  if ((this->turn && this->InCheck(Color::BLACK, this->bKing)) || (!this->turn && this->InCheck(Color::WHITE, this->wKing)))
  {
    return 0;
  }

  // Base Case
  if (depth == 0)
  {
    // std::cout << this->ConvertToFEN() << std::endl;
    return 1;
  }

  if (this->thirdOccurrence) {
    return 0;
  }

  uint64_t nodes = 0ULL;
  uint64_t targets = this->turn ? ~(this->whites()) : ~(this->blacks());
  uint64_t opponent = this->turn ? this->blacks() : this->whites();

  Chess gameCopy(*this);
  int idx = -1;
  uint64_t currMoves = 0ULL;
  uint64_t enPassantMask = this->enPassantIdx == -1 ? 0ULL : (1ULL << this->enPassantIdx);
  if (this->turn)
  {

    // White Pawns
    while (gameCopy.wPawns)
    {
      idx = pop_lsb(gameCopy.wPawns);
      if (idx < 16)
      {
        currMoves = (up(1ULL << idx) & this->empties()) | (PAWN_TAKES[idx][0] & opponent) | (up(up(1ULL << idx) & this->empties()) & this->empties());
      }
      else
      {
        currMoves = (up(1ULL << idx) & this->empties()) | (PAWN_TAKES[idx][0] & (opponent | enPassantMask));
      }
      // Non-promotion moves
      while (currMoves & ~RANK_8)
      {
        Chess nextMoveGame(*this);
        nextMoveGame.MovePiece('P', idx, pop_lsb(currMoves), opponent);
        nodes += nextMoveGame.perft(depth - 1);
      }
      // Promotion moves
      while (currMoves)
      {
        int currEnd = pop_lsb(currMoves);
        for (const char option : promotions)
        {
          Chess nextMoveGame(*this);

          // MovePiece handles most of the stuff correctly, but we need to replace the pawn with whatever the promotion choice is
          nextMoveGame.MovePiece('P', idx, currEnd, opponent);
          nextMoveGame.PromotePawn(option, currEnd);
          nodes += nextMoveGame.perft(depth - 1);
        }
      }
    }

    // White Knights
    while (gameCopy.wKnights)
    {
      idx = pop_lsb(gameCopy.wKnights);
      currMoves = KNIGHT_MOVES[idx] & targets;
      while (currMoves)
      {
        Chess nextMoveGame(*this);
        nextMoveGame.MovePiece('N', idx, pop_lsb(currMoves), opponent);
        nodes += nextMoveGame.perft(depth - 1);
      }
    }

    // White Bishops
    while (gameCopy.wBishops)
    {
      idx = pop_lsb(gameCopy.wBishops);
      currMoves = BISHOP_MOVES[idx][BishopHash(idx, this->empties(), opponent)] & targets;
      while (currMoves)
      {
        Chess nextMoveGame(*this);
        nextMoveGame.MovePiece('B', idx, pop_lsb(currMoves), opponent);
        nodes += nextMoveGame.perft(depth - 1);
      }
    }

    // White Rooks
    while (gameCopy.wRooks)
    {
      idx = pop_lsb(gameCopy.wRooks);
      currMoves = ROOK_MOVES[idx][RookHash(idx, this->empties(), opponent)] & targets;
      while (currMoves)
      {
        Chess nextMoveGame(*this);
        nextMoveGame.MovePiece('R', idx, pop_lsb(currMoves), opponent);
        nodes += nextMoveGame.perft(depth - 1);
      }
    }

    // White Queens
    while (gameCopy.wQueens)
    {
      idx = pop_lsb(gameCopy.wQueens);
      currMoves = QUEEN_MOVES[idx][QueenHash(idx, this->empties(), opponent)] & targets;
      while (currMoves)
      {
        Chess nextMoveGame(*this);
        nextMoveGame.MovePiece('Q', idx, pop_lsb(currMoves), opponent);
        nodes += nextMoveGame.perft(depth - 1);
      }
    }

    // White King
    idx = pop_lsb(gameCopy.wKing);
    currMoves = KING_MOVES[idx] & targets;
    while (currMoves)
    {
      Chess nextMoveGame(*this);
      nextMoveGame.MovePiece('K', idx, pop_lsb(currMoves), opponent);
      nodes += nextMoveGame.perft(depth - 1);
    }

    if (this->wCastle && ((this->empties() & 0x60ULL) == 0x60ULL) && !this->InCheck(Color::WHITE, this->wKing) && !this->InCheck(Color::WHITE, right(this->wKing)) && !this->InCheck(Color::WHITE, right(right(this->wKing))))
    {
      Chess nextMoveGame(*this);
      nextMoveGame.MovePiece('K', 4, 6, opponent);
      nodes += nextMoveGame.perft(depth - 1);
    }
    if (this->wQueenCastle && ((this->empties() & 0xEULL) == 0xEULL) && !this->InCheck(Color::WHITE, this->wKing) && !this->InCheck(Color::WHITE, left(this->wKing)) && !this->InCheck(Color::WHITE, left(left(this->wKing))))
    {
      Chess nextMoveGame(*this);
      nextMoveGame.MovePiece('K', 4, 2, opponent);
      nodes += nextMoveGame.perft(depth - 1);
    }
  }
  else
  {
    // Black Pawns
    while (gameCopy.bPawns)
    {
      idx = pop_lsb(gameCopy.bPawns);
      if (idx >= 48)
      {
        currMoves = (down(1ULL << idx) & this->empties()) | (PAWN_TAKES[idx][1] & opponent) | (down(down(1ULL << idx) & this->empties()) & this->empties());
      }
      else
      {
        currMoves = (down(1ULL << idx) & this->empties()) | (PAWN_TAKES[idx][1] & (opponent | enPassantMask));
      }
      // Promotion moves
      while (currMoves & RANK_1)
      {
        int currEnd = pop_lsb(currMoves);
        for (const char option : promotions)
        {
          Chess nextMoveGame(*this);

          // MovePiece handles most of the stuff correctly, but we need to replace the pawn with whatever the promotion choice is
          nextMoveGame.MovePiece('p', idx, currEnd, opponent);
          nextMoveGame.PromotePawn(option, currEnd);
          nodes += nextMoveGame.perft(depth - 1);
        }
      }
      // Non-promotion moves
      while (currMoves)
      {
        Chess nextMoveGame(*this);
        nextMoveGame.MovePiece('p', idx, pop_lsb(currMoves), opponent);
        nodes += nextMoveGame.perft(depth - 1);
      }
    }

    // Black Knights
    while (gameCopy.bKnights)
    {
      idx = pop_lsb(gameCopy.bKnights);
      currMoves = KNIGHT_MOVES[idx] & targets;
      while (currMoves)
      {
        Chess nextMoveGame(*this);
        nextMoveGame.MovePiece('n', idx, pop_lsb(currMoves), opponent);
        nodes += nextMoveGame.perft(depth - 1);
      }
    }

    // Black Bishops
    while (gameCopy.bBishops)
    {
      idx = pop_lsb(gameCopy.bBishops);
      currMoves = BISHOP_MOVES[idx][BishopHash(idx, this->empties(), opponent)] & targets;
      while (currMoves)
      {
        Chess nextMoveGame(*this);
        nextMoveGame.MovePiece('b', idx, pop_lsb(currMoves), opponent);
        nodes += nextMoveGame.perft(depth - 1);
      }
    }

    // Black Rooks
    while (gameCopy.bRooks)
    {
      idx = pop_lsb(gameCopy.bRooks);
      currMoves = ROOK_MOVES[idx][RookHash(idx, this->empties(), opponent)] & targets;
      while (currMoves)
      {
        Chess nextMoveGame(*this);
        nextMoveGame.MovePiece('r', idx, pop_lsb(currMoves), opponent);
        nodes += nextMoveGame.perft(depth - 1);
      }
    }

    // Black Queens
    while (gameCopy.bQueens)
    {
      idx = pop_lsb(gameCopy.bQueens);
      currMoves = QUEEN_MOVES[idx][QueenHash(idx, this->empties(), opponent)] & targets;
      while (currMoves)
      {
        Chess nextMoveGame(*this);
        nextMoveGame.MovePiece('q', idx, pop_lsb(currMoves), opponent);
        nodes += nextMoveGame.perft(depth - 1);
      }
    }

    // Black King
    idx = pop_lsb(gameCopy.bKing);
    currMoves = KING_MOVES[idx] & targets;
    while (currMoves)
    {
      Chess nextMoveGame(*this);
      nextMoveGame.MovePiece('k', idx, pop_lsb(currMoves), opponent);
      nodes += nextMoveGame.perft(depth - 1);
    }

    if (this->bCastle && ((this->empties() & 0x6000000000000000ULL) == 0x6000000000000000ULL) && !this->InCheck(Color::BLACK, this->bKing) && !this->InCheck(Color::BLACK, right(this->bKing)) && !this->InCheck(Color::BLACK, right(right(this->bKing))))
    {
      Chess nextMoveGame(*this);
      nextMoveGame.MovePiece('k', 60, 62, opponent);
      nodes += nextMoveGame.perft(depth - 1);
    }
    if (this->bQueenCastle && ((this->empties() & 0xE00000000000000ULL) == 0xE00000000000000ULL) && !this->InCheck(Color::BLACK, this->bKing) && !this->InCheck(Color::BLACK, left(this->bKing)) && !this->InCheck(Color::BLACK, left(left(this->bKing))))
    {
      Chess nextMoveGame(*this);
      nextMoveGame.MovePiece('k', 60, 58, opponent);
      nodes += nextMoveGame.perft(depth - 1);
    }
  }
  return nodes;
}