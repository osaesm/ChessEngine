#include "chess.h"
#include <iostream>
#include <bitset>

// 0: white forward, 1: white takes, 2: black forward, 3: black takes
uint64_t Chess::PAWN_MOVES[64][4] = {};
uint64_t Chess::KNIGHT_MOVES[64] = {};
uint64_t Chess::KING_MOVES[64] = {};
ankerl::unordered_dense::map<int, uint64_t> Chess::BISHOP_MOVES[64] = {};
ankerl::unordered_dense::map<int, uint64_t> Chess::ROOK_MOVES[64] = {};
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
              ROOK_MOVES[idx][hash] = 0;
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
                      int queenHash = (left << 21) + (upLeft << 18) + (up << 15) + (upRight << 12) + (right << 9) + (downRight << 6) + (down << 3) + downRight;
                      QUEEN_MOVES[idx][queenHash] = 0;
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
            for (int downLeft = 0; downLeft <= row; ++downLeft)
            {
              int hash = (upLeft << 9) + (upRight << 6) + (downRight << 3) + downLeft;
              BISHOP_MOVES[idx][hash] = 0;
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
    PAWN_MOVES[i][0] = 0;
    if (i >= 8 && i < 16)
    {
      PAWN_MOVES[i][0] = up(idx) | up(up(idx));
    }
    else
    {
      PAWN_MOVES[i][0] = up(idx);
    }
    PAWN_MOVES[i][1] = 0;
    PAWN_MOVES[i][1] = up_left(idx) | up_right(idx);
    PAWN_MOVES[i][2] = 0;
    if (i >= 48 && i < 56)
    {
      PAWN_MOVES[i][2] = down(idx) | down(down(idx));
    }
    else
    {
      PAWN_MOVES[i][2] = down(idx);
    }
    PAWN_MOVES[i][3] = 0;
    PAWN_MOVES[i][3] = down_left(idx) | down_right(idx);
    KNIGHT_MOVES[i] = 0;
    KNIGHT_MOVES[i] = down(down_left(idx)) | down(down_right(idx)) | left(down_left(idx)) | right(down_right(idx)) | up(up_left(idx)) | up(up_right(idx)) | left(up_left(idx)) | right(up_right(idx));
    KING_MOVES[i] = 0;
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
  this->wPawns = 0;
  this->bPawns = 0;
  this->wKnights = 0;
  this->bKnights = 0;
  this->wBishops = 0;
  this->bBishops = 0;
  this->wRooks = 0;
  this->bRooks = 0;
  this->wQueens = 0;
  this->bQueens = 0;
  this->wKing = 0;
  this->bKing = 0;
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
  // Creating copy to pop pieces off of
  Chess *copy = new Chess(*this);

  // First Part
  char board[64];
  for (auto x = 0; x < 64; ++x)
  {
    board[x] = ' ';
  }
  // Iterate over each piece
  // White Pawns
  while (copy->wPawns)
  {
    int idx = pop_lsb(copy->wPawns);
    board[idx] = 'P';
  }
  // Black Pawns
  while (copy->bPawns)
  {
    int idx = pop_lsb(copy->bPawns);
    board[idx] = 'p';
  }
  // White Knights
  while (copy->wKnights)
  {
    int idx = pop_lsb(copy->wKnights);
    board[idx] = 'N';
  }
  // Black Knights
  while (copy->bKnights)
  {
    int idx = pop_lsb(copy->bKnights);
    board[idx] = 'n';
  }
  // White Bishops
  while (copy->wBishops)
  {
    int idx = pop_lsb(copy->wBishops);
    board[idx] = 'B';
  }
  // Black Bishops
  while (copy->bBishops)
  {
    int idx = pop_lsb(copy->bBishops);
    board[idx] = 'b';
  }
  // White Rooks
  while (copy->wRooks)
  {
    int idx = pop_lsb(copy->wRooks);
    board[idx] = 'R';
  }
  // Black Rooks
  while (copy->bRooks)
  {
    int idx = pop_lsb(copy->bRooks);
    board[idx] = 'r';
  }
  // White Queens
  while (copy->wQueens)
  {
    int idx = pop_lsb(copy->wQueens);
    board[idx] = 'Q';
  }
  // Black Queens
  while (copy->bQueens)
  {
    int idx = pop_lsb(copy->bQueens);
    board[idx] = 'q';
  }
  // White King
  while (copy->wKing)
  {
    int idx = pop_lsb(copy->wKing);
    board[idx] = 'K';
  }
  // Black King
  while (copy->bKing)
  {
    int idx = pop_lsb(copy->bKing);
    board[idx] = 'k';
  }

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
    boardHash += "K";
  if (this->wQueenCastle)
    boardHash += "Q";
  if (this->bCastle)
    boardHash += "k";
  if (this->bQueenCastle)
    boardHash += "q";
  if (!this->wCastle && !this->wQueenCastle && !this->bCastle && !this->bQueenCastle)
    boardHash += "-";

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
      boardHash += "b" + std::to_string((this->enPassantIdx / 8) + 1);
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
  // uint64_t allPieces = (this->whites() | this->blacks()) ^ (kingColor == Color::WHITE ? this->wKing : this->bKing);
  // uint64_t opponent = (kingColor == Color::WHITE) ? this->blacks() : this->whites();
  // uint64_t oppRooks = (kingColor == Color::WHITE) ? this->bRooks : this->wRooks;
  // uint64_t oppBishops = (kingColor == Color::WHITE) ? this->bBishops : this->wBishops;
  // uint64_t oppKnights = (kingColor == Color::WHITE) ? this->bKnights : this->wKnights;
  // uint64_t oppQueens = (kingColor == Color::WHITE) ? this->bQueens : this->wQueens;
  // uint64_t oppPawns = (kingColor == Color::WHITE) ? this->bPawns : this->wPawns;
  // uint64_t oppKing = (kingColor == Color::WHITE) ? this->bKing : this->wKing;

  // const uint64_t kingIdx = (kingColor == Color::WHITE) ? get_lsb(this->wKing) : get_lsb(this->bKing);
  // for (uint64_t idx = kingIdx - 9, iMask = down_left(kingBoard); iMask; idx -= 9, iMask = down_left(iMask))
  // {
  //   if (get_bit(oppBishops, idx) || get_bit(oppQueens, idx) || ((idx == (kingIdx - 9)) && (kingColor == Color::BLACK) && get_bit(oppPawns, idx)))
  //   {
  //     return true;
  //   }
  //   if (get_bit(allPieces, idx))
  //   {
  //     break;
  //   }
  // }
  // for (uint64_t idx = kingIdx - 8, iMask = down(kingBoard); iMask; idx -= 8, iMask = down(iMask))
  // {
  //   if (get_bit(oppRooks, idx) || get_bit(oppQueens, idx))
  //   {
  //     return true;
  //   }
  //   if (get_bit(allPieces, idx))
  //   {
  //     break;
  //   }
  // }

  // for (uint64_t idx = kingIdx - 7, iMask = down_right(kingBoard); iMask; idx -= 7, iMask = down_right(iMask))
  // {
  //   if (idx == kingIdx - 7 && kingColor == Color::BLACK)
  //   {
  //     std::cout << get_bit(oppPawns, idx) << std::endl;
  //   }
  //   if (get_bit(oppBishops, idx) || get_bit(oppQueens, idx) || ((idx == (kingIdx - 7)) && (kingColor == Color::BLACK) && get_bit(oppPawns, idx)))
  //   {
  //     return true;
  //   }
  //   if (get_bit(allPieces, idx))
  //   {
  //     break;
  //   }
  // }

  // for (uint64_t idx = kingIdx - 1, iMask = left(kingBoard); iMask; idx -= 1, iMask = left(iMask))
  // {
  //   if (get_bit(oppRooks, idx) || get_bit(oppQueens, idx))
  //   {
  //     return true;
  //   }
  //   if (get_bit(allPieces, idx))
  //   {
  //     break;
  //   }
  // }

  // for (uint64_t idx = kingIdx + 1, iMask = right(kingBoard); iMask; idx += 1, iMask = right(iMask))
  // {
  //   if (get_bit(oppRooks, idx) || get_bit(oppQueens, idx))
  //   {
  //     return true;
  //   }
  //   if (get_bit(allPieces, idx))
  //   {
  //     break;
  //   }
  // }

  // for (uint64_t idx = kingIdx + 7, iMask = up_left(kingBoard); iMask; idx += 7, iMask = up_left(iMask))
  // {
  //   if (get_bit(oppBishops, idx) || get_bit(oppQueens, idx) || (kingColor == Color::WHITE && get_bit(oppPawns, idx)))
  //   {
  //     return true;
  //   }
  //   if (get_bit(allPieces, idx))
  //   {
  //     break;
  //   }
  // }

  // for (uint64_t idx = kingIdx + 8, iMask = up(kingBoard); iMask; idx += 8, iMask = up(iMask))
  // {
  //   if (get_bit(oppRooks, idx) || get_bit(oppQueens, idx))
  //   {
  //     return true;
  //   }
  //   if (get_bit(allPieces, idx))
  //   {
  //     break;
  //   }
  // }

  // for (uint64_t idx = kingIdx + 9, iMask = up_right(kingBoard); iMask; idx += 9, iMask = up_right(iMask))
  // {
  //   if (get_bit(oppBishops, idx) || get_bit(oppQueens, idx) || (kingColor == Color::WHITE && get_bit(oppPawns, idx)))
  //   {
  //     return true;
  //   }
  //   if (get_bit(allPieces, idx))
  //   {
  //     break;
  //   }
  // }
  // return (KNIGHT_MOVES[kingIdx] & oppKnights) ? true : false;
  return false;
}

std::vector<Chess *> Chess::LegalMoves()
{
  std::vector<Chess *> legalMoves;
  if (this->turn == Color::WHITE)
  {
    for (auto const &game : this->PseudoLegalMoves())
    {
      if (!game->InCheck(Color::WHITE, get_lsb(game->wKing)))
      {
        legalMoves.push_back(game);
      }
    }
  }
  else
  {
    for (auto const &game : this->PseudoLegalMoves())
    {
      if (!game->InCheck(Color::BLACK, get_lsb(game->bKing)))
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
 * TODO:
 * - Figure out castling
 * - Figure out pawn upgrades
 */
void Chess::MovePiece(const char pieceType, const int start, const int end, uint64_t opponent)
{
  // If we take (not en passant), set opponent bit to 0 and lastPawnOrTake to 0
  if (get_bit(opponent, end))
  {
    clear_bit(opponent, end);
    this->lastPawnOrTake = 0;
    // Updating the other colors to remove the taken piece
    if (this->turn == Color::WHITE)
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
  }
  // If enPassantIdx == -1, we check if that's the current move, and regardless we set the enPassantIdx to -1.
  else if (this->enPassantIdx != -1)
  {
    // Check if en passant capture
    if (this->enPassantIdx == end)
    {
      if (pieceType == 'P')
      {
        clear_bit(this->bPawns, end - 8);
      }
      else
      {
        clear_bit(this->wPawns, end + 8);
      }
    }
    // Set en passant to -1
    this->enPassantIdx = -1;
  }
  // All non-capture moves
  else
  {
    ++this->lastPawnOrTake;
  }

  // Updating bitboards of the piece that's moving
  // Setting lastPawnOrTake to 0 if moving a pawn
  // Updating castling variables when moving rooks / kings
  // Updating enPassantIdx if moving a pawn two squares
  // Resetting firstOccurrence / secondOccurrence with pawn moves (irreversible)
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
    firstOccurrence.clear();
    secondOccurrence.clear();
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
      this->wQueenCastle = false;
    }
    else if (this->wCastle && (start == 7))
    {
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
    if (this->wQueenCastle)
    {
      this->wQueenCastle = false;
    }
    if (this->wCastle)
    {
      this->wCastle = false;
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
    firstOccurrence.clear();
    secondOccurrence.clear();
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
      this->bQueenCastle = false;
    }
    else if (this->bCastle)
    {
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
    if (this->bQueenCastle)
    {
      this->bQueenCastle = false;
    }
    if (this->bCastle)
    {
      this->bCastle = false;
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
}

std::vector<Chess *> Chess::PseudoLegalMoves()
{
  std::vector<Chess *> pseudolegalMoves;
  return pseudolegalMoves;
}

uint64_t Chess::perft(int depth)
{
  // Base Case
  if (depth == 0)
  {
    // std::cout << this->ConvertToFEN() << std::endl;
    if (this->turn)
    {
      // Did Black's move leave them in check?
      return this->InCheck(Color::BLACK, this->bKing) ? 0 : 1;
    }
    // Did White's move leave them in check?
    return this->InCheck(Color::WHITE, this->wKing) ? 0 : 1;
  }

  uint64_t nodes = 0ULL;
  uint64_t targets = this->turn ? ~(this->whites()) : ~(this->blacks());
  uint64_t opponent = this->turn ? this->blacks() : this->whites();

  Chess gameCopy(*this);
  int idx = -1;
  uint64_t currMoves = 0;
  if (this->turn)
  {

    // White Pawns
    while (gameCopy.wPawns)
    {
      idx = pop_lsb(gameCopy.wPawns);
      currMoves = (PAWN_MOVES[idx][0] & this->empties()) | (PAWN_MOVES[idx][1] & (opponent | (1ULL << this->enPassantIdx)));
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
      currMoves = BISHOP_MOVES[idx][BishopHash(idx, targets)];
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
      currMoves = ROOK_MOVES[idx][RookHash(idx, targets)];
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
      currMoves = QUEEN_MOVES[idx][QueenHash(idx, targets)];
      while (currMoves)
      {
        Chess nextMoveGame(*this);
        nextMoveGame.MovePiece('Q', idx, pop_lsb(currMoves), opponent);
        nodes += nextMoveGame.perft(depth - 1);
      }
    }

    // White King
    currMoves = KING_MOVES[idx] & targets;
    while (currMoves)
    {
      Chess nextMoveGame(*this);
      nextMoveGame.MovePiece('K', idx, pop_lsb(currMoves), opponent);
      nodes += nextMoveGame.perft(depth - 1);
    }
  }
  else
  {
  }
  return nodes;
}