#include <iostream>
#include <string>
#include <vector>

#include "chess.hpp"

bool FenTest1()
{
  std::string fenString = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 0";
  Chess *game = new Chess(fenString);
  std::string result = game->ConvertToFEN();
  if (result.compare(fenString) != 0)
  {
    std::cout << "Failed FEN Test 1" << std::endl;
    std::cout << "Expected: \t" << fenString << std::endl;
    std::cout << "Actual: \t\t" << result << std::endl;
    return false;
  }
  delete game;
  return true;
}

bool FenTest2()
{
  std::string fenString = "r6r/1b2k1bq/8/8/7B/8/8/R3K2R b KQ - 3 2";
  Chess *game = new Chess(fenString);
  std::string result = game->ConvertToFEN();
  if (result.compare(fenString) != 0)
  {
    std::cout << "Failed FEN Test 1" << std::endl;
    std::cout << "Expected: \t" << fenString << std::endl;
    std::cout << "Actual: \t\t" << result << std::endl;
    return false;
  }
  delete game;
  return true;
}

bool FenTest3()
{
  std::string fenString = "8/8/8/2k5/2pP4/8/B7/4K3 b - d3 0 3";
  Chess *game = new Chess(fenString);
  std::string result = game->ConvertToFEN();
  if (result.compare(fenString) != 0)
  {
    std::cout << "Failed FEN Test 1" << std::endl;
    std::cout << "Expected: \t" << fenString << std::endl;
    std::cout << "Actual: \t\t" << result << std::endl;
    return false;
  }
  delete game;
  return true;
}

bool FenTest4()
{
  std::string fenString = "r1bqkbnr/pppppppp/n7/8/8/P7/1PPPPPPP/RNBQKBNR w KQkq - 2 2";
  Chess *game = new Chess(fenString);
  std::string result = game->ConvertToFEN();
  if (result.compare(fenString) != 0)
  {
    std::cout << "Failed FEN Test 1" << std::endl;
    std::cout << "Expected: \t" << fenString << std::endl;
    std::cout << "Actual: \t\t" << result << std::endl;
    return false;
  }
  delete game;
  return true;
}

bool FenTest5()
{
  std::string fenString = "r3k2r/p1pp1pb1/bn2Qnp1/2qPN3/1p2P3/2N5/PPPBBPPP/R3K2R b KQkq - 3 2";
  Chess *game = new Chess(fenString);
  std::string result = game->ConvertToFEN();
  if (result.compare(fenString) != 0)
  {
    std::cout << "Failed FEN Test 1" << std::endl;
    std::cout << "Expected: \t" << fenString << std::endl;
    std::cout << "Actual: \t\t" << result << std::endl;
    return false;
  }
  delete game;
  return true;
}

void FenConversionTests()
{
  int passedTests = 0;

  std::vector<bool> tests{
      FenTest1(),
      FenTest2(),
      FenTest3(),
      FenTest4(),
      FenTest5(),
  };
  for (auto x : tests)
  {
    if (x) {
      ++passedTests;
    }
  }

  std::cout << "Fen Conversion Results: \t" << passedTests << " / " << tests.size() << std::endl;
}

bool PerftTest1()
{
  std::string fenString = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 0";
  Chess *game = new Chess(fenString);
  long results[6] = {20, 400, 8902, 197281, 4865609, 119060324};
  long actual = 0;
  for (int i = 0; i < sizeof(results) / sizeof(results[0]); ++i)
  {
    actual = game->perft(i + 1);
    if (actual != results[i])
    {
      std::cout << "Failed Perft Test 1" << std::endl;
      std::cout << "Depth: " << i + 1 << std::endl;
      std::cout << "Expected: \t" << results[i] << std::endl;
      std::cout << "Actual: \t" << actual << std::endl;
      return false;
    }
  }
  delete game;
  return true;
}

bool PerftTest2()
{
  std::string fenString = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 0";
  Chess *game = new Chess(fenString);
  long results[6] = {48, 2039, 97862, 4085603, 193690690, 8031647685};
  long actual = 0;
  for (int i = 0; i < sizeof(results) / sizeof(results[0]); ++i)
  {
    actual = game->perft(i + 1);
    if (actual != results[i])
    {
      std::cout << "Failed Perft Test 2" << std::endl;
      std::cout << "Depth: " << i + 1 << std::endl;
      std::cout << "Expected: \t" << results[i] << std::endl;
      std::cout << "Actual: \t" << actual << std::endl;
      return false;
    }
  }
  delete game;
  return true;
}

bool PerftTest3()
{
  std::string fenString = "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 0";
  Chess *game = new Chess(fenString);
  long results[6] = {14, 191, 2812, 43238, 674624, 11030083};
  long actual = 0;
  for (int i = 0; i < sizeof(results) / sizeof(results[0]); ++i)
  {
    actual = game->perft(i + 1);
    if (actual != results[i])
    {
      std::cout << "Failed Perft Test 3" << std::endl;
      std::cout << "Depth: " << i + 1 << std::endl;
      std::cout << "Expected: \t" << results[i] << std::endl;
      std::cout << "Actual: \t" << actual << std::endl;
      return false;
    }
  }
  delete game;
  return true;
}

bool PerftTest4()
{
  std::string fenString = "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1";
  Chess *game = new Chess(fenString);
  long results[6] = {6, 264, 9467, 422333, 15833292, 706045033};
  long actual = 0;
  for (int i = 0; i < sizeof(results) / sizeof(results[0]); ++i)
  {
    actual = game->perft(i + 1);
    if (actual != results[i])
    {
      std::cout << "Failed Perft Test 4" << std::endl;
      std::cout << "Depth: " << i + 1 << std::endl;
      std::cout << "Expected: \t" << results[i] << std::endl;
      std::cout << "Actual: \t" << actual << std::endl;
      return false;
    }
  }
  delete game;
  return true;
}

bool PerftTest5 ()
{
  std::string fenString = "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8";
  Chess *game = new Chess(fenString);
  long results[5] = {44, 1486, 62379, 2103487, 89941194};
  long actual = 0;
  for (int i = 0; i < sizeof(results) / sizeof(results[0]); ++i)
  {
    actual = game->perft(i + 1);
    if (actual != results[i])
    {
      std::cout << "Failed Perft Test 5" << std::endl;
      std::cout << "Depth: " << i + 1 << std::endl;
      std::cout << "Expected: \t" << results[i] << std::endl;
      std::cout << "Actual: \t" << actual << std::endl;
      return false;
    }
  }
  delete game;
  return true;
}

void PerftTests()
{
  int passedTests = 0;

  std::vector<bool> tests{
      PerftTest1(),
      PerftTest2(),
      PerftTest3(),
      PerftTest4(),
      PerftTest5(),
      // PerftTest6(),
      // PerftTest7(),
      // PerftTest8(),
      // PerftTest9(),
      // PerftTest10()
  };
  for (auto x : tests)
  {
    if (x)
      ++passedTests;
  }
  std::cout << "Perft Results: \t\t" << passedTests << " / " << tests.size() << std::endl;
}

int main()
{
  Chess::Initialize();
  FenConversionTests();
  PerftTests();
  return 0;
}