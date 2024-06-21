#include <iostream>

#include "chess.h"

int testOne(const int nodesLen)
{
    Chess *currGame = new Chess("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 0");
    long expectedNodes[nodesLen] = {1, 20, 400, 8902, 197281, 4865609, 119060324};
    for (auto x = 0; x < nodesLen; ++x)
    {
        int result = currGame->Perft(x);
        if (result != expectedNodes[x])
        {
            std::cout << "Failed Test One" << std::endl << "Expected:\t"
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
    long expectedNodes[nodesLen] = {1, 48, 2039, 97862, 4085603, 193690690, 8031647685};
    for (auto x = 0; x < nodesLen; ++x)
    {
        int result = currGame->Perft(x);
        if (result != expectedNodes[x])
        {
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
    long expectedNodes[nodesLen] = {1, 14, 191, 2812, 43238, 674624, 11030083, 178633661, 3009794393};
    for (auto x = 0; x < nodesLen; ++x)
    {
        int result = currGame->Perft(x);
        if (result != expectedNodes[x])
        {
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
    long expectedNodes[nodesLen] = {1, 6, 264, 9467, 422333, 15833292, 706045033};
    for (auto x = 0; x < nodesLen; ++x)
    {
        int result = currGame->Perft(x);
        if (result != expectedNodes[x])
        {
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

    std::cout << "Overall Score: " << (resultsOne + resultsTwo + resultsThree + resultsFour) << "/" << (7 + 7 + 9 + 7) << std::endl;
    return 0;
}