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

// int testTwo(const int nodesLen)
// {
//   Chess *currGame = new Chess("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 0");
//   long expectedNodes[nodesLen] = {1, 48, 2039, 97862, 4085603, 193690690, 8031647685};
//   for (auto x = 0; x < nodesLen; ++x)
//   {
//     int result = currGame->Perft(x);
//     if (result != expectedNodes[x])
//     {
//       std::cout << "Failed Test Two" << std::endl
//                 << "Expected:\t" << expectedNodes[x] << std::endl
//                 << "Actual:\t\t" << result << std::endl;
//       return x;
//     }
//   }
//   currGame->ClearPieces();
//   delete currGame;
//   return nodesLen;
// }

// int testThree(const int nodesLen)
// {
//   Chess *currGame = new Chess("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 0");
//   long expectedNodes[nodesLen] = {1, 14, 191, 2812, 43238, 674624, 11030083, 178633661, 3009794393};
//   for (auto x = 0; x < nodesLen; ++x)
//   {
//     int result = currGame->Perft(x);
//     if (result != expectedNodes[x])
//     {
//       std::cout << "Failed Test Three" << std::endl
//                 << "Expected:\t" << expectedNodes[x] << std::endl
//                 << "Actual:\t\t" << result << std::endl;
//       return x;
//     }
//   }
//   currGame->ClearPieces();
//   delete currGame;
//   return nodesLen;
// }

// int testFour(const int nodesLen)
// {
//   Chess *currGame = new Chess("r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1");
//   long expectedNodes[nodesLen] = {1, 6, 264, 9467, 422333, 15833292, 706045033};
//   for (auto x = 0; x < nodesLen; ++x)
//   {
//     int result = currGame->Perft(x);
//     if (result != expectedNodes[x])
//     {
//       std::cout << "Failed Test Four" << std::endl
//                 << "Expected:\t" << expectedNodes[x] << std::endl
//                 << "Actual:\t\t" << result << std::endl;
//       return x;
//     }
//   }
//   currGame->ClearPieces();
//   delete currGame;
//   return nodesLen;
// }

// int testFive(const int nodesLen)
// {
//   Chess *currGame = new Chess("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8  ");
//   long expectedNodes[nodesLen] = {1, 44, 1486, 62379, 2103487, 89941194};
//   for (auto x = 0; x < nodesLen; ++x)
//   {
//     int result = currGame->Perft(x);
//     if (result != expectedNodes[x])
//     {
//       std::cout << "Failed Test Five" << std::endl
//                 << "Expected:\t" << expectedNodes[x] << std::endl
//                 << "Actual:\t\t" << result << std::endl;
//       return x;
//     }
//   }
//   currGame->ClearPieces();
//   delete currGame;
//   return nodesLen;
// }

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
  // int resultsTwo = testTwo(7);
  // if (resultsTwo < 7)
  // {
  //   std::cout << "Failed Test Two: " << resultsTwo << "/" << 7 << std::endl;
  // }
  // else
  // {
  //   std::cout << "Passed Test Two!" << std::endl;
  // }
  // int resultsThree = testThree(9);
  // if (resultsThree < 9)
  // {
  //   std::cout << "Failed Test Three: " << resultsThree << "/" << 9 << std::endl;
  // }
  // else
  // {
  //   std::cout << "Passed Test Three!" << std::endl;
  // }
  // int resultsFour = testFour(7);
  // if (resultsFour < 7)
  // {
  //   std::cout << "Failed Test Four: " << resultsFour << "/" << 7 << std::endl;
  // }
  // else
  // {
  //   std::cout << "Passed Test Four!" << std::endl;
  // }
  // int resultsFive = testFive(6);
  // if (resultsFour < 6)
  // {
  //   std::cout << "Failed Test Five: " << resultsFive << "/" << 6 << std::endl;
  // }
  // else
  // {
  //   std::cout << "Passed Test Five!" << std::endl;
  // }
  // int actualScore = resultsOne + resultsTwo + resultsThree + resultsFour + resultsFive;
  // int expectedScore = 7 + 7 + 9 + 7 + 6;
  // std::cout << "Overall Score: " << actualScore << "/" << expectedScore << std::endl;
  return 0;
}