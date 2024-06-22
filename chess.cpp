#include "chess.h"
#include <iostream>

Chess::Chess(const std::string &fenString)
{
  std::string boardOccurrence = "";
  for (auto idx = 0; idx < 64; ++idx)
  {
    this->pieces[idx] = nullptr;
  }
  // First part of FEN string is the board representation
  // 0-based index
  int currRow = 7;
  int currCol = 0;
  int idx = 0;
  for (idx = 0; idx < fenString.length(); ++idx)
  {
    const char &c = fenString[idx];
    if (c == ' ')
    {
      break;
    }
    if (c == '/')
    {
      --currRow;
      currCol = 0;
      continue;
    }
    if (c > '0' && c < '9')
    {
      currCol += (c - '0');
      continue;
    }
    Piece *newPiece = new Piece();
    if ((c > 'A') && (c < 'Z'))
    {
      newPiece->color = Piece::Color::WHITE;
    }
    else
    {
      newPiece->color = Piece::Color::BLACK;
    }
    switch (c)
    {
    case 'P':
    case 'p':
      newPiece->type = Piece::Type::PAWN;
      break;
    case 'N':
    case 'n':
      newPiece->type = Piece::Type::KNIGHT;
      break;
    case 'B':
    case 'b':
      newPiece->type = Piece::Type::BISHOP;
      break;
    case 'R':
    case 'r':
      newPiece->type = Piece::Type::ROOK;
      break;
    case 'Q':
    case 'q':
      newPiece->type = Piece::Type::QUEEN;
      break;
    default:
      newPiece->type = Piece::Type::KING;
      break;
    }
    pieces[(currRow * 8) + currCol] = newPiece;
    ++currCol;
  }
  idx++;

  // Second part: parsing the turn
  this->turn = (fenString[idx] == 'w') ? Piece::Color::WHITE : Piece::Color::BLACK;

  // Third part: castling rights
  this->wCastle = false;
  this->wQueenCastle = false;
  this->bCastle = false;
  this->bQueenCastle = false;
  idx += 2;
  while (fenString[idx] != ' ')
  {
    if (!this->wCastle && (fenString[idx] == 'K'))
    {
      this->wCastle = true;
    }
    else if (!this->wQueenCastle && (fenString[idx] == 'Q'))
    {
      this->wQueenCastle = true;
    }
    else if (!this->bCastle && (fenString[idx] == 'k'))
    {
      this->bCastle = true;
    }
    else if (!this->bQueenCastle && (fenString[idx] == 'q'))
    {
      this->bQueenCastle = true;
    }
    ++idx;
  }

  // fourth part: en passant
  ++idx;
  if (fenString[idx] == '-')
  {
    enPassantIdx = -1;
    boardOccurrence += "-";
  }
  else
  {
    enPassantIdx = (fenString[idx] - 'a');
    ++idx;
    enPassantIdx += ((fenString[idx] - '1') * 8);
  }
  ++idx;
  this->occurrences[fenString.substr(idx)] = 1;

  // fifth part: half turns
  ++idx;
  this->lastPawnOrTake = 0;
  while (fenString[idx] != ' ')
  {
    this->lastPawnOrTake = (this->lastPawnOrTake * 10) + (fenString[idx] - '0');
    ++idx;
  }

  // sixth part: full turns;
  ++idx;
  this->fullTurns = 0;
  while (idx < fenString.length())
  {
    this->fullTurns = (this->fullTurns * 10) + (fenString[idx] - '0');
    ++idx;
  }
}

// KEEPS THE SAME POINTERS TO PIECES (saves memory)
Chess::Chess(const Chess &originalGame)
{
  for (auto x = 0; x < 64; ++x)
  {
    this->pieces[x] = originalGame.pieces[x];
  }
  for (const auto &elem : originalGame.occurrences)
  {
    this->occurrences[elem.first] = elem.second;
  }
  this->turn = originalGame.turn;
  this->wCastle = originalGame.wCastle;
  this->wQueenCastle = originalGame.wQueenCastle;
  this->bCastle = originalGame.bCastle;
  this->bQueenCastle = originalGame.bQueenCastle;
  this->enPassantIdx = originalGame.enPassantIdx;
  this->lastPawnOrTake = originalGame.lastPawnOrTake;
  this->fullTurns = originalGame.fullTurns;
}

void Chess::ClearPieces()
{
  for (auto idx = 0; idx < 64; ++idx)
  {
    if (this->pieces[idx])
      delete this->pieces[idx];
  }
}

// Sees if other color has king in check from kingIdx
bool Chess::InCheck(const short kingIdx, const Piece::Color kingColor)
{
  short startRow = kingIdx / 8;
  short startCol = kingIdx % 8;
  bool searchDown = true;
  // -9
  for (short x = 1, newIdx = kingIdx - 9; (x <= startRow) && (x <= startCol); ++x, newIdx -= 9)
  {
    if (this->pieces[newIdx])
    {
      if (this->pieces[newIdx]->color == kingColor)
      {
        if (this->pieces[newIdx]->type == Piece::Type::KING)
        {
          continue;
        }
        break;
      }
      else if ((this->pieces[newIdx]->type == Piece::Type::BISHOP) || (this->pieces[newIdx]->type == Piece::Type::QUEEN) || (x == 1 && ((kingColor == Piece::Color::BLACK && (this->pieces[newIdx]->type == Piece::Type::PAWN)) || (this->pieces[newIdx]->type == Piece::Type::KING))))
      {
        return true;
      }
      break;
    }
  }
  // -8
  for (short newIdx = kingIdx - 8; newIdx >= 0; newIdx -= 8)
  {
    if (this->pieces[newIdx])
    {
      if (this->pieces[newIdx]->color == kingColor)
      {
        if (this->pieces[newIdx]->type == Piece::Type::KING)
        {
          continue;
        }
        break;
      }
      else if ((this->pieces[newIdx]->type == Piece::Type::ROOK) || (this->pieces[newIdx]->type == Piece::Type::QUEEN) || (newIdx == (kingIdx - 8) && this->pieces[newIdx]->type == Piece::Type::KING))
      {
        return true;
      }
      break;
    }
  }
  // -7
  for (short x = 1, newIdx = kingIdx - 7; (x <= startRow) && ((x + startCol) < 8); ++x, newIdx -= 7)
  {
    if (this->pieces[newIdx])
    {
      if (this->pieces[newIdx]->color == kingColor)
      {
        if (this->pieces[newIdx]->type == Piece::Type::KING)
        {
          continue;
        }
        break;
      }
      else if ((this->pieces[newIdx]->type == Piece::Type::BISHOP) || (this->pieces[newIdx]->type == Piece::Type::QUEEN) || (x == 1 && ((kingColor == Piece::Color::BLACK && (this->pieces[newIdx]->type == Piece::Type::PAWN)) || (this->pieces[newIdx]->type == Piece::Type::KING))))
      {
        return true;
      }
      break;
    }
  }
  // -1
  for (short x = 1, newIdx = kingIdx - 1; x <= startCol; ++x, --newIdx)
  {
    if (this->pieces[newIdx])
    {
      if (this->pieces[newIdx]->color == kingColor)
      {
        if (this->pieces[newIdx]->type == Piece::Type::KING)
        {
          continue;
        }
        break;
      }
      else if ((this->pieces[newIdx]->type == Piece::Type::ROOK) || (this->pieces[newIdx]->type == Piece::Type::QUEEN) || (x == 1 && this->pieces[newIdx]->type == Piece::Type::KING))
      {
        return true;
      }
      break;
    }
  }
  // +1
  for (short x = 1, newIdx = kingIdx + 1; (x + startCol) < 8; ++x, ++newIdx)
  {
    if (this->pieces[newIdx])
    {
      if (this->pieces[newIdx]->color == kingColor)
      {
        if (this->pieces[newIdx]->type == Piece::Type::KING)
        {
          continue;
        }
        break;
      }
      else if ((this->pieces[newIdx]->type == Piece::Type::ROOK) || (this->pieces[newIdx]->type == Piece::Type::QUEEN) || (x == 1 && this->pieces[newIdx]->type == Piece::Type::KING))
      {
        return true;
      }
      break;
    }
  }
  // +7
  for (short x = 1, newIdx = kingIdx + 7; ((x + startRow) < 8) && (x <= startCol); ++x, newIdx += 7)
  {
    if (this->pieces[newIdx])
    {
      if (this->pieces[newIdx]->color == kingColor)
      {
        if (this->pieces[newIdx]->type == Piece::Type::KING)
        {
          continue;
        }
        break;
      }
      else if ((this->pieces[newIdx]->type == Piece::Type::BISHOP) || (this->pieces[newIdx]->type == Piece::Type::QUEEN) || (x == 1 && ((kingColor == Piece::Color::WHITE && (this->pieces[newIdx]->type == Piece::Type::PAWN)) || (this->pieces[newIdx]->type == Piece::Type::KING))))
      {
        return true;
      }
      break;
    }
  }
  // +8
  for (short newIdx = kingIdx + 8; newIdx < 64; newIdx += 8)
  {
    if (this->pieces[newIdx])
    {
      if (this->pieces[newIdx]->color == kingColor)
      {
        if (this->pieces[newIdx]->type == Piece::Type::KING)
        {
          continue;
        }
        break;
      }
      else if ((this->pieces[newIdx]->type == Piece::Type::ROOK) || (this->pieces[newIdx]->type == Piece::Type::QUEEN) || (newIdx == (kingIdx + 8) && this->pieces[newIdx]->type == Piece::Type::KING))
      {
        return true;
      }
      break;
    }
  }
  // +9
  for (short x = 1, newIdx = kingIdx + 9; ((x + startRow) < 8) && ((x + startCol) < 8); ++x, newIdx += 9)
  {
    if (this->pieces[newIdx])
    {
      if (this->pieces[newIdx]->color == kingColor)
      {
        if (this->pieces[newIdx]->type == Piece::Type::KING)
        {
          continue;
        }
        break;
      }
      else if ((this->pieces[newIdx]->type == Piece::Type::BISHOP) || (this->pieces[newIdx]->type == Piece::Type::QUEEN) || (x == 1 && ((kingColor == Piece::Color::WHITE && (this->pieces[newIdx]->type == Piece::Type::PAWN)) || (this->pieces[newIdx]->type == Piece::Type::KING))))
      {
        return true;
      }
      break;
    }
  }

  // Knight checks
  if (startRow > 1)
  {
    if (startCol > 0 && (this->pieces[kingIdx - 17]) && (this->pieces[kingIdx - 17]->type == Piece::Type::KNIGHT) && (this->pieces[kingIdx - 17]->color != kingColor))
    {
      return true;
    }
    if (startCol < 7 && (this->pieces[kingIdx - 15]) && (this->pieces[kingIdx - 15]->type == Piece::Type::KNIGHT) && (this->pieces[kingIdx - 15]->color != kingColor))
    {
      return true;
    }
  }
  if (startRow > 0)
  {
    if (startCol > 1 && (this->pieces[kingIdx - 10]) && (this->pieces[kingIdx - 10]->type == Piece::Type::KNIGHT) && (this->pieces[kingIdx - 10]->color != kingColor))
    {
      return true;
    }
    if (startCol < 6 && (this->pieces[kingIdx - 6]) && (this->pieces[kingIdx - 6]->type == Piece::Type::KNIGHT) && (this->pieces[kingIdx - 6]->color != kingColor))
    {
      return true;
    }
  }
  if (startRow < 7)
  {
    if (startCol > 1 && (this->pieces[kingIdx + 6]) && (this->pieces[kingIdx + 6]->type == Piece::Type::KNIGHT) && (this->pieces[kingIdx + 6]->color != kingColor))
    {
      return true;
    }
    if (startCol < 6 && (this->pieces[kingIdx + 10]) && (this->pieces[kingIdx + 10]->type == Piece::Type::KNIGHT) && (this->pieces[kingIdx + 10]->color != kingColor))
    {
      return true;
    }
  }
  if (startRow < 6)
  {
    if (startCol > 0 && (this->pieces[kingIdx + 15]) && (this->pieces[kingIdx + 15]->type == Piece::Type::KNIGHT) && (this->pieces[kingIdx + 15]->color != kingColor))
    {
      return true;
    }
    if (startCol < 7 && (this->pieces[kingIdx + 17]) && (this->pieces[kingIdx + 17]->type == Piece::Type::KNIGHT) && (this->pieces[kingIdx + 17]->color != kingColor))
    {
      return true;
    }
  }

  return false;
}

PerftResults Chess::Perft(const int depth)
{
  for (auto x = 0; x < 64; ++x)
  {
    if (this->pieces[x] && this->pieces[x]->type == Piece::Type::KING && this->pieces[x]->color != this->turn)
    {
      if (this->InCheck(x, (Piece::Color) !this->turn))
      {
        return PerftResults();
      }
      break;
    }
  }
  PerftResults results;
  // checks vs checkmates (check if game is over in general)
  if (depth == 0)
  {
    // std::cout << this->ConvertToFEN() << std::endl;
    results.nodes = 1;
    for (auto x = 0; x < 64; ++x)
    {
      if (this->pieces[x] && this->pieces[x]->type == Piece::Type::KING && this->pieces[x]->color == this->turn)
      {
        if (this->InCheck(x, this->turn))
        {
          results.checks = 1;
        }
        break;
      }
    }
    return results;
  }
  // PseudoLegalMoves
  // See if move is actually legal
  Piece::Type promotions[4] = {Piece::Type::QUEEN, Piece::Type::ROOK, Piece::Type::BISHOP, Piece::Type::KNIGHT};
  // iterating over rows and columns is a lot easier than 0-63 directly (avoids modular math)
  std::string fenString = "";
  for (auto row = 0; row < 8; ++row)
  {
    for (auto col = 0; col < 8; ++col)
    {
      auto idx = (8 * row) + col;
      Piece *currPiece = this->pieces[idx];
      if (currPiece && currPiece->color == this->turn)
      {
        switch (currPiece->type)
        {
        case Piece::Type::PAWN:
          // White Pawn
          if (currPiece->color == Piece::Color::WHITE)
          {
            // Check if we can go one space forward)
            if (!this->pieces[idx + 8])
            {
              // Check if this move leads to promotion
              if (row == 6)
              {
                for (auto option : promotions)
                {
                  Chess *nextMoveGame = this->UpgradePawn(idx, idx + 8, option);
                  PerftResults nextResults = nextMoveGame->Perft(depth - 1);
                  if (depth == 1 && nextResults.nodes == 1L)
                  {
                    nextResults.promotions = 1;
                  }
                  results += nextResults;
                  delete nextMoveGame->pieces[idx + 8];
                  delete nextMoveGame;
                }
              }
              else
              {
                Chess *nextMoveGame = this->MovePiece(idx, idx + 8, true);
                results += nextMoveGame->Perft(depth - 1);
                delete nextMoveGame;
                // Check if we can move 2 squares
                if (row == 1 && !this->pieces[idx + 16])
                {
                  Chess *nextMoveGame = this->MovePiece(idx, idx + 16, false);
                  nextMoveGame->enPassantIdx = idx + 8;
                  fenString = nextMoveGame->BoardIdx();
                  nextMoveGame->occurrences[fenString] = 1;
                  results += nextMoveGame->Perft(depth - 1);
                  delete nextMoveGame;
                }
              }
            }
            // Check if we can take left
            if (col != 0)
            {
              // Normal take
              if (this->pieces[idx + 7] && (this->pieces[idx + 7]->color == Piece::Color::BLACK))
              {
                if (row == 6)
                {
                  // Check if taking leads to promotion
                  for (auto option : promotions)
                  {
                    Chess *nextMoveGame = this->UpgradePawn(idx, idx + 7, option);
                    PerftResults nextResults = nextMoveGame->Perft(depth - 1);
                    if (depth == 1 && nextResults.nodes == 1L)
                    {
                      nextResults.promotions = 1;
                      nextResults.captures = 1;
                    }
                    results += nextResults;
                    delete nextMoveGame->pieces[idx + 7];
                    delete nextMoveGame;
                  }
                }
                else
                {
                  Chess *nextMoveGame = this->MovePiece(idx, idx + 7, true);
                  PerftResults nextResults = nextMoveGame->Perft(depth - 1);
                  if (depth == 1 && nextResults.nodes == 1L)
                  {
                    nextResults.captures = 1;
                  }
                  results += nextResults;
                  delete nextMoveGame;
                }
              }
              // En Passant Take
              else if (this->enPassantIdx == (idx + 7))
              {
                Chess *nextMoveGame = this->MovePiece(idx, idx + 7, false);
                nextMoveGame->pieces[idx - 1] = nullptr;
                fenString = nextMoveGame->BoardIdx();
                nextMoveGame->occurrences[fenString] = 1;
                PerftResults nextResults = nextMoveGame->Perft(depth - 1);
                if (depth == 1 && nextResults.nodes == 1L)
                {
                  nextResults.enPassants = 1;
                  nextResults.captures = 1;
                }
                results += nextResults;
                delete nextMoveGame;
              }
            }
            // Check if we can take right
            if (col != 7)
            {
              // Normal take
              if (this->pieces[idx + 9] && (this->pieces[idx + 9]->color == Piece::Color::BLACK))
              {
                if (row == 6)
                {
                  for (auto option : promotions)
                  {
                    Chess *nextMoveGame = this->UpgradePawn(idx, idx + 9, option);
                    PerftResults nextResults = nextMoveGame->Perft(depth - 1);
                    if (depth == 1 && nextResults.nodes == 1L)
                    {
                      nextResults.promotions = 1;
                      nextResults.captures = 1;
                    }
                    results += nextResults;
                    delete nextMoveGame->pieces[idx + 9];
                    delete nextMoveGame;
                  }
                }
                else
                {
                  Chess *nextMoveGame = this->MovePiece(idx, idx + 9, true);
                  PerftResults nextResults = nextMoveGame->Perft(depth - 1);
                  if (depth == 1 && nextResults.nodes == 1L)
                  {
                    nextResults.captures = 1;
                  }
                  results += nextResults;
                  delete nextMoveGame;
                }
              }
              // En Passant Take
              else if (this->enPassantIdx == (idx + 9))
              {
                Chess *nextMoveGame = this->MovePiece(idx, idx + 9, false);
                nextMoveGame->pieces[idx + 1] = nullptr;
                fenString = nextMoveGame->BoardIdx();
                nextMoveGame->occurrences[fenString] = 1;
                PerftResults nextResults = nextMoveGame->Perft(depth - 1);
                if (depth == 1 && nextResults.nodes == 1L)
                {
                  nextResults.enPassants = 1;
                  nextResults.captures = 1;
                }
                results += nextResults;
                delete nextMoveGame;
              }
            }
          }
          // Black Pawn
          else
          {
            // Can we go forward
            if (!this->pieces[idx - 8])
            {
              // Check if the move leads to promotion
              if (row == 1)
              {
                for (auto option : promotions)
                {
                  Chess *nextMoveGame = this->UpgradePawn(idx, idx - 8, option);
                  PerftResults nextResults = nextMoveGame->Perft(depth - 1);
                  if (depth == 1 && nextResults.nodes == 1L)
                  {
                    nextResults.promotions = 1;
                  }
                  results += nextResults;
                  delete nextMoveGame->pieces[idx - 8];
                  delete nextMoveGame;
                }
              }
              else
              {
                Chess *nextMoveGame = this->MovePiece(idx, idx - 8, true);
                results += nextMoveGame->Perft(depth - 1);
                delete nextMoveGame;
                // Check if we can move two squares
                if (row == 6 && !this->pieces[idx - 16])
                {
                  Chess *nextMoveGame = this->MovePiece(idx, idx - 16, false);
                  nextMoveGame->enPassantIdx = idx - 8;
                  fenString = nextMoveGame->BoardIdx();
                  nextMoveGame->occurrences[fenString] = 1;
                  results += nextMoveGame->Perft(depth - 1);
                  delete nextMoveGame;
                }
              }
            }
            // Check if we can take left
            if (col != 7)
            {
              // Normal Take
              if (this->pieces[idx - 7] && (this->pieces[idx - 7]->color == Piece::Color::WHITE))
              {
                if (row == 1)
                {
                  // Check if taking leads to promotion
                  for (auto option : promotions)
                  {
                    Chess *nextMoveGame = this->UpgradePawn(idx, idx - 7, option);
                    PerftResults nextResults = nextMoveGame->Perft(depth - 1);
                    if (depth == 1 && nextResults.nodes == 1L)
                    {
                      nextResults.promotions = 1;
                      nextResults.captures = 1;
                    }
                    results += nextResults;
                    delete nextMoveGame->pieces[idx - 7];
                    delete nextMoveGame;
                  }
                }
                else
                {
                  Chess *nextMoveGame = this->MovePiece(idx, idx - 7, true);
                  PerftResults nextResults = nextMoveGame->Perft(depth - 1);
                  if (depth == 1 && nextResults.nodes == 1L)
                  {
                    nextResults.captures = 1;
                  }
                  results += nextResults;
                  delete nextMoveGame;
                }
              }
              // En Passant Take
              else if (this->enPassantIdx == (idx - 7))
              {
                Chess *nextMoveGame = this->MovePiece(idx, idx - 7, false);
                nextMoveGame->pieces[idx + 1] = nullptr;
                fenString = nextMoveGame->BoardIdx();
                nextMoveGame->occurrences[fenString] = 1;
                PerftResults nextResults = nextMoveGame->Perft(depth - 1);
                if (depth == 1 && nextResults.nodes == 1L)
                {
                  nextResults.captures = 1;
                  nextResults.enPassants = 1;
                }
                results += nextResults;
                delete nextMoveGame;
              }
            }
            // Check if we can take right
            if (col != 0)
            {
              // Normal take
              if (this->pieces[idx - 9] && (this->pieces[idx - 9]->color == Piece::Color::WHITE))
              {
                // Check if taking leads to promotion
                if (row == 1)
                {
                  for (auto option : promotions)
                  {
                    Chess *nextMoveGame = this->UpgradePawn(idx, idx - 9, option);
                    PerftResults nextResults = nextMoveGame->Perft(depth - 1);
                    if (depth == 1 && nextResults.nodes == 1L)
                    {
                      nextResults.promotions = 1;
                      nextResults.captures = 1;
                    }
                    results += nextResults;
                    delete nextMoveGame->pieces[idx - 9];
                    delete nextMoveGame;
                  }
                }
                else
                {
                  Chess *nextMoveGame = this->MovePiece(idx, idx - 9, true);
                  PerftResults nextResults = nextMoveGame->Perft(depth - 1);
                  if (depth == 1 && nextResults.nodes == 1L)
                  {
                    nextResults.captures = 1;
                  }
                  results += nextResults;
                  delete nextMoveGame;
                }
              }
              // En Passant Take
              else if (this->enPassantIdx == (idx - 9))
              {
                Chess *nextMoveGame = this->MovePiece(idx, idx - 9, false);
                nextMoveGame->pieces[idx - 1] = nullptr;
                fenString = nextMoveGame->BoardIdx();
                nextMoveGame->occurrences[fenString] = 1;
                PerftResults nextResults = nextMoveGame->Perft(depth - 1);
                if (depth == 1 && nextResults.nodes == 1L)
                {
                  nextResults.captures = 1;
                  nextResults.enPassants = 1;
                }
                results += nextResults;
                delete nextMoveGame;
              }
            }
          }
          break;
        case Piece::Type::KNIGHT:
          if (row > 1)
          {
            if (col > 0 && (!this->pieces[idx - 17] || (this->pieces[idx - 17]->color != this->turn)))
            {
              Chess *nextMoveGame = this->MovePiece(idx, idx - 17, true);
              PerftResults nextResults = nextMoveGame->Perft(depth - 1);
              if (depth == 1 && nextResults.nodes == 1L && (this->pieces[idx - 17]))
              {
                nextResults.captures = 1;
              }
              results += nextResults;
              delete nextMoveGame;
            }
            if (col < 7 && (!this->pieces[idx - 15] || (this->pieces[idx - 15]->color != this->turn)))
            {
              Chess *nextMoveGame = this->MovePiece(idx, idx - 15, true);
              PerftResults nextResults = nextMoveGame->Perft(depth - 1);
              if (depth == 1 && nextResults.nodes == 1L && (this->pieces[idx - 15]))
              {
                nextResults.captures = 1;
              }
              results += nextResults;
              delete nextMoveGame;
            }
          }
          if (row > 0)
          {
            if (col > 1 && (!this->pieces[idx - 10] || (this->pieces[idx - 10]->color != this->turn)))
            {
              Chess *nextMoveGame = this->MovePiece(idx, idx - 10, true);
              PerftResults nextResults = nextMoveGame->Perft(depth - 1);
              if (depth == 1 && nextResults.nodes == 1L && (this->pieces[idx - 10]))
              {
                nextResults.captures = 1;
              }
              results += nextResults;
              delete nextMoveGame;
            }
            if (col < 6 && (!this->pieces[idx - 6] || (this->pieces[idx - 6]->color != this->turn)))
            {
              Chess *nextMoveGame = this->MovePiece(idx, idx - 6, true);
              PerftResults nextResults = nextMoveGame->Perft(depth - 1);
              if (depth == 1 && nextResults.nodes == 1L && (this->pieces[idx - 6]))
              {
                nextResults.captures = 1;
              }
              results += nextResults;
              delete nextMoveGame;
            }
          }
          if (row < 7)
          {
            if (col > 1 && (!this->pieces[idx + 6] || (this->pieces[idx + 6]->color != this->turn)))
            {
              Chess *nextMoveGame = this->MovePiece(idx, idx + 6, true);
              PerftResults nextResults = nextMoveGame->Perft(depth - 1);
              if (depth == 1 && nextResults.nodes == 1L && (this->pieces[idx + 6]))
              {
                nextResults.captures = 1;
              }
              results += nextResults;
              delete nextMoveGame;
            }
            if (col < 6 && (!this->pieces[idx + 10] || (this->pieces[idx + 10]->color != this->turn)))
            {
              Chess *nextMoveGame = this->MovePiece(idx, idx + 10, true);
              PerftResults nextResults = nextMoveGame->Perft(depth - 1);
              if (depth == 1 && nextResults.nodes == 1L && (this->pieces[idx + 10]))
              {
                nextResults.captures = 1;
              }
              results += nextResults;
              delete nextMoveGame;
            }
          }
          if (row < 6)
          {
            if (col > 0 && (!this->pieces[idx + 15] || (this->pieces[idx + 15]->color != this->turn)))
            {
              Chess *nextMoveGame = this->MovePiece(idx, idx + 15, true);
              PerftResults nextResults = nextMoveGame->Perft(depth - 1);
              if (depth == 1 && nextResults.nodes == 1L && (this->pieces[idx + 15]))
              {
                nextResults.captures = 1;
              }
              results += nextResults;
              delete nextMoveGame;
            }
            if (col < 7 && (!this->pieces[idx + 17] || (this->pieces[idx + 17]->color != this->turn)))
            {
              Chess *nextMoveGame = this->MovePiece(idx, idx + 17, true);
              PerftResults nextResults = nextMoveGame->Perft(depth - 1);
              if (depth == 1 && nextResults.nodes == 1L && (this->pieces[idx + 17]))
              {
                nextResults.captures = 1;
              }
              results += nextResults;
              delete nextMoveGame;
            }
          }
          break;
        case Piece::Type::BISHOP:
          // -9
          for (short x = 1, nextIdx = idx - 9; (x <= row) && (x <= col); ++x, nextIdx -= 9)
          {
            if (!this->pieces[nextIdx] || (this->pieces[nextIdx]->color != this->turn))
            {
              Chess *nextMoveGame = this->MovePiece(idx, nextIdx, true);
              PerftResults nextResults = nextMoveGame->Perft(depth - 1);
              if (depth == 1 && nextResults.nodes == 1L && (this->pieces[nextIdx]))
              {
                nextResults.captures = 1;
              }
              results += nextResults;
              delete nextMoveGame;
            }
            if (this->pieces[nextIdx])
            {
              break;
            }
          }
          // -7
          for (short x = 1, nextIdx = idx - 7; (x <= row) && ((x + col) < 8); ++x, nextIdx -= 7)
          {
            if (!this->pieces[nextIdx] || (this->pieces[nextIdx]->color != this->turn))
            {
              Chess *nextMoveGame = this->MovePiece(idx, nextIdx, true);
              PerftResults nextResults = nextMoveGame->Perft(depth - 1);
              if (depth == 1 && nextResults.nodes == 1L && (this->pieces[nextIdx]))
              {
                nextResults.captures = 1;
              }
              results += nextResults;
              delete nextMoveGame;
            }
            if (this->pieces[nextIdx])
            {
              break;
            }
          }
          // +7
          for (short x = 1, nextIdx = idx + 7; ((x + row) < 8) && (x <= col); ++x, nextIdx += 7)
          {
            if (!this->pieces[nextIdx] || (this->pieces[nextIdx]->color != this->turn))
            {
              Chess *nextMoveGame = this->MovePiece(idx, nextIdx, true);
              PerftResults nextResults = nextMoveGame->Perft(depth - 1);
              if (depth == 1 && nextResults.nodes == 1L && (this->pieces[nextIdx]))
              {
                nextResults.captures = 1;
              }
              results += nextResults;
              delete nextMoveGame;
            }
            if (this->pieces[nextIdx])
            {
              break;
            }
          }
          // +9
          for (short x = 1, nextIdx = idx + 9; ((x + row) < 8) && ((x + col) < 8); ++x, nextIdx += 9)
          {
            if (!this->pieces[nextIdx] || (this->pieces[nextIdx]->color != this->turn))
            {
              Chess *nextMoveGame = this->MovePiece(idx, nextIdx, true);
              PerftResults nextResults = nextMoveGame->Perft(depth - 1);
              if (depth == 1 && nextResults.nodes == 1L && (this->pieces[nextIdx]))
              {
                nextResults.captures = 1;
              }
              results += nextResults;
              delete nextMoveGame;
            }
            if (this->pieces[nextIdx])
            {
              break;
            }
          }
          break;
        case Piece::Type::ROOK:
          for (auto downIdx = idx - 8; downIdx >= 0; downIdx -= 8)
          {
            if (!this->pieces[downIdx] || (this->pieces[downIdx]->color != this->turn))
            {
              Chess *nextMoveGame = this->MovePiece(idx, downIdx, false);
              if (nextMoveGame->bCastle && idx == 63)
              {
                nextMoveGame->bCastle = false;
              }
              else if (nextMoveGame->bQueenCastle && idx == 56)
              {
                nextMoveGame->bQueenCastle = false;
              }
              fenString = nextMoveGame->BoardIdx();
              nextMoveGame->occurrences[fenString]++;
              PerftResults nextResults = nextMoveGame->Perft(depth - 1);
              if (depth == 1 && nextResults.nodes == 1L && (this->pieces[downIdx]))
              {
                nextResults.captures = 1;
              }
              results += nextResults;
              delete nextMoveGame;
            }
            if (this->pieces[downIdx])
            {
              break;
            }
          }
          for (auto upIdx = idx + 8; upIdx < 64; upIdx += 8)
          {
            if (!this->pieces[upIdx] || (this->pieces[upIdx]->color != this->turn))
            {
              Chess *nextMoveGame = this->MovePiece(idx, upIdx, false);
              if (nextMoveGame->wCastle && idx == 7)
              {
                nextMoveGame->wCastle = false;
              }
              else if (nextMoveGame->wQueenCastle && idx == 0)
              {
                nextMoveGame->wQueenCastle = false;
              }
              fenString = nextMoveGame->BoardIdx();
              nextMoveGame->occurrences[fenString]++;
              PerftResults nextResults = nextMoveGame->Perft(depth - 1);
              if (depth == 1 && nextResults.nodes == 1L && (this->pieces[upIdx]))
              {
                nextResults.captures = 1;
              }
              results += nextResults;
              delete nextMoveGame;
            }
            if (this->pieces[upIdx])
            {
              break;
            }
          }
          for (auto leftIdx = idx - 1; leftIdx >= (row * 8); --leftIdx)
          {
            if (!this->pieces[leftIdx] || (this->pieces[leftIdx]->color != this->turn))
            {
              Chess *nextMoveGame = this->MovePiece(idx, leftIdx, false);
              if (nextMoveGame->wCastle && idx == 7)
              {
                nextMoveGame->wCastle = false;
              }
              else if (nextMoveGame->bCastle && idx == 63)
              {
                nextMoveGame->bCastle = false;
              }
              fenString = nextMoveGame->BoardIdx();
              nextMoveGame->occurrences[fenString]++;
              PerftResults nextResults = nextMoveGame->Perft(depth - 1);
              if (depth == 1 && nextResults.nodes == 1L && (this->pieces[leftIdx]))
              {
                nextResults.captures = 1;
              }
              results += nextResults;
              delete nextMoveGame;
            }
            if (this->pieces[leftIdx])
            {
              break;
            }
          }
          for (auto rightIdx = idx + 1; rightIdx < ((row + 1) * 8); ++rightIdx)
          {
            if (!this->pieces[rightIdx] || (this->pieces[rightIdx]->color != this->turn))
            {
              Chess *nextMoveGame = this->MovePiece(idx, rightIdx, false);
              if (nextMoveGame->wQueenCastle && idx == 0)
              {
                nextMoveGame->wQueenCastle = false;
              }
              else if (nextMoveGame->bQueenCastle && idx == 56)
              {
                nextMoveGame->bQueenCastle = false;
              }
              fenString = nextMoveGame->BoardIdx();
              nextMoveGame->occurrences[fenString]++;
              PerftResults nextResults = nextMoveGame->Perft(depth - 1);
              if (depth == 1 && nextResults.nodes == 1L && (this->pieces[rightIdx]))
              {
                nextResults.captures = 1;
              }
              results += nextResults;
              delete nextMoveGame;
            }
            if (this->pieces[rightIdx])
            {
              break;
            }
          }
          break;
        case Piece::Type::QUEEN:
          // -9
          for (short x = 1, nextIdx = idx - 9; (x <= row) && (x <= col); ++x, nextIdx -= 9)
          {
            if (!this->pieces[nextIdx] || (this->pieces[nextIdx]->color != this->turn))
            {
              Chess *nextMoveGame = this->MovePiece(idx, nextIdx, true);
              PerftResults nextResults = nextMoveGame->Perft(depth - 1);
              if (depth == 1 && nextResults.nodes == 1L && (this->pieces[nextIdx]))
              {
                nextResults.captures = 1;
              }
              results += nextResults;
              delete nextMoveGame;
            }
            if (this->pieces[nextIdx])
            {
              break;
            }
          }
          // -7
          for (short x = 1, nextIdx = idx - 7; (x <= row) && ((x + col) < 8); ++x, nextIdx -= 7)
          {
            if (!this->pieces[nextIdx] || (this->pieces[nextIdx]->color != this->turn))
            {
              Chess *nextMoveGame = this->MovePiece(idx, nextIdx, true);
              PerftResults nextResults = nextMoveGame->Perft(depth - 1);
              if (depth == 1 && nextResults.nodes == 1L && (this->pieces[nextIdx]))
              {
                nextResults.captures = 1;
              }
              results += nextResults;
              delete nextMoveGame;
            }
            if (this->pieces[nextIdx])
            {
              break;
            }
          }
          // +7
          for (short x = 1, nextIdx = idx + 7; ((x + row) < 8) && (x <= col); ++x, nextIdx += 7)
          {
            if (!this->pieces[nextIdx] || (this->pieces[nextIdx]->color != this->turn))
            {
              Chess *nextMoveGame = this->MovePiece(idx, nextIdx, true);
              PerftResults nextResults = nextMoveGame->Perft(depth - 1);
              if (depth == 1 && nextResults.nodes == 1L && (this->pieces[nextIdx]))
              {
                nextResults.captures = 1;
              }
              results += nextResults;
              delete nextMoveGame;
            }
            if (this->pieces[nextIdx])
            {
              break;
            }
          }
          // +9
          for (short x = 1, nextIdx = idx + 9; ((x + row) < 8) && ((x + col) < 8); ++x, nextIdx += 9)
          {
            if (!this->pieces[nextIdx] || (this->pieces[nextIdx]->color != this->turn))
            {
              Chess *nextMoveGame = this->MovePiece(idx, nextIdx, true);
              PerftResults nextResults = nextMoveGame->Perft(depth - 1);
              if (depth == 1 && nextResults.nodes == 1L && (this->pieces[nextIdx]))
              {
                nextResults.captures = 1;
              }
              results += nextResults;
              delete nextMoveGame;
            }
            if (this->pieces[nextIdx])
            {
              break;
            }
          }
          for (auto downIdx = idx - 8; downIdx >= 0; downIdx -= 8)
          {
            if (!this->pieces[downIdx] || (this->pieces[downIdx]->color != this->turn))
            {
              Chess *nextMoveGame = this->MovePiece(idx, downIdx, true);
              PerftResults nextResults = nextMoveGame->Perft(depth - 1);
              if (depth == 1 && nextResults.nodes == 1L && (this->pieces[downIdx]))
              {
                nextResults.captures = 1;
              }
              results += nextResults;
              delete nextMoveGame;
            }
            if (this->pieces[downIdx])
            {
              break;
            }
          }
          for (auto upIdx = idx + 8; upIdx < 64; upIdx += 8)
          {
            if (!this->pieces[upIdx] || (this->pieces[upIdx]->color != this->turn))
            {
              Chess *nextMoveGame = this->MovePiece(idx, upIdx, true);
              PerftResults nextResults = nextMoveGame->Perft(depth - 1);
              if (depth == 1 && nextResults.nodes == 1L && (this->pieces[upIdx]))
              {
                nextResults.captures = 1;
              }
              results += nextResults;
              delete nextMoveGame;
            }
            if (this->pieces[upIdx])
            {
              break;
            }
          }
          for (auto leftIdx = idx - 1; leftIdx >= (row * 8); --leftIdx)
          {
            if (!this->pieces[leftIdx] || (this->pieces[leftIdx]->color != this->turn))
            {
              Chess *nextMoveGame = this->MovePiece(idx, leftIdx, true);
              PerftResults nextResults = nextMoveGame->Perft(depth - 1);
              if (depth == 1 && nextResults.nodes == 1L && (this->pieces[leftIdx]))
              {
                nextResults.captures = 1;
              }
              results += nextResults;
              delete nextMoveGame;
            }
            if (this->pieces[leftIdx])
            {
              break;
            }
          }
          for (auto rightIdx = idx + 1; rightIdx < ((row + 1) * 8); ++rightIdx)
          {
            if (!this->pieces[rightIdx] || (this->pieces[rightIdx]->color != this->turn))
            {
              Chess *nextMoveGame = this->MovePiece(idx, rightIdx, true);
              PerftResults nextResults = nextMoveGame->Perft(depth - 1);
              if (depth == 1 && nextResults.nodes == 1L && (this->pieces[rightIdx]))
              {
                nextResults.captures = 1;
              }
              results += nextResults;
              delete nextMoveGame;
            }
            if (this->pieces[rightIdx])
            {
              break;
            }
          }
          break;
        default:
          // King goes one any direction
          auto nextIdx = 0;
          short directions[8] = {-1, -1, -1, -1, -1, -1, -1, -1};
          if (row != 0 && col != 0)
          {
            nextIdx = idx - 9;
            if (!this->pieces[nextIdx] || this->pieces[nextIdx]->color != this->turn)
            {
              directions[0] = nextIdx;
            }
          }
          if (row != 0)
          {
            nextIdx = idx - 8;
            if (!this->pieces[nextIdx] || this->pieces[nextIdx]->color != this->turn)
            {
              directions[1] = nextIdx;
            }
          }
          if (row != 0 && col != 7)
          {
            nextIdx = idx - 7;
            if (!this->pieces[nextIdx] || this->pieces[nextIdx]->color != this->turn)
            {
              directions[2] = nextIdx;
            }
          }
          if (col != 0)
          {
            nextIdx = idx - 1;
            if (!this->pieces[nextIdx] || this->pieces[nextIdx]->color != this->turn)
            {
              directions[3] = nextIdx;
            }
          }
          if (col != 7)
          {
            nextIdx = idx + 1;
            if (!this->pieces[nextIdx] || this->pieces[nextIdx]->color != this->turn)
            {
              directions[4] = nextIdx;
            }
          }
          if (row != 7 && col != 0)
          {
            nextIdx = idx + 7;
            if (!this->pieces[nextIdx] || this->pieces[nextIdx]->color != this->turn)
            {
              directions[5] = nextIdx;
            }
          }
          if (row != 7)
          {
            nextIdx = idx + 8;
            if (!this->pieces[nextIdx] || this->pieces[nextIdx]->color != this->turn)
            {
              directions[6] = nextIdx;
            }
          }
          if (row != 7 && col != 7)
          {
            nextIdx = idx + 9;
            if (!this->pieces[nextIdx] || this->pieces[nextIdx]->color != this->turn)
            {
              directions[7] = nextIdx;
            }
          }
          for (auto i = 0; i < 8; ++i)
          {
            if (directions[i] != -1)
            {
              Chess *nextMoveGame = this->MovePiece(idx, directions[i], false);
              if (this->turn == Piece::Color::WHITE)
              {
                if (nextMoveGame->wCastle)
                {
                  nextMoveGame->wCastle = false;
                }
                if (nextMoveGame->wQueenCastle)
                {
                  nextMoveGame->wQueenCastle = false;
                }
              }
              else
              {
                if (nextMoveGame->bCastle)
                {
                  nextMoveGame->bCastle = false;
                }
                if (nextMoveGame->bQueenCastle)
                {
                  nextMoveGame->bQueenCastle = false;
                }
              }
              fenString = nextMoveGame->BoardIdx();
              nextMoveGame->occurrences[fenString]++;
              PerftResults nextResults = nextMoveGame->Perft(depth - 1);
              if (depth == 1 && nextResults.nodes == 1L && (this->pieces[directions[i]]))
              {
                nextResults.captures = 1;
              }
              results += nextResults;
              delete nextMoveGame;
            }
          }
          break;
        }
      }
    }
  }
  if (this->turn == Piece::Color::WHITE)
  {
    if (this->wCastle && !this->pieces[5] && !this->pieces[6] && !this->InCheck(4, Piece::Color::WHITE) && !this->InCheck(5, Piece::Color::WHITE) && !this->InCheck(6, Piece::Color::WHITE))
    {
      Chess *nextMoveGame = this->MovePiece(4, 6, false);
      nextMoveGame->wCastle = false;
      if (nextMoveGame->wQueenCastle)
      {
        nextMoveGame->wQueenCastle = false;
      }
      nextMoveGame->pieces[5] = this->pieces[7];
      fenString = nextMoveGame->BoardIdx();
      nextMoveGame->occurrences[fenString]++;
      PerftResults nextResults = nextMoveGame->Perft(depth - 1);
      if (depth == 1)
      {
        nextResults.castles = 1;
      }
      results += nextResults;
      delete nextMoveGame;
    }
    if (this->wQueenCastle && !this->pieces[1] && !this->pieces[2] && !this->pieces[3] && !this->InCheck(2, Piece::Color::WHITE) && !this->InCheck(3, Piece::Color::WHITE) && !this->InCheck(4, Piece::Color::WHITE))
    {
      Chess *nextMoveGame = this->MovePiece(4, 2, false);
      nextMoveGame->wQueenCastle = false;
      if (nextMoveGame->wCastle)
      {
        nextMoveGame->wCastle = false;
      }
      nextMoveGame->pieces[3] = this->pieces[0];
      fenString = nextMoveGame->BoardIdx();
      nextMoveGame->occurrences[fenString]++;
      PerftResults nextResults = nextMoveGame->Perft(depth - 1);
      if (depth == 1)
      {
        nextResults.castles = 1;
      }
      results += nextResults;
      delete nextMoveGame;
    }
  }
  else
  {
    if (this->bCastle && !this->pieces[61] && !this->pieces[62] && !this->InCheck(60, Piece::Color::BLACK) && !this->InCheck(61, Piece::Color::BLACK) && !this->InCheck(62, Piece::Color::BLACK))
    {
      Chess *nextMoveGame = this->MovePiece(60, 62, false);
      nextMoveGame->bCastle = false;
      if (nextMoveGame->bQueenCastle)
      {
        nextMoveGame->bQueenCastle = false;
      }
      nextMoveGame->pieces[61] = this->pieces[63];
      fenString = nextMoveGame->BoardIdx();
      nextMoveGame->occurrences[fenString]++;
      PerftResults nextResults = nextMoveGame->Perft(depth - 1);
      if (depth == 1)
      {
        nextResults.castles = 1;
      }
      results += nextResults;
      delete nextMoveGame;
    }
    if (this->bQueenCastle && !this->pieces[57] && !this->pieces[58] && !this->pieces[59] && !this->InCheck(58, Piece::Color::BLACK) && !this->InCheck(59, Piece::Color::BLACK) && !this->InCheck(60, Piece::Color::BLACK))
    {
      Chess *nextMoveGame = this->MovePiece(60, 58, false);
      nextMoveGame->bQueenCastle = false;
      if (nextMoveGame->bCastle)
      {
        nextMoveGame->bCastle = false;
      }
      nextMoveGame->pieces[59] = this->pieces[56];
      fenString = nextMoveGame->BoardIdx();
      nextMoveGame->occurrences[fenString]++;
      PerftResults nextResults = nextMoveGame->Perft(depth - 1);
      if (depth == 1)
      {
        nextResults.castles = 1;
      }
      results += nextResults;
      delete nextMoveGame;
    }
  }
  return results;
}

// This function does the following:
// * Creates a copy of the game
// * Moves the piece from "start" to "end" in the copy
// * Increments the lastPawnOrTake & fullTurns
// * Sets en passant idx to -1 (if needed, set later)
// * Can't deal with all occurrences bc of castling & en passant
Chess *Chess::MovePiece(const short start, const short end, const bool updateOccurrences)
{
  Chess *nextMoveGame = new Chess(*this);
  nextMoveGame->pieces[start] = nullptr;
  nextMoveGame->pieces[end] = this->pieces[start];
  if ((this->pieces[start]->type == Piece::Type::PAWN) || this->pieces[end])
  {
    nextMoveGame->lastPawnOrTake = 0;
  }
  else
  {
    ++nextMoveGame->lastPawnOrTake;
  }
  nextMoveGame->turn = (this->turn == Piece::Color::WHITE) ? Piece::Color::BLACK : Piece::Color::WHITE;
  if (this->turn == Piece::Color::BLACK)
  {
    ++nextMoveGame->fullTurns;
  }
  if (nextMoveGame->enPassantIdx != -1)
  {
    nextMoveGame->enPassantIdx = -1;
  }
  if (updateOccurrences)
  {
    nextMoveGame->occurrences[nextMoveGame->BoardIdx()]++;
  }
  return nextMoveGame;
}

Chess *Chess::UpgradePawn(const short start, const short end, const Piece::Type option)
{
  Chess *nextMoveGame = new Chess(*this);
  nextMoveGame->pieces[start] = nullptr;
  nextMoveGame->pieces[end] = new Piece();
  nextMoveGame->pieces[end]->color = this->pieces[start]->color;
  nextMoveGame->pieces[end]->type = option;
  nextMoveGame->lastPawnOrTake = 0;
  if (nextMoveGame->enPassantIdx != -1)
  {
    nextMoveGame->enPassantIdx = -1;
  }
  nextMoveGame->turn = (this->turn == Piece::Color::WHITE) ? Piece::Color::BLACK : Piece::Color::WHITE;
  if (this->turn == Piece::Color::BLACK)
  {
    ++nextMoveGame->fullTurns;
  }
  nextMoveGame->occurrences[nextMoveGame->BoardIdx()]++;
  return nextMoveGame;
}

std::vector<Chess *> Chess::PseudolegalMoves()
{
  // PseudoLegalMoves
  // See if move is actually legal
  std::vector<Chess *> pseudoLegalMoves;
  Piece::Type promotions[4] = {Piece::Type::QUEEN, Piece::Type::ROOK, Piece::Type::BISHOP, Piece::Type::KNIGHT};
  // iterating over rows and columns is a lot easier than 0-63 directly (avoids modular math)
  std::string fenString = "";
  for (auto row = 0; row < 8; ++row)
  {
    for (auto col = 0; col < 8; ++col)
    {
      auto idx = (8 * row) + col;
      Piece *currPiece = this->pieces[idx];
      if (currPiece && currPiece->color == this->turn)
      {
        switch (currPiece->type)
        {
        case Piece::Type::PAWN:
          // White Pawn
          if (currPiece->color == Piece::Color::WHITE)
          {
            // Check if we can go one space forward)
            if (!this->pieces[idx + 8])
            {
              // Check if this move leads to promotion
              if (row + 1 == 7)
              {
                for (auto option : promotions)
                {
                  pseudoLegalMoves.push_back(this->UpgradePawn(idx, idx + 8, option));
                }
              }
              else
              {
                pseudoLegalMoves.push_back(this->MovePiece(idx, idx + 8, true));
                // Check if we can move 2 squares
                if (idx < 16 && !this->pieces[idx + 16])
                {
                  Chess *nextMoveGame = this->MovePiece(idx, idx + 16, false);
                  nextMoveGame->enPassantIdx = idx + 8;
                  fenString = nextMoveGame->BoardIdx();
                  nextMoveGame->occurrences[fenString] = 1;
                  pseudoLegalMoves.push_back(nextMoveGame);
                }
              }
            }
            // Check if we can take left
            if (col != 0)
            {
              // Normal take
              if ((this->pieces[idx + 7] != nullptr) && (this->pieces[idx + 7]->color == Piece::Color::BLACK))
              {
                // Does taking lead to promotion
                if (row + 1 == 7)
                {
                  for (auto option : promotions)
                  {
                    pseudoLegalMoves.push_back(this->UpgradePawn(idx, idx + 7, option));
                  }
                }
                else
                {
                  pseudoLegalMoves.push_back(this->MovePiece(idx, idx + 7, true));
                }
              }
              // En Passant Take
              else if (this->enPassantIdx == (idx + 7))
              {
                Chess *nextMoveGame = this->MovePiece(idx, idx + 7, false);
                nextMoveGame->pieces[idx - 1] = nullptr;
                fenString = nextMoveGame->BoardIdx();
                nextMoveGame->occurrences[fenString] = 1;
                pseudoLegalMoves.push_back(nextMoveGame);
              }
            }
            // Check if we can take right
            if (col != 7)
            {
              // Normal take
              if ((this->pieces[idx + 9] != nullptr) && (this->pieces[idx + 9]->color == Piece::Color::BLACK))
              {
                // Does taking lead to promotion
                if (row + 1 == 7)
                {
                  for (auto option : promotions)
                  {
                    pseudoLegalMoves.push_back(this->UpgradePawn(idx, idx + 9, option));
                  }
                }
                else
                {
                  pseudoLegalMoves.push_back(this->MovePiece(idx, idx + 9, true));
                }
              }
              // En Passant Take
              else if (this->enPassantIdx == (idx + 9))
              {
                Chess *nextMoveGame = this->MovePiece(idx, idx + 9, false);
                nextMoveGame->pieces[idx + 1] = nullptr;
                fenString = nextMoveGame->BoardIdx();
                nextMoveGame->occurrences[fenString] = 1;
                pseudoLegalMoves.push_back(nextMoveGame);
              }
            }
          }
          // Black Pawn
          else
          {
            // Can we go forward
            if (!this->pieces[idx - 8])
            {
              // Check if the move leads to promotion
              if (row - 1 == 0)
              {
                for (auto option : promotions)
                {
                  pseudoLegalMoves.push_back(this->UpgradePawn(idx, idx - 8, option));
                }
              }
              else
              {
                pseudoLegalMoves.push_back(this->MovePiece(idx, idx - 8, true));
                // Check if we can move two squares
                if (idx >= 48 && !this->pieces[idx - 16])
                {
                  Chess *nextMoveGame = this->MovePiece(idx, idx - 16, false);
                  nextMoveGame->enPassantIdx = idx - 8;
                  fenString = nextMoveGame->BoardIdx();
                  nextMoveGame->occurrences[fenString] = 1;
                  pseudoLegalMoves.push_back(nextMoveGame);
                }
              }
            }
            // Check if we can take left
            if (col != 7)
            {
              // Normal Take
              if ((this->pieces[idx - 7] != nullptr) && (this->pieces[idx - 7]->color == Piece::Color::WHITE))
              {
                if (row - 1 == 0)
                {
                  // Check if taking leads to promotion
                  for (auto option : promotions)
                  {
                    pseudoLegalMoves.push_back(this->UpgradePawn(idx, idx - 7, option));
                  }
                }
                else
                {
                  pseudoLegalMoves.push_back(this->MovePiece(idx, idx - 7, true));
                }
              }
              // En Passant Take
              else if (this->enPassantIdx == (idx - 7))
              {
                Chess *nextMoveGame = this->MovePiece(idx, idx - 7, false);
                nextMoveGame->pieces[idx + 1] = nullptr;
                fenString = nextMoveGame->BoardIdx();
                nextMoveGame->occurrences[fenString] = 1;
                pseudoLegalMoves.push_back(nextMoveGame);
              }
            }
            // Check if we can take right
            if (col != 0)
            {
              // Normal take
              if ((this->pieces[idx - 9] != nullptr) && (this->pieces[idx - 9]->color == Piece::Color::WHITE))
              {
                // Check if taking leads to promotion
                if (row - 1 == 0)
                {
                  for (auto option : promotions)
                  {
                    pseudoLegalMoves.push_back(this->UpgradePawn(idx, idx - 9, option));
                  }
                }
                else
                {
                  pseudoLegalMoves.push_back(this->MovePiece(idx, idx - 9, true));
                }
              }
              // En Passant Take
              else if (this->enPassantIdx == (idx - 9))
              {
                Chess *nextMoveGame = this->MovePiece(idx, idx - 9, false);
                nextMoveGame->pieces[idx - 1] = nullptr;
                fenString = nextMoveGame->BoardIdx();
                nextMoveGame->occurrences[fenString] = 1;
                pseudoLegalMoves.push_back(nextMoveGame);
              }
            }
          }
          break;
        case Piece::Type::KNIGHT:
          if (row > 1)
          {
            if (col > 0 && (!this->pieces[idx - 17] || (this->pieces[idx - 17]->color != this->turn)))
            {
              pseudoLegalMoves.push_back(this->MovePiece(idx, idx - 17, true));
            }
            if (col < 7 && (!this->pieces[idx - 15] || (this->pieces[idx - 15]->color != this->turn)))
            {
              pseudoLegalMoves.push_back(this->MovePiece(idx, idx - 15, true));
            }
          }
          if (row > 0)
          {
            if (col > 1 && (!this->pieces[idx - 10] || (this->pieces[idx - 10]->color != this->turn)))
            {
              pseudoLegalMoves.push_back(this->MovePiece(idx, idx - 10, true));
            }
            if (col < 6 && (!this->pieces[idx - 6] || (this->pieces[idx - 6]->color != this->turn)))
            {
              pseudoLegalMoves.push_back(this->MovePiece(idx, idx - 6, true));
            }
          }
          if (row < 7)
          {
            if (col > 1 && (!this->pieces[idx + 6] || (this->pieces[idx + 6]->color != this->turn)))
            {
              pseudoLegalMoves.push_back(this->MovePiece(idx, idx + 6, true));
            }
            if (col < 6 && (!this->pieces[idx + 10] || (this->pieces[idx + 10]->color != this->turn)))
            {
              pseudoLegalMoves.push_back(this->MovePiece(idx, idx + 10, true));
            }
          }
          if (row < 6)
          {
            if (col > 0 && (!this->pieces[idx + 15] || (this->pieces[idx + 15]->color != this->turn)))
            {
              pseudoLegalMoves.push_back(this->MovePiece(idx, idx + 15, true));
            }
            if (col < 7 && (!this->pieces[idx + 17] || (this->pieces[idx + 17]->color != this->turn)))
            {
              pseudoLegalMoves.push_back(this->MovePiece(idx, idx + 17, true));
            }
          }
          break;
        case Piece::Type::BISHOP:
          // -9
          for (short x = 1, nextIdx = idx - 9; (x <= row) && (x <= col); ++x, nextIdx -= 9)
          {
            if (!this->pieces[nextIdx] || (this->pieces[nextIdx]->color != this->turn))
            {
              pseudoLegalMoves.push_back(this->MovePiece(idx, nextIdx, true));
            }
            if (this->pieces[nextIdx])
            {
              break;
            }
          }
          // -7
          for (short x = 1, nextIdx = idx - 7; (x <= row) && ((x + col) < 8); ++x, nextIdx -= 7)
          {
            if (!this->pieces[nextIdx] || (this->pieces[nextIdx]->color != this->turn))
            {
              pseudoLegalMoves.push_back(this->MovePiece(idx, nextIdx, true));
            }
            if (this->pieces[nextIdx])
            {
              break;
            }
          }
          // +7
          for (short x = 1, nextIdx = idx + 7; ((x + row) < 8) && (x <= col); ++x, nextIdx += 7)
          {
            if (!this->pieces[nextIdx] || (this->pieces[nextIdx]->color != this->turn))
            {
              pseudoLegalMoves.push_back(this->MovePiece(idx, nextIdx, true));
            }
            if (this->pieces[nextIdx])
            {
              break;
            }
          }
          // +9
          for (short x = 1, nextIdx = idx + 9; ((x + row) < 8) && ((x + col) < 8); ++x, nextIdx += 9)
          {
            if (!this->pieces[nextIdx] || (this->pieces[nextIdx]->color != this->turn))
            {
              pseudoLegalMoves.push_back(this->MovePiece(idx, nextIdx, true));
            }
            if (this->pieces[nextIdx])
            {
              break;
            }
          }
          break;
        case Piece::Type::ROOK:
          for (auto downIdx = idx - 8; downIdx >= 0; downIdx -= 8)
          {
            if (!this->pieces[downIdx] || (this->pieces[downIdx]->color != this->turn))
            {
              Chess *nextMoveGame = this->MovePiece(idx, downIdx, false);
              if (nextMoveGame->bCastle && idx == 63)
              {
                nextMoveGame->bCastle = false;
              }
              else if (nextMoveGame->bQueenCastle && idx == 56)
              {
                nextMoveGame->bQueenCastle = false;
              }
              fenString = nextMoveGame->BoardIdx();
              nextMoveGame->occurrences[fenString]++;
              pseudoLegalMoves.push_back(nextMoveGame);
            }
            if (this->pieces[downIdx])
            {
              break;
            }
          }
          for (auto upIdx = idx + 8; upIdx < 64; upIdx += 8)
          {
            if (!this->pieces[upIdx] || (this->pieces[upIdx]->color != this->turn))
            {
              Chess *nextMoveGame = this->MovePiece(idx, upIdx, false);
              if (nextMoveGame->wCastle && idx == 7)
              {
                nextMoveGame->wCastle = false;
              }
              else if (nextMoveGame->wQueenCastle && idx == 0)
              {
                nextMoveGame->wQueenCastle = false;
              }
              fenString = nextMoveGame->BoardIdx();
              nextMoveGame->occurrences[fenString]++;
              pseudoLegalMoves.push_back(nextMoveGame);
            }
            if (this->pieces[upIdx])
            {
              break;
            }
          }
          for (auto leftIdx = idx - 1; leftIdx >= (idx - col); --leftIdx)
          {
            if (!this->pieces[leftIdx] || (this->pieces[leftIdx]->color != this->turn))
            {
              Chess *nextMoveGame = this->MovePiece(idx, leftIdx, false);
              if (nextMoveGame->wCastle && idx == 7)
              {
                nextMoveGame->wCastle = false;
              }
              else if (nextMoveGame->bCastle && idx == 63)
              {
                nextMoveGame->bCastle = false;
              }
              fenString = nextMoveGame->BoardIdx();
              nextMoveGame->occurrences[fenString]++;
              pseudoLegalMoves.push_back(nextMoveGame);
            }
            if (this->pieces[leftIdx])
            {
              break;
            }
          }
          for (auto rightIdx = idx + 1; rightIdx < idx + (8 - col); ++rightIdx)
          {
            if (!this->pieces[rightIdx] || (this->pieces[rightIdx]->color != this->turn))
            {
              Chess *nextMoveGame = this->MovePiece(idx, rightIdx, false);
              if (nextMoveGame->wQueenCastle && idx == 0)
              {
                nextMoveGame->wQueenCastle = false;
              }
              else if (nextMoveGame->bQueenCastle && idx == 56)
              {
                nextMoveGame->bQueenCastle = false;
              }
              fenString = nextMoveGame->BoardIdx();
              nextMoveGame->occurrences[fenString]++;
              pseudoLegalMoves.push_back(nextMoveGame);
            }
            if (this->pieces[rightIdx])
            {
              break;
            }
          }
          break;
        case Piece::Type::QUEEN:
          // -9
          for (short x = 1, nextIdx = idx - 9; (x <= row) && (x <= col); ++x, nextIdx -= 9)
          {
            if (!this->pieces[nextIdx] || (this->pieces[nextIdx]->color != this->turn))
            {
              pseudoLegalMoves.push_back(this->MovePiece(idx, nextIdx, true));
            }
            if (this->pieces[nextIdx])
            {
              break;
            }
          }
          // -7
          for (short x = 1, nextIdx = idx - 7; (x <= row) && ((x + col) < 8); ++x, nextIdx -= 7)
          {
            if (!this->pieces[nextIdx] || (this->pieces[nextIdx]->color != this->turn))
            {
              pseudoLegalMoves.push_back(this->MovePiece(idx, nextIdx, true));
            }
            if (this->pieces[nextIdx])
            {
              break;
            }
          }
          // +7
          for (short x = 1, nextIdx = idx + 7; ((x + row) < 8) && (x <= col); ++x, nextIdx += 7)
          {
            if (!this->pieces[nextIdx] || (this->pieces[nextIdx]->color != this->turn))
            {
              pseudoLegalMoves.push_back(this->MovePiece(idx, nextIdx, true));
            }
            if (this->pieces[nextIdx])
            {
              break;
            }
          }
          // +9
          for (short x = 1, nextIdx = idx + 9; ((x + row) < 8) && ((x + col) < 8); ++x, nextIdx += 9)
          {
            if (!this->pieces[nextIdx] || (this->pieces[nextIdx]->color != this->turn))
            {
              pseudoLegalMoves.push_back(this->MovePiece(idx, nextIdx, true));
            }
            if (this->pieces[nextIdx])
            {
              break;
            }
          }
          for (auto downIdx = idx - 8; downIdx >= 0; downIdx -= 8)
          {
            if (!this->pieces[downIdx] || (this->pieces[downIdx]->color != this->turn))
            {
              pseudoLegalMoves.push_back(this->MovePiece(idx, downIdx, true));
            }
            if (this->pieces[downIdx])
            {
              break;
            }
          }
          for (auto upIdx = idx + 8; upIdx < 64; upIdx += 8)
          {
            if (!this->pieces[upIdx] || (this->pieces[upIdx]->color != this->turn))
            {
              pseudoLegalMoves.push_back(this->MovePiece(idx, upIdx, true));
            }
            if (this->pieces[upIdx])
            {
              break;
            }
          }
          for (auto leftIdx = idx - 1; leftIdx >= (idx - col); --leftIdx)
          {
            if (!this->pieces[leftIdx] || (this->pieces[leftIdx]->color != this->turn))
            {
              pseudoLegalMoves.push_back(this->MovePiece(idx, leftIdx, true));
            }
            if (this->pieces[leftIdx])
            {
              break;
            }
          }
          for (auto rightIdx = idx + 1; rightIdx < idx + (8 - col); ++rightIdx)
          {
            if (!this->pieces[rightIdx] || (this->pieces[rightIdx]->color != this->turn))
            {
              pseudoLegalMoves.push_back(this->MovePiece(idx, rightIdx, true));
            }
            if (this->pieces[rightIdx])
            {
              break;
            }
          }
          break;
        default:
          // King goes one any direction
          auto nextIdx = 0;
          short directions[8] = {-1, -1, -1, -1, -1, -1, -1, -1};
          if (row != 0 && col != 0)
          {
            nextIdx = idx - 9;
            if (!this->pieces[nextIdx] || this->pieces[nextIdx]->color != this->turn)
            {
              directions[0] = nextIdx;
            }
          }
          if (row != 0)
          {
            nextIdx = idx - 8;
            if (!this->pieces[nextIdx] || this->pieces[nextIdx]->color != this->turn)
            {
              directions[1] = nextIdx;
            }
          }
          if (row != 0 && col != 7)
          {
            nextIdx = idx - 7;
            if (!this->pieces[nextIdx] || this->pieces[nextIdx]->color != this->turn)
            {
              directions[2] = nextIdx;
            }
          }
          if (col != 0)
          {
            nextIdx = idx - 1;
            if (!this->pieces[nextIdx] || this->pieces[nextIdx]->color != this->turn)
            {
              directions[3] = nextIdx;
            }
          }
          if (col != 7)
          {
            nextIdx = idx + 1;
            if (!this->pieces[nextIdx] || this->pieces[nextIdx]->color != this->turn)
            {
              directions[4] = nextIdx;
            }
          }
          if (row != 7 && col != 0)
          {
            nextIdx = idx + 7;
            if (!this->pieces[nextIdx] || this->pieces[nextIdx]->color != this->turn)
            {
              directions[5] = nextIdx;
            }
          }
          if (row != 7)
          {
            nextIdx = idx + 8;
            if (!this->pieces[nextIdx] || this->pieces[nextIdx]->color != this->turn)
            {
              directions[6] = nextIdx;
            }
          }
          if (row != 7 && col != 7)
          {
            nextIdx = idx + 9;
            if (!this->pieces[nextIdx] || this->pieces[nextIdx]->color != this->turn)
            {
              directions[7] = nextIdx;
            }
          }
          for (auto i = 0; i < 8; ++i)
          {
            if (directions[i] != -1)
            {
              Chess *nextMoveGame = this->MovePiece(idx, directions[i], false);
              if (this->turn == Piece::Color::WHITE)
              {
                if (nextMoveGame->wCastle)
                {
                  nextMoveGame->wCastle = false;
                }
                if (nextMoveGame->wQueenCastle)
                {
                  nextMoveGame->wQueenCastle = false;
                }
              }
              else
              {
                if (nextMoveGame->bCastle)
                {
                  nextMoveGame->bCastle = false;
                }
                if (nextMoveGame->bQueenCastle)
                {
                  nextMoveGame->bQueenCastle = false;
                }
              }
              fenString = nextMoveGame->BoardIdx();
              nextMoveGame->occurrences[fenString]++;
              pseudoLegalMoves.push_back(nextMoveGame);
            }
          }
          break;
        }
      }
    }
  }
  return pseudoLegalMoves;
}

std::vector<Chess *> Chess::LegalMoves()
{
  std::string fenString = "";
  std::vector<Chess *> legalMoves;
  // Castling
  if (this->turn == Piece::Color::WHITE)
  {
    if (this->wCastle && !this->pieces[5] && !this->pieces[6] && !this->InCheck(4, Piece::Color::WHITE) && !this->InCheck(5, Piece::Color::WHITE) && !this->InCheck(6, Piece::Color::WHITE))
    {
      Chess *nextMoveGame = this->MovePiece(4, 6, false);
      nextMoveGame->wCastle = false;
      if (nextMoveGame->wQueenCastle)
      {
        nextMoveGame->wQueenCastle = false;
      }
      nextMoveGame->pieces[5] = this->pieces[7];
      fenString = nextMoveGame->BoardIdx();
      nextMoveGame->occurrences[fenString]++;
      legalMoves.push_back(nextMoveGame);
    }
    if (this->wQueenCastle && !this->pieces[1] && !this->pieces[2] && !this->pieces[3] && !this->InCheck(2, Piece::Color::WHITE) && !this->InCheck(3, Piece::Color::WHITE) && !this->InCheck(4, Piece::Color::WHITE))
    {
      Chess *nextMoveGame = this->MovePiece(4, 2, false);
      nextMoveGame->wQueenCastle = false;
      if (nextMoveGame->wCastle)
      {
        nextMoveGame->wCastle = false;
      }
      nextMoveGame->pieces[3] = this->pieces[0];
      fenString = nextMoveGame->BoardIdx();
      nextMoveGame->occurrences[fenString]++;
      legalMoves.push_back(nextMoveGame);
    }
  }
  else
  {
    if (this->bCastle && !this->pieces[61] && !this->pieces[62] && !this->InCheck(60, Piece::Color::BLACK) && !this->InCheck(61, Piece::Color::BLACK) && !this->InCheck(62, Piece::Color::BLACK))
    {
      Chess *nextMoveGame = this->MovePiece(60, 62, false);
      nextMoveGame->bCastle = false;
      if (nextMoveGame->bQueenCastle)
      {
        nextMoveGame->bQueenCastle = false;
      }
      nextMoveGame->pieces[61] = this->pieces[63];
      fenString = nextMoveGame->BoardIdx();
      nextMoveGame->occurrences[fenString]++;
      legalMoves.push_back(nextMoveGame);
    }
    if (this->bQueenCastle && !this->pieces[57] && !this->pieces[58] && !this->pieces[59] && !this->InCheck(58, Piece::Color::BLACK) && !this->InCheck(59, Piece::Color::BLACK) && !this->InCheck(60, Piece::Color::BLACK))
    {
      Chess *nextMoveGame = this->MovePiece(60, 58, false);
      nextMoveGame->bQueenCastle = false;
      if (nextMoveGame->bCastle)
      {
        nextMoveGame->bCastle = false;
      }
      nextMoveGame->pieces[59] = this->pieces[56];
      fenString = nextMoveGame->BoardIdx();
      nextMoveGame->occurrences[fenString]++;
      legalMoves.push_back(nextMoveGame);
    }
  }
  short possibleGameKing = this->turn == Piece::Color::WHITE ? 4 : 60;
  for (Chess *possibleGame : this->PseudolegalMoves())
  {
    // Get King Index for color that just moved
    if (!possibleGame->pieces[possibleGameKing] || (possibleGame->pieces[possibleGameKing]->type != Piece::Type::KING) || (possibleGame->pieces[possibleGameKing]->color != this->turn))
    {
      for (auto x = 0; x < 64; ++x)
      {
        if (possibleGame->pieces[x] && (possibleGame->pieces[x]->type == Piece::Type::KING) && (possibleGame->pieces[x]->color == this->turn))
        {
          possibleGameKing = x;
          break;
        }
      }
    }
    // Check legality
    if (!possibleGame->InCheck(possibleGameKing, this->turn))
    {
      legalMoves.push_back(possibleGame);
    }
  }
  return legalMoves;
}

std::string Chess::ConvertToFEN()
{
  return this->BoardIdx() + " " + std::to_string(this->lastPawnOrTake) + " " + std::to_string(this->fullTurns);
}

std::string Chess::BoardIdx()
{
  std::string fenString = "";

  // first part: board representation
  short blankSpaces = 0;
  for (auto row = 56; row > -1; row -= 8)
  {
    for (auto col = 0; col < 8; ++col)
    {
      if (!this->pieces[row + col])
      {
        ++blankSpaces;
      }
      else
      {
        if (blankSpaces != 0)
        {
          fenString += std::to_string(blankSpaces);
        }
        blankSpaces = 0;
        switch (this->pieces[row + col]->type)
        {
        case Piece::Type::PAWN:
          fenString += (this->pieces[row + col]->color == Piece::Color::WHITE) ? "P" : "p";
          break;
        case Piece::Type::KNIGHT:
          fenString += (this->pieces[row + col]->color == Piece::Color::WHITE) ? "N" : "n";
          break;
        case Piece::Type::BISHOP:
          fenString += (this->pieces[row + col]->color == Piece::Color::WHITE) ? "B" : "b";
          break;
        case Piece::Type::ROOK:
          fenString += (this->pieces[row + col]->color == Piece::Color::WHITE) ? "R" : "r";
          break;
        case Piece::Type::QUEEN:
          fenString += (this->pieces[row + col]->color == Piece::Color::WHITE) ? "Q" : "q";
          break;
        default:
          fenString += (this->pieces[row + col]->color == Piece::Color::WHITE) ? "K" : "k";
          break;
        }
      }
    }
    if (blankSpaces != 0)
    {
      fenString += std::to_string(blankSpaces);
      blankSpaces = 0;
    }
    if (row != 0)
    {
      fenString += "/";
    }
  }

  // second part: turn
  fenString += (this->turn == Piece::Color::WHITE) ? " w " : " b ";

  // third part: castling rights
  if (!this->wCastle && !this->wQueenCastle && !this->bCastle && !this->bQueenCastle)
  {
    fenString += "- ";
  }
  else
  {
    if (this->wCastle)
    {
      fenString += "K";
    }
    if (this->wQueenCastle)
    {
      fenString += "Q";
    }
    if (this->bCastle)
    {
      fenString += "k";
    }
    if (this->bQueenCastle)
    {
      fenString += "q";
    }
    fenString += " ";
  }

  // fourth part: en passant square
  if (this->enPassantIdx == -1)
  {
    fenString += "-";
  }
  else
  {
    switch (this->enPassantIdx % 8)
    {
    case 0:
      fenString += "a";
      break;
    case 1:
      fenString += "b";
      break;
    case 2:
      fenString += "c";
      break;
    case 3:
      fenString += "d";
      break;
    case 4:
      fenString += "e";
      break;
    case 5:
      fenString += "f";
      break;
    case 6:
      fenString += "g";
      break;
    default:
      fenString += "h";
      break;
    }
    fenString += (this->enPassantIdx < 32) ? "3" : "6";
  }
  return fenString;
}

void Chess::PrintBoard()
{
  for (auto row = 7; row > -1; --row)
  {
    std::cout << "-----------------" << std::endl
              << "|";
    for (auto col = 0; col < 8; ++col)
    {
      auto currIdx = (row * 8) + col;
      std::string currSquare = " ";
      if (this->pieces[currIdx])
      {
        switch (this->pieces[currIdx]->type)
        {
        case Piece::Type::PAWN:
          currSquare = this->pieces[currIdx]->color ? "P" : "p";
          break;
        case Piece::Type::KNIGHT:
          currSquare = this->pieces[currIdx]->color ? "N" : "n";
          break;
        case Piece::Type::BISHOP:
          currSquare = this->pieces[currIdx]->color ? "B" : "b";
          break;
        case Piece::Type::ROOK:
          currSquare = this->pieces[currIdx]->color ? "R" : "r";
          break;
        case Piece::Type::QUEEN:
          currSquare = this->pieces[currIdx]->color ? "Q" : "q";
          break;
        default:
          currSquare = this->pieces[currIdx]->color ? "K" : "k";
          break;
        }
      }
      std::cout << currSquare << "|";
    }
    std::cout << std::endl;
  }
  std::cout << "-----------------" << std::endl;
}