#include "chess.h"
#include <iostream>
#include <bitset>

uint64_t Chess::KNIGHT_MOVES[64] = {};
uint64_t Chess::KING_MOVES[64] = {};
ankerl::unordered_dense::map<int, uint64_t> Chess::BISHOP_MOVES[64] = {};
ankerl::unordered_dense::map<int, uint64_t> Chess::ROOK_MOVES[64] = {};
ankerl::unordered_dense::map<int, uint64_t> Chess::QUEEN_MOVES[64] = {};

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
  // Initialize knight and king moves
  for (uint64_t i = 0, idx = 1; i < 64; ++i, idx <<= 1)
  {
    KNIGHT_MOVES[i] = 0;
    KNIGHT_MOVES[i] = down(downleft(idx)) | down(downright(idx)) | left(downleft(idx)) | right(downright(idx)) | up(upleft(idx)) | up(upright(idx)) | left(upleft(idx)) | right(upright(idx));
    KING_MOVES[i] = 0;
    KING_MOVES[i] = downleft(idx) | down(idx) | downright(idx) | left(idx) | right(idx) | upleft(idx) | up(idx) | upright(idx);
  }
}

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

bool Chess::InCheck(const Color kingColor, const short kingIdx)
{
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

// This function flips the turn and increments, increments/resets lastPawnOrTake, updates castling on taking a rook, updates en passant on moving pawn 2 squares
// and updates the piece bitboards (including when taking en passant)
// This function doesn't handle castling (maybe happens at some point)
void Chess::MovePiece(const char pieceType, const short start, const short end, uint64_t opponent)
{
  switch (pieceType)
  {
  case 'P':
    set_bit(this->wPawns, end);
    clear_bit(this->wPawns, end);
    this->lastPawnOrTake = 0;
    break;
  case 'N':
    set_bit(this->wKnights, end);
    clear_bit(this->wKnights, end);
    break;
  case 'B':
    set_bit(this->wBishops, end);
    clear_bit(this->wBishops, end);
    break;
  case 'R':
    set_bit(this->wRooks, end);
    clear_bit(this->wRooks, end);
    break;
  case 'Q':
    set_bit(this->wQueens, end);
    clear_bit(this->wQueens, end);
    break;
  case 'K':
    set_bit(this->wKing, end);
    clear_bit(this->wKing, end);
    break;
  case 'p':
    set_bit(this->bPawns, end);
    clear_bit(this->bPawns, end);
    this->lastPawnOrTake = 0;
    break;
  case 'n':
    set_bit(this->bKnights, end);
    clear_bit(this->bKnights, end);
    break;
  case 'b':
    set_bit(this->bBishops, end);
    clear_bit(this->bBishops, end);
    break;
  case 'r':
    set_bit(this->bRooks, end);
    clear_bit(this->bRooks, end);
    break;
  case 'q':
    set_bit(this->bQueens, end);
    clear_bit(this->bQueens, end);
    break;
  default:
    set_bit(this->bKing, end);
    clear_bit(this->bKing, end);
    break;
  }
  if (this->turn == Color::BLACK)
  {
    ++this->fullTurns;
  }
  // Flip turn
  this->turn = (Color) !this->turn;
  // Set en passant to -1
  if (this->enPassantIdx != -1)
  {
    this->enPassantIdx = -1;
  }
  // If we take, set opponent bit to 0 and lastPawnOrTake to 0
  if (get_bit(opponent, end))
  {
    clear_bit(opponent, end);
    this->lastPawnOrTake = 0;
    // Updating the other colors to remove the taken piece
    if (this->turn == Color::WHITE)
    {
      // White Takes En Passant
      if (this->enPassantIdx == end)
      {
        clear_bit(this->bPawns, end - 8);
      }
      else
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
    }
    else
    {
      // Black Takes En Passant
      if (this->enPassantIdx == end)
      {
        clear_bit(this->wPawns, end + 8);
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
          this->bCastle = false;
        }
        else if (this->bQueenCastle && end == 0)
        {
          this->bQueenCastle = false;
        }
      }
    }
  }
  else
  {
    ++this->lastPawnOrTake;
  }
}

std::vector<Chess *> Chess::PseudoLegalMoves()
{
  std::vector<Chess *> pseudolegalMoves;
  return pseudolegalMoves;
}

uint64_t Chess::perft(int depth)
{
  // Base case
  if (depth == 0)
  {
    uint64_t tempKing = 0ULL;
    if (this->turn == Color::WHITE)
    {
      tempKing = this->bKing;
    }
    else
    {
      tempKing = this->wKing;
    }
    return this->InCheck((Color)(!this->turn), pop_lsb(tempKing)) ? 0 : 1;
  }
  uint64_t nodes = 0;
  // targets is a bitboard of available squares
  uint64_t targets = 0ULL;
  // opponent is the non-turn pieces
  uint64_t opponent = 0ULL;

  // Copy of the current game for iterating over
  Chess gameCopy(*this);
  int currIdx = -1;

  // White Turn
  if (this->turn == Color::WHITE)
  {
    targets = ~(this->whites());
    opponent = this->blacks();
    uint64_t enPassantMask = (this->enPassantIdx == -1) ? 0ULL : (1ULL << this->enPassantIdx);
    // White Pawns Advancing (Not Promoting)
    uint64_t upPawns = up(this->wPawns) & targets & ~opponent & ~RANK_8;
    while (upPawns)
    {
      // Grabbing current piece
      int nextIdx = pop_lsb(upPawns);
      Chess nextMoveGame(*this);
      nextMoveGame.MovePiece('P', nextIdx - 8, nextIdx, opponent);
      nodes += nextMoveGame.perft(depth - 1);
    }
    // White Pawns Advancing 2 squares
    uint64_t twoSquarePawns = up(up(this->wPawns & RANK_2)) & targets & ~opponent;
    while (twoSquarePawns)
    {
      int nextIdx = pop_lsb(twoSquarePawns);
      Chess nextMoveGame(*this);
      nextMoveGame.MovePiece('P', nextIdx - 16, nextIdx, opponent);
      nextMoveGame.enPassantIdx = nextIdx - 8;
      nodes += nextMoveGame.perft(depth - 1);
    }
    // White Pawns Taking Left (Not Promoting)
    uint64_t upleftPawns = upleft(this->wPawns) & opponent & ~RANK_8;
    while (upleftPawns)
    {
      int nextIdx = pop_lsb(upleftPawns);
      Chess nextMoveGame(*this);
      nextMoveGame.MovePiece('P', nextIdx - 9, nextIdx, opponent);
      nodes += nextMoveGame.perft(depth - 1);
    }
    // White Pawns Taking Right (Not Promoting)
    uint64_t uprightPawns = upright(this->wPawns) & opponent & ~RANK_8;
    while (uprightPawns)
    {
      int nextIdx = pop_lsb(uprightPawns);
      Chess nextMoveGame(*this);
      nextMoveGame.MovePiece('P', nextIdx - 7, nextIdx, opponent);
      nodes += nextMoveGame.perft(depth - 1);
    }
    uint64_t upPromotions = up(this->wPawns) & targets & ~opponent & RANK_8;
    while (upPromotions)
    {
      int nextIdx = pop_lsb(upPromotions);
      // Queen Promotion
      Chess nextMoveGame(*this);
      nextMoveGame.MovePiece('P', nextIdx - 8, nextIdx, opponent);
      clear_bit(nextMoveGame.wPawns, nextIdx);
      set_bit(nextMoveGame.wQueens, nextIdx);
      nodes += nextMoveGame.perft(depth - 1);

      // Rook Promotion
      nextMoveGame = Chess(*this);
      nextMoveGame.MovePiece('P', nextIdx - 8, nextIdx, opponent);
      clear_bit(nextMoveGame.wPawns, nextIdx);
      set_bit(nextMoveGame.wRooks, nextIdx);
      nodes += nextMoveGame.perft(depth - 1);

      // Knight Promotion
      nextMoveGame = Chess(*this);
      nextMoveGame.MovePiece('P', nextIdx - 8, nextIdx, opponent);
      clear_bit(nextMoveGame.wPawns, nextIdx);
      set_bit(nextMoveGame.wKnights, nextIdx);
      nodes += nextMoveGame.perft(depth - 1);

      // Bishop Promotion
      nextMoveGame = Chess(*this);
      nextMoveGame.MovePiece('P', nextIdx - 8, nextIdx, opponent);
      clear_bit(nextMoveGame.wPawns, nextIdx);
      set_bit(nextMoveGame.wBishops, nextIdx);
      nodes += nextMoveGame.perft(depth - 1);
    }

    uint64_t upleftPromotions = upleft(this->wPawns) & opponent & RANK_8;
    while (upleftPromotions)
    {
      int nextIdx = pop_lsb(upleftPromotions);
      // Queen Promotion
      Chess nextMoveGame(*this);
      nextMoveGame.MovePiece('P', nextIdx - 7, nextIdx, opponent);
      clear_bit(nextMoveGame.wPawns, nextIdx);
      set_bit(nextMoveGame.wQueens, nextIdx);
      nodes += nextMoveGame.perft(depth - 1);

      // Rook Promotion
      nextMoveGame = Chess(*this);
      nextMoveGame.MovePiece('P', nextIdx - 7, nextIdx, opponent);
      clear_bit(nextMoveGame.wPawns, nextIdx);
      set_bit(nextMoveGame.wRooks, nextIdx);
      nodes += nextMoveGame.perft(depth - 1);

      // Knight Promotion
      nextMoveGame = Chess(*this);
      nextMoveGame.MovePiece('P', nextIdx - 7, nextIdx, opponent);
      clear_bit(nextMoveGame.wPawns, nextIdx);
      set_bit(nextMoveGame.wKnights, nextIdx);
      nodes += nextMoveGame.perft(depth - 1);

      // Bishop Promotion
      nextMoveGame = Chess(*this);
      nextMoveGame.MovePiece('P', nextIdx - 7, nextIdx, opponent);
      clear_bit(nextMoveGame.wPawns, nextIdx);
      set_bit(nextMoveGame.wBishops, nextIdx);
      nodes += nextMoveGame.perft(depth - 1);
    }

    uint64_t uprightPromotions = upright(this->wPawns) & opponent & RANK_8;
    while (uprightPromotions)
    {
      int nextIdx = pop_lsb(uprightPromotions);
      // Queen Promotion
      Chess nextMoveGame(*this);
      nextMoveGame.MovePiece('P', nextIdx - 9, nextIdx, opponent);
      clear_bit(nextMoveGame.wPawns, nextIdx);
      set_bit(nextMoveGame.wQueens, nextIdx);
      nodes += nextMoveGame.perft(depth - 1);

      // Rook Promotion
      nextMoveGame = Chess(*this);
      nextMoveGame.MovePiece('P', nextIdx - 9, nextIdx, opponent);
      clear_bit(nextMoveGame.wPawns, nextIdx);
      set_bit(nextMoveGame.wRooks, nextIdx);
      nodes += nextMoveGame.perft(depth - 1);

      // Knight Promotion
      nextMoveGame = Chess(*this);
      nextMoveGame.MovePiece('P', nextIdx - 9, nextIdx, opponent);
      clear_bit(nextMoveGame.wPawns, nextIdx);
      set_bit(nextMoveGame.wKnights, nextIdx);
      nodes += nextMoveGame.perft(depth - 1);

      // Bishop Promotion
      nextMoveGame = Chess(*this);
      nextMoveGame.MovePiece('P', nextIdx - 9, nextIdx, opponent);
      clear_bit(nextMoveGame.wPawns, nextIdx);
      set_bit(nextMoveGame.wBishops, nextIdx);
      nodes += nextMoveGame.perft(depth - 1);
    }

    // White Left En Passant
    uint64_t leftEnPassant = upleft(this->wPawns) & enPassantMask;
    if (leftEnPassant)
    {
      Chess nextMoveGame(*this);
      nextMoveGame.MovePiece('P', this->enPassantIdx - 7, this->enPassantIdx, opponent);
      nodes += nextMoveGame.perft(depth - 1);
    }

    // White Right En Passant
    uint64_t rightEnPassant = upright(this->wPawns) & enPassantMask;
    if (rightEnPassant)
    {
      Chess nextMoveGame(*this);
      nextMoveGame.MovePiece('P', this->enPassantIdx - 9, this->enPassantIdx, opponent);
      nodes += nextMoveGame.perft(depth - 1);
    }

    // White Knights
    while (gameCopy.wKnights)
    {
      // Grabbing current piece
      currIdx = pop_lsb(gameCopy.wKnights);
      uint64_t possibleMoves = this->KNIGHT_MOVES[currIdx] & targets;
      while (possibleMoves)
      {
        // Next possible move
        int nextIdx = pop_lsb(possibleMoves);
        // Initialize next move game
        Chess nextMoveGame(*this);
        nextMoveGame.MovePiece('N', currIdx, nextIdx, opponent);
        nodes += nextMoveGame.perft(depth - 1);
      }
    }
    // White Bishops
    while (gameCopy.wBishops)
    {
      // Grabbing current piece
      currIdx = pop_lsb(gameCopy.wBishops);
      // All the possible moves based on the hash
      // (Use targets in the hash instead of blockers??)
      uint64_t possibleMoves = this->BISHOP_MOVES[currIdx][BishopHash(currIdx, this->whites())] & targets;
      while (possibleMoves)
      {
        // Next possible move
        int nextIdx = pop_lsb(possibleMoves);
        // Initialize next move game
        Chess nextMoveGame(*this);
        nextMoveGame.MovePiece('B', currIdx, nextIdx, opponent);
        nodes += nextMoveGame.perft(depth - 1);
      }
    }
    // White Rooks
    while (gameCopy.wRooks)
    {
      // Grabbing current piece
      currIdx = pop_lsb(gameCopy.wRooks);
      // All the possible moves based on the hash
      // (Use targets in the hash instead of blockers??)
      uint64_t possibleMoves = this->ROOK_MOVES[currIdx][RookHash(currIdx, this->whites())] & targets;
      while (possibleMoves)
      {
        // Next possible move
        int nextIdx = pop_lsb(possibleMoves);
        // Initialize next move game
        Chess nextMoveGame(*this);
        if (currIdx == 0 && nextMoveGame.wQueenCastle)
        {
          nextMoveGame.wQueenCastle = false;
        }
        else if (currIdx == 7 && nextMoveGame.wCastle)
        {
          nextMoveGame.wCastle = false;
        }
        nextMoveGame.MovePiece('R', currIdx, nextIdx, opponent);
        nodes += nextMoveGame.perft(depth - 1);
      }
    }
    // White Queens
    while (gameCopy.wQueens)
    {
      // Grabbing current piece
      currIdx = pop_lsb(gameCopy.wQueens);
      // All the possible moves based on the hash
      // (Use targets in the hash instead of blockers??)
      uint64_t possibleMoves = this->QUEEN_MOVES[currIdx][QueenHash(currIdx, this->whites())] & targets;
      while (possibleMoves)
      {
        // Next possible move
        int nextIdx = pop_lsb(possibleMoves);
        // Initialize next move game
        Chess nextMoveGame(*this);
        nextMoveGame.MovePiece('Q', currIdx, nextIdx, opponent);
        nodes += nextMoveGame.perft(depth - 1);
      }
    }
    // White King
    currIdx = pop_lsb(gameCopy.wKing);
    uint64_t possibleMoves = this->KING_MOVES[currIdx] & targets;
    while (possibleMoves)
    {
      // Next possible move
      int nextIdx = pop_lsb(possibleMoves);
      // Initialize next move game
      Chess nextMoveGame(*this);
      if (nextMoveGame.wQueenCastle)
      {
        nextMoveGame.wQueenCastle = false;
      }
      if (nextMoveGame.wCastle)
      {
        nextMoveGame.wCastle = false;
      }
      nextMoveGame.MovePiece('K', currIdx, nextIdx, opponent);
      nodes += nextMoveGame.perft(depth - 1);
    }

    // White Castles
    // 0xEULL is 1110 aka idx 1,2,3
    if (gameCopy.wQueenCastle && (targets & ~opponent & 0xEULL) && !this->InCheck(Color::WHITE, 4) && !this->InCheck(Color::WHITE, 3) && !this->InCheck(Color::WHITE, 2))
    {
      Chess nextMoveGame(*this);
      nextMoveGame.MovePiece('K', 4, 2, opponent);
      nextMoveGame.wCastle = false;
      nextMoveGame.wQueenCastle = false;
      clear_bit(nextMoveGame.wRooks, 0);
      set_bit(nextMoveGame.wRooks, 3);
      nodes += nextMoveGame.perft(depth - 1);
    }
    // 0x60ULL is 01100000 aka idx 5,6
    if (gameCopy.wCastle && (targets & ~opponent & 0x60ULL) && !this->InCheck(Color::WHITE, 4) && !this->InCheck(Color::WHITE, 5) && !this->InCheck(Color::WHITE, 6))
    {
      Chess nextMoveGame(*this);
      nextMoveGame.MovePiece('K', 4, 6, opponent);
      nextMoveGame.wCastle = false;
      nextMoveGame.wQueenCastle = false;
      clear_bit(nextMoveGame.wRooks, 7);
      set_bit(nextMoveGame.wRooks, 5);
      nodes += nextMoveGame.perft(depth - 1);
    }
  }
  // Black Turn
  else
  {
    targets = ~(this->blacks());
    opponent = this->whites();
    uint64_t enPassantMask = (this->enPassantIdx == -1) ? 0ULL : (1ULL << this->enPassantIdx);
    // White Pawns Advancing (Not Promoting)
    uint64_t downPawns = down(this->bPawns) & targets & ~opponent & ~RANK_1;
    while (downPawns)
    {
      // Grabbing current piece
      int nextIdx = pop_lsb(downPawns);
      Chess nextMoveGame(*this);
      nextMoveGame.MovePiece('p', nextIdx + 8, nextIdx, opponent);
      nodes += nextMoveGame.perft(depth - 1);
    }
    // White Pawns Advancing 2 squares
    uint64_t twoSquarePawns = down(down(this->bPawns & RANK_7)) & targets & ~opponent;
    while (twoSquarePawns)
    {
      int nextIdx = pop_lsb(twoSquarePawns);
      Chess nextMoveGame(*this);
      nextMoveGame.MovePiece('p', nextIdx + 16, nextIdx, opponent);
      nextMoveGame.enPassantIdx = nextIdx + 8;
      nodes += nextMoveGame.perft(depth - 1);
    }
    // White Pawns Taking Left (Not Promoting)
    uint64_t downleftPawns = downleft(this->bPawns) & opponent & ~RANK_1;
    while (downleftPawns)
    {
      int nextIdx = pop_lsb(downleftPawns);
      Chess nextMoveGame(*this);
      nextMoveGame.MovePiece('p', nextIdx + 9, nextIdx, opponent);
      nodes += nextMoveGame.perft(depth - 1);
    }
    // White Pawns Taking Right (Not Promoting)
    uint64_t downrightPawns = downright(this->bPawns) & opponent & ~RANK_1;
    while (downrightPawns)
    {
      int nextIdx = pop_lsb(downrightPawns);
      Chess nextMoveGame(*this);
      nextMoveGame.MovePiece('p', nextIdx + 7, nextIdx, opponent);
      nodes += nextMoveGame.perft(depth - 1);
    }
    uint64_t downPromotions = down(this->bPawns) & targets & ~opponent & RANK_1;
    while (downPromotions)
    {
      int nextIdx = pop_lsb(downPromotions);
      // Queen Promotion
      Chess nextMoveGame(*this);
      nextMoveGame.MovePiece('p', nextIdx + 8, nextIdx, opponent);
      clear_bit(nextMoveGame.bPawns, nextIdx);
      set_bit(nextMoveGame.bQueens, nextIdx);
      nodes += nextMoveGame.perft(depth - 1);

      // Rook Promotion
      nextMoveGame = Chess(*this);
      nextMoveGame.MovePiece('p', nextIdx + 8, nextIdx, opponent);
      clear_bit(nextMoveGame.bPawns, nextIdx);
      set_bit(nextMoveGame.bRooks, nextIdx);
      nodes += nextMoveGame.perft(depth - 1);

      // Knight Promotion
      nextMoveGame = Chess(*this);
      nextMoveGame.MovePiece('p', nextIdx + 8, nextIdx, opponent);
      clear_bit(nextMoveGame.bPawns, nextIdx);
      set_bit(nextMoveGame.bKnights, nextIdx);
      nodes += nextMoveGame.perft(depth - 1);

      // Bishop Promotion
      nextMoveGame = Chess(*this);
      nextMoveGame.MovePiece('p', nextIdx + 8, nextIdx, opponent);
      clear_bit(nextMoveGame.bPawns, nextIdx);
      set_bit(nextMoveGame.bBishops, nextIdx);
      nodes += nextMoveGame.perft(depth - 1);
    }

    uint64_t downLeftPromotions = downleft(this->bPawns) & opponent & RANK_1;
    while (downLeftPromotions)
    {
      int nextIdx = pop_lsb(downLeftPromotions);
      // Queen Promotion
      Chess nextMoveGame(*this);
      nextMoveGame.MovePiece('p', nextIdx + 9, nextIdx, opponent);
      clear_bit(nextMoveGame.bPawns, nextIdx);
      set_bit(nextMoveGame.bQueens, nextIdx);
      nodes += nextMoveGame.perft(depth - 1);

      // Rook Promotion
      nextMoveGame = Chess(*this);
      nextMoveGame.MovePiece('p', nextIdx + 9, nextIdx, opponent);
      clear_bit(nextMoveGame.bPawns, nextIdx);
      set_bit(nextMoveGame.bRooks, nextIdx);
      nodes += nextMoveGame.perft(depth - 1);

      // Knight Promotion
      nextMoveGame = Chess(*this);
      nextMoveGame.MovePiece('p', nextIdx + 9, nextIdx, opponent);
      clear_bit(nextMoveGame.bPawns, nextIdx);
      set_bit(nextMoveGame.bKnights, nextIdx);
      nodes += nextMoveGame.perft(depth - 1);

      // Bishop Promotion
      nextMoveGame = Chess(*this);
      nextMoveGame.MovePiece('p', nextIdx + 9, nextIdx, opponent);
      clear_bit(nextMoveGame.bPawns, nextIdx);
      set_bit(nextMoveGame.bBishops, nextIdx);
      nodes += nextMoveGame.perft(depth - 1);
    }

    uint64_t downRightPromotions = downright(this->bPawns) & opponent & RANK_1;
    while (downRightPromotions)
    {
      int nextIdx = pop_lsb(downRightPromotions);
      // Queen Promotion
      Chess nextMoveGame(*this);
      nextMoveGame.MovePiece('p', nextIdx + 7, nextIdx, opponent);
      clear_bit(nextMoveGame.bPawns, nextIdx);
      set_bit(nextMoveGame.bQueens, nextIdx);
      nodes += nextMoveGame.perft(depth - 1);

      // Rook Promotion
      nextMoveGame = Chess(*this);
      nextMoveGame.MovePiece('p', nextIdx + 7, nextIdx, opponent);
      clear_bit(nextMoveGame.bPawns, nextIdx);
      set_bit(nextMoveGame.bRooks, nextIdx);
      nodes += nextMoveGame.perft(depth - 1);

      // Knight Promotion
      nextMoveGame = Chess(*this);
      nextMoveGame.MovePiece('p', nextIdx + 7, nextIdx, opponent);
      clear_bit(nextMoveGame.bPawns, nextIdx);
      set_bit(nextMoveGame.bKnights, nextIdx);
      nodes += nextMoveGame.perft(depth - 1);

      // Bishop Promotion
      nextMoveGame = Chess(*this);
      nextMoveGame.MovePiece('p', nextIdx + 7, nextIdx, opponent);
      clear_bit(nextMoveGame.bPawns, nextIdx);
      set_bit(nextMoveGame.bBishops, nextIdx);
      nodes += nextMoveGame.perft(depth - 1);
    }

    uint64_t leftEnPassant = downleft(this->bPawns) & enPassantMask;
    if (leftEnPassant)
    {
      Chess nextMoveGame(*this);
      nextMoveGame.MovePiece('p', enPassantIdx + 9, enPassantIdx, opponent);
      nodes += nextMoveGame.perft(depth - 1);
    }
    uint64_t rightEnPassant = downright(this->bPawns) & enPassantMask;
    if (rightEnPassant)
    {
      Chess nextMoveGame(*this);
      nextMoveGame.MovePiece('p', enPassantIdx + 7, enPassantIdx, opponent);
      nodes += nextMoveGame.perft(depth - 1);
    }
    // Black Knights
    while (gameCopy.bKnights)
    {
      // Grabbing current piece
      currIdx = pop_lsb(gameCopy.bKnights);
      uint64_t possibleMoves = this->KNIGHT_MOVES[currIdx] & targets;
      while (possibleMoves)
      {
        // Next possible move
        int nextIdx = pop_lsb(possibleMoves);
        // Initialize next move game
        Chess nextMoveGame(*this);
        nextMoveGame.MovePiece('n', currIdx, nextIdx, opponent);
        nodes += nextMoveGame.perft(depth - 1);
      }
    }
    // Black Bishops
    while (gameCopy.bBishops)
    {
      // Grabbing current piece
      currIdx = pop_lsb(gameCopy.bBishops);
      uint64_t possibleMoves = this->BISHOP_MOVES[currIdx][BishopHash(currIdx, this->blacks())] & targets;
      while (possibleMoves)
      {
        // Next possible move
        int nextIdx = pop_lsb(possibleMoves);
        // Initialize next move game
        Chess nextMoveGame(*this);
        nextMoveGame.MovePiece('b', currIdx, nextIdx, opponent);
        nodes += nextMoveGame.perft(depth - 1);
      }
    }
    // Black Rooks
    while (gameCopy.bRooks)
    {
      // Grabbing current piece
      currIdx = pop_lsb(gameCopy.bRooks);
      uint64_t possibleMoves = this->ROOK_MOVES[currIdx][RookHash(currIdx, this->blacks())] & targets;
      while (possibleMoves)
      {
        // Next possible move
        int nextIdx = pop_lsb(possibleMoves);
        // Initialize next move game
        Chess nextMoveGame(*this);
        if (currIdx == 56 && nextMoveGame.bQueenCastle)
        {
          nextMoveGame.bQueenCastle = false;
        }
        else if (currIdx == 63 && nextMoveGame.bCastle)
        {
          nextMoveGame.bCastle = false;
        }
        nextMoveGame.MovePiece('r', currIdx, nextIdx, opponent);
        nodes += nextMoveGame.perft(depth - 1);
      }
    }
    // Black Queens
    while (gameCopy.bQueens)
    {
      // Grabbing current piece
      currIdx = pop_lsb(gameCopy.bQueens);
      uint64_t possibleMoves = this->QUEEN_MOVES[currIdx][QueenHash(currIdx, this->blacks())] & targets;
      while (possibleMoves)
      {
        // Next possible move
        int nextIdx = pop_lsb(possibleMoves);
        // Initialize next move game
        Chess nextMoveGame(*this);
        nextMoveGame.MovePiece('q', currIdx, nextIdx, opponent);
        nodes += nextMoveGame.perft(depth - 1);
      }
    }
    // Black King
    currIdx = pop_lsb(gameCopy.bKing);
    uint64_t possibleMoves = this->KING_MOVES[currIdx] & targets;
    while (possibleMoves)
    {
      // Next possible move
      int nextIdx = pop_lsb(possibleMoves);
      // Initialize next move game
      Chess nextMoveGame(*this);
      if (nextMoveGame.bQueenCastle)
      {
        nextMoveGame.bQueenCastle = false;
      }
      if (nextMoveGame.bCastle)
      {
        nextMoveGame.bCastle = false;
      }
      nextMoveGame.MovePiece('k', currIdx, nextIdx, opponent);
      nodes += nextMoveGame.perft(depth - 1);
    }

    // Black Castles
    // 0xE00000000000000ULL is 1110 00000000 00000000 00000000 00000000 00000000 00000000 00000000 aka idx 58, 59, 60
    if (gameCopy.bQueenCastle && (targets & ~opponent & 0xE00000000000000ULL) && !this->InCheck(Color::BLACK, 58) && !this->InCheck(Color::BLACK, 59) && !this->InCheck(Color::BLACK, 60))
    {
      Chess nextMoveGame(*this);
      nextMoveGame.MovePiece('k', 60, 58, opponent);
      nextMoveGame.bCastle = false;
      nextMoveGame.bQueenCastle = false;
      clear_bit(nextMoveGame.bRooks, 0);
      set_bit(nextMoveGame.bRooks, 3);
      nodes += nextMoveGame.perft(depth - 1);
    }
    // 0x6000000000000000ULL is 01100000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 aka idx 5,6
    if (gameCopy.bCastle && (targets & ~opponent & 0x6000000000000000ULL) && !this->InCheck(Color::BLACK, 60) && !this->InCheck(Color::BLACK, 61) && !this->InCheck(Color::BLACK, 62))
    {
      Chess nextMoveGame(*this);
      nextMoveGame.MovePiece('k', 60, 62, opponent);
      nextMoveGame.bCastle = false;
      nextMoveGame.bQueenCastle = false;
      clear_bit(nextMoveGame.bRooks, 7);
      set_bit(nextMoveGame.bRooks, 5);
      nodes += nextMoveGame.perft(depth - 1);
    }
  }
  return nodes;
}

void Chess::divide(int depth)
{
  return;
}