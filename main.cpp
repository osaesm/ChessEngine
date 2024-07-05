#include <iostream>

#include "chess.h"

int main()
{
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
  int totalRookBoards = 0;
  int totalBishopBoards = 0;
  int totalQueenBoards = 0;
  for (int i = 0; i < 64; ++i) {
    totalRookBoards += ROOK_MOVES[i].size();
    totalBishopBoards += BISHOP_MOVES[i].size();
    totalQueenBoards += QUEEN_MOVES[i].size();
  }
  std::cout << totalRookBoards << std::endl;
  std::cout << totalBishopBoards << std::endl;
  std::cout << totalQueenBoards << std::endl;

  Chess * game = new Chess("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 0");
  std::cout << game->ConvertToFEN() << std::endl;
  delete game;
  return 0;
}