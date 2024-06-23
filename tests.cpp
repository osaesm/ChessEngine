#include <iostream>

#include "chess.h"

int testOne(const int nodesLen)
{
  Chess *currGame = new Chess("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 0");
  PerftResults expectedResults[nodesLen] = {
    PerftResults(1, 0, 0, 0, 0, 0),
    PerftResults(20, 0, 0, 0, 0, 0),
    PerftResults(400, 0, 0, 0, 0, 0),
    PerftResults(8902, 34, 0, 0, 0, 12),
    PerftResults(197281, 1576, 0, 0, 0, 469),
    PerftResults(4865609, 82719, 258, 0, 0, 27351),
    PerftResults(119060324, 2812008, 5248, 0, 0, 809099),
  };
  for (auto x = 0; x < nodesLen; ++x)
  {
    PerftResults result = currGame->Perft(x);
    if (result != expectedResults[x])
    {
      std::cout << "Failed Test One" << std::endl
                << "Expected:\t" << expectedResults[x].toString() << std::endl
                << "Actual:\t\t" << result.toString() << std::endl;
      return x;
    }
  }
  currGame->ClearPieces();
  delete currGame;
  return nodesLen;
}

int testTwo(const int nodesLen)
{
  Chess *currGame = new Chess("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 0");
  PerftResults expectedResults[nodesLen] = {
    PerftResults(1, 0, 0, 0, 0, 0),
    PerftResults(48, 8, 0, 2, 0, 0),
    PerftResults(2039, 351, 1, 91, 0, 3),
    PerftResults(97862, 17102, 45, 3162, 0, 993),
    PerftResults(4085603, 757163, 1929, 128013, 15172, 25523),
    PerftResults(193690690, 35043416, 73365, 4993637, 8392, 3309887),
    PerftResults(8031647685, 1558445089, 3577504, 184513607, 56627920, 92238050),
  };
  for (auto x = 0; x < nodesLen; ++x)
  {
    PerftResults result = currGame->Perft(x);
    if (result != expectedResults[x])
    {
      std::cout << "Failed Test Two" << std::endl
                << "Expected:\t" << expectedResults[x].toString() << std::endl
                << "Actual:\t\t" << result.toString() << std::endl;
      return x;
    }
  }
  currGame->ClearPieces();
  delete currGame;
  return nodesLen;
}

int testThree(const int nodesLen)
{
  Chess *currGame = new Chess("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 0");
  PerftResults expectedResults[nodesLen] = {
    PerftResults(1, 0, 0, 0, 0, 0),
    PerftResults(14, 1, 0, 0, 0, 2),
    PerftResults(191, 14, 0, 0, 0, 10),
    PerftResults(2812, 209, 2, 0, 0, 267),
    PerftResults(43238, 3348, 123, 0, 0, 1680),
    PerftResults(674624, 52051, 1165, 0, 0, 52950),
    PerftResults(11030083, 940350, 33325, 0, 7552, 452473),
    PerftResults(178633661, 14519036, 294874, 0, 140024, 12797406),
    PerftResults(3009794393, 267586558, 8009239, 0, 6578076, 135626805),
  };
  for (auto x = 0; x < nodesLen; ++x)
  {
    PerftResults result = currGame->Perft(x);
    if (result != expectedResults[x])
    {
      std::cout << "Failed Test Three" << std::endl
                << "Expected:\t" << expectedResults[x].toString() << std::endl
                << "Actual:\t\t" << result.toString() << std::endl;
      return x;
    }
  }
  currGame->ClearPieces();
  delete currGame;
  return nodesLen;
}

int testFour(const int nodesLen)
{
  Chess *currGame = new Chess("r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1");
  PerftResults expectedResults[nodesLen] = {
    PerftResults(1, 0, 0, 0, 0, 1),
    PerftResults(6, 0, 0, 0, 0, 0),
    PerftResults(264, 87, 0, 6, 48, 10),
    PerftResults(9467, 1021, 4, 0, 120, 38),
    PerftResults(422333, 131393, 0, 7795, 60032, 15492),
    PerftResults(15833292, 2046173, 6512, 0, 329464, 200568),
    PerftResults(706045033, 210369132, 212, 10882006, 81102984, 26973664),
  };
  for (auto x = 0; x < nodesLen; ++x)
  {
    PerftResults result = currGame->Perft(x);
    if (result != expectedResults[x])
    {
      std::cout << "Failed Test Four" << std::endl
                << "Expected:\t" << expectedResults[x].toString() << std::endl
                << "Actual:\t\t" << result.toString() << std::endl;
      return x;
    }
  }
  currGame->ClearPieces();
  delete currGame;
  return nodesLen;
}

int main()
{
  int resultsOne = testOne(7);
  if (resultsOne < 7)
  {
    std::cout << "Failed Test One: " << resultsOne << "/" << 7 << std::endl;
  }
  else
  {
    std::cout << "Passed Test One!" << std::endl;
  }
  int resultsTwo = testTwo(7);
  if (resultsTwo < 7)
  {
    std::cout << "Failed Test Two: " << resultsTwo << "/" << 7 << std::endl;
  }
  else
  {
    std::cout << "Passed Test Two!" << std::endl;
  }
  int resultsThree = testThree(9);
  if (resultsThree < 9)
  {
    std::cout << "Failed Test Three: " << resultsThree << "/" << 9 << std::endl;
  }
  else
  {
    std::cout << "Passed Test Three!" << std::endl;
  }
  int resultsFour = testFour(7);
  if (resultsFour < 7)
  {
    std::cout << "Failed Test Four: " << resultsFour << "/" << 7 << std::endl;
  }
  else
  {
    std::cout << "Passed Test Four!" << std::endl;
  }
  int actualScore = resultsOne + resultsTwo + resultsThree + resultsFour;
  int expectedScore = 7 + 7 + 9 + 7 ;
  std::cout << "Overall Score: " << actualScore << "/" << expectedScore << std::endl;
  return 0;
}