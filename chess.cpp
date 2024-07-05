#include "chess.h"

Chess::Chess(const std::string &fenString)
{
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
  this->turn = (fenString[idx] == 'w') ? Chess::Color::WHITE : Chess::Color::BLACK;

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
  this->wCastle = x.wCastle;
  this->wQueenCastle = x.wQueenCastle;
  this->bCastle = x.bCastle;
  this->bQueenCastle = x.bQueenCastle;
  this->enPassantIdx = x.enPassantIdx;
  this->lastPawnOrTake = x.lastPawnOrTake;
  this->fullTurns = x.fullTurns;
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
  if (this->turn == Chess::Color::WHITE)
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

bool Chess::InCheck()
{
  return true;
}

std::vector<Chess *> Chess::LegalMoves()
{
  std::vector<Chess *> legalMoves;
  for (auto const &game : this->PseudoLegalMoves())
  {
    if (!game->InCheck())
    {
      legalMoves.push_back(game);
    }
  }
  return legalMoves;
}

std::vector<Chess *> Chess::PseudoLegalMoves()
{
  std::vector<Chess *> pseudolegalMoves;
  return pseudolegalMoves;
}

uint64_t Chess::perft(int depth)
{
  return 0ULL;
}

void Chess::divide(int depth)
{
  return;
}