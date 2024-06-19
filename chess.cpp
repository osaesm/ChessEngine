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

// This function does the following:
// * Creates a copy of the game
// * Moves the piece from "start" to "end" in the copy
// * Increments the lastPawnOrTake & fullTurns
// * Sets en passant idx to -1 (if needed, set later)
// * Can't deal with occurrences bc of castling & en passant
Chess* Chess::MovePiece(const short start, const short end, const bool updateOccurrences) {
    Chess* nextMoveGame = new Chess(*this);
    nextMoveGame->pieces[start] = nullptr;
    nextMoveGame->pieces[end] = this->pieces[start];
    if (this->pieces[start]->type == Piece::Type::PAWN || this->pieces[end]) {
        nextMoveGame->lastPawnOrTake = 0;
    } else {
        ++nextMoveGame->lastPawnOrTake;
    }
    nextMoveGame->turn = (this->turn == Piece::Color::WHITE) ? Piece::Color::BLACK ? Piece::Color::WHITE;
    if (this->turn == Piece::Color::BLACK) {
        ++this->fullTurns;
    }
    if (nextMoveGame->enPassantIdx != -1) {
        nextMoveGame->enPassantIdx = -1;
    }
    if (updateOccurrences) {
        nextMoveGame->occurrences[nextMoveGame->BoardIdx()]++;
    }
    return nextMoveGame;
}

Chess* Chess::UpgradePawn(const short start, const short end, const Piece::Type option) {
    Chess* nextMoveGame = new Chess(*this);
    nextMoveGame->pieces[start] = nullptr;
    nextMoveGame->pieces[end] = new Piece();
    nextMoveGame->pieces[end]->color = this->pieces[start]->color;
    nextMoveGame->pieces[end]->type = option;
    nextMoveGame->lastPawnOrTake = 0;
    if (nextMoveGame->enPassantIdx != -1) {
        nextMoveGame->enPassantIdx = -1;
    }
    nextMoveGame->turn = (this->turn == Piece::Color::WHITE) ? Piece::Color::BLACK ? Piece::Color::WHITE;
    if (this->turn == Piece::Color::BLACK) {
        ++this->fullTurns;
    }
    nextMoveGame->occurrences[nextMoveGame->BoardIdx()]++;
    return nextMoveGame;
}

std::vector<Chess *> Chess::LegalMoves()
{
    // PseudoLegalMoves
    // See if move is actually legal
    std::vector<Chess *> pseudoLegalMoves;
    Piece::Type promotions[4] = {Piece::Type::QUEEN, Piece::Type::ROOK, Piece::Type::BISHOP, Piece::Type::KNIGHT};
    // iterating over rows and columns is a lot easier than 0-63 directly (avoids modular math)
    short diagonals[4] = {-9, -7, 7, 9};
    short straights[4] = {-8, -1, 1, 8};
    std::string fenString = "";
    for (auto row = 0; row < 8; ++row)
    {
        for (auto col = 0; col < 8; ++col)
        {
            auto idx = (8 * row) + col;
            Piece *currPiece = this->pieces[idx];
            if (currPiece->color == this->turn)
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
                                Chess* nextMoveGame = this->MovePiece(idx, idx + 7, false);
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
                                Chess *nextMoveGame = this->MovePiece(idx, idx + 9);
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
                                        pseudoLegalMoves.push_back(this->UpgradePawn(idx, idx - 7, option))
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
                    if (row > 1) {
                        if (col > 0 && (!this->pieces[idx - 17] || (this->pieces[idx - 17]->color != this->turn))) {
                            pseudoLegalMoves.push_back(this->MovePiece(idx, idx - 17, true));
                        }
                        if (col < 7 && (!this->pieces[idx - 15] || (this->pieces[idx - 15]->color != this->turn))) {
                            pseudoLegalMoves.push_back(this->MovePiece(idx, idx - 15, true));
                        }
                    }
                    if (row > 0) {
                        if (col > 1 && (!this->pieces[idx - 10] || (this->pieces[idx - 10]->color != this->turn))) {
                            pseudoLegalMoves.push_back(this->MovePiece(idx, idx - 10, true));
                        }
                        if (col < 6 && (!this->pieces[idx - 6] || (this->pieces[idx - 6]->color != this->turn))) {
                            pseudoLegalMoves.push_back(this->MovePiece(idx, idx - 6, true));
                        }
                    }
                    if (row < 7) {
                        if (col > 1 && (!this->pieces[idx + 6] || (this->pieces[idx + 6]->color != this->turn))) {
                            pseudoLegalMoves.push_back(this->MovePiece(idx, idx + 6, true));
                        }
                        if (col < 6 && (!this->pieces[idx + 10] || (this->pieces[idx + 10]->color != this->turn))) {
                            pseudoLegalMoves.push_back(this->MovePiece(idx, idx + 10, true));
                        }
                    }
                    if (row < 6) {
                        if (col > 0 && (!this->pieces[idx + 15] || (this->pieces[idx + 15]->color != this->turn))) {
                            pseudoLegalMoves.push_back(this->MovePiece(idx, idx + 15, true));
                        }
                        if (col < 7 && (!this->pieces[idx + 17] || (this->pieces[idx + 17]->color != this->turn))) {
                            pseudoLegalMoves.push_back(this->MovePiece(idx, idx + 17, true));
                        }
                    }
                    break;
                case Piece::Type::BISHOP:
                    // Sliding with increments of -9, -7, 7, 9
                    for (auto direction : diagonals)
                    {
                        auto rowIteration = (direction > 0) ? 1 : -1;
                        auto colIteration = (direction % 8 == 1) ? 1 : -1;
                        for (auto newRow = row + rowIteration; newRow >= 0 && newRow < 8; newRow += rowIteration)
                        {
                            for (auto newCol = col + colIteration; newCol >= 0 && newCol < 8; newCol += colIteration)
                            {
                                auto nextIdx = (newRow * 8) + newCol;
                                if (!this->pieces[nextIdx] || this->pieces[nextIdx]->color != this->turn)
                                {
                                    pseudoLegalMoves.push_back(this->MovePiece(idx, nextIdx, true));
                                }
                                if (this->pieces[nextIdx]) {
                                    break;
                                }
                            }
                        }
                    }
                    break;
                case Piece::Type::ROOK:
                    // Sliding with increments of -8, -1, 1, 8
                    // need to mark castle variables
                    for (auto downRow = row - 1; downRow >= 0; --downRow)
                    {
                        auto nextIdx = (downRow * 8) + col;
                        if (!this->pieces[nextIdx] || this->pieces[nextIdx]->color != this->turn)
                        {
                            nextMoveGame->pieces[nextIdx] = currPiece;
                            fenString = nextMoveGame->BoardIdx();
                            if (nextMoveGame->occurrences.find(fenString) == nextMoveGame->occurrences.end())
                            {
                                nextMoveGame->occurrences[fenString] = 1;
                            }
                            else
                            {
                                nextMoveGame->occurrences[fenString]++;
                            }
                            if (this->pieces[nextIdx])
                            {
                                nextMoveGame->lastPawnOrTake = 0;
                            }
                            pseudoLegalMoves.push_back(nextMoveGame);
                            nextMoveGame = new Chess(*nextMoveGame);
                            nextMoveGame->pieces[nextIdx] = nullptr;
                            if (nextMoveGame->occurrences[fenString] == 1)
                            {
                                nextMoveGame->occurrences.erase(fenString);
                            }
                            else
                            {
                                nextMoveGame->occurrences[fenString]--;
                            }
                        }
                    }
                    for (auto upRow = row + 1; upRow < 8; ++upRow)
                    {
                        auto nextIdx = (upRow * 8) + col;
                        if (!this->pieces[nextIdx])
                        {
                            nextMoveGame->pieces[nextIdx] = currPiece;
                            fenString = nextMoveGame->BoardIdx();
                            if (nextMoveGame->occurrences.find(fenString) == nextMoveGame->occurrences.end())
                            {
                                nextMoveGame->occurrences[fenString] = 1;
                            }
                            else
                            {
                                nextMoveGame->occurrences[fenString]++;
                            }
                            pseudoLegalMoves.push_back(nextMoveGame);
                            nextMoveGame = new Chess(*nextMoveGame);
                            nextMoveGame->pieces[nextIdx] = nullptr;
                            if (nextMoveGame->occurrences[fenString] == 1)
                            {
                                nextMoveGame->occurrences.erase(fenString);
                            }
                            else
                            {
                                nextMoveGame->occurrences[fenString]--;
                            }
                        }
                        else if (this->pieces[nextIdx]->color != this->turn)
                        {
                            nextMoveGame->pieces[nextIdx] = currPiece;
                            nextMoveGame->lastPawnOrTake = 0;
                            fenString = nextMoveGame->BoardIdx();
                            if (nextMoveGame->occurrences.find(fenString) == nextMoveGame->occurrences.end())
                            {
                                nextMoveGame->occurrences[fenString] = 1;
                            }
                            else
                            {
                                nextMoveGame->occurrences[fenString]++;
                            }
                            pseudoLegalMoves.push_back(nextMoveGame);
                            nextMoveGame = new Chess(*nextMoveGame);
                            nextMoveGame->pieces[nextIdx] = this->pieces[nextIdx];
                            nextMoveGame->lastPawnOrTake = this->lastPawnOrTake + 1;
                            if (nextMoveGame->occurrences[fenString] == 1)
                            {
                                nextMoveGame->occurrences.erase(fenString);
                            }
                            else
                            {
                                nextMoveGame->occurrences[fenString]--;
                            }
                        }
                    }
                    for (auto leftCol = col - 1; leftCol >= 0; --leftCol)
                    {
                        auto nextIdx = (row * 8) + leftCol;
                        if (!this->pieces[nextIdx])
                        {
                            nextMoveGame->pieces[nextIdx] = currPiece;
                            fenString = nextMoveGame->BoardIdx();
                            if (nextMoveGame->occurrences.find(fenString) == nextMoveGame->occurrences.end())
                            {
                                nextMoveGame->occurrences[fenString] = 1;
                            }
                            else
                            {
                                nextMoveGame->occurrences[fenString]++;
                            }
                            pseudoLegalMoves.push_back(nextMoveGame);
                            nextMoveGame = new Chess(*nextMoveGame);
                            nextMoveGame->pieces[nextIdx] = nullptr;
                            if (nextMoveGame->occurrences[fenString] == 1)
                            {
                                nextMoveGame->occurrences.erase(fenString);
                            }
                            else
                            {
                                nextMoveGame->occurrences[fenString]--;
                            }
                        }
                        else if (this->pieces[nextIdx]->color != this->turn)
                        {
                            nextMoveGame->pieces[nextIdx] = currPiece;
                            nextMoveGame->lastPawnOrTake = 0;
                            fenString = nextMoveGame->BoardIdx();
                            if (nextMoveGame->occurrences.find(fenString) == nextMoveGame->occurrences.end())
                            {
                                nextMoveGame->occurrences[fenString] = 1;
                            }
                            else
                            {
                                nextMoveGame->occurrences[fenString]++;
                            }
                            pseudoLegalMoves.push_back(nextMoveGame);
                            nextMoveGame = new Chess(*nextMoveGame);
                            nextMoveGame->pieces[nextIdx] = this->pieces[nextIdx];
                            nextMoveGame->lastPawnOrTake = this->lastPawnOrTake + 1;
                            if (nextMoveGame->occurrences[fenString] == 1)
                            {
                                nextMoveGame->occurrences.erase(fenString);
                            }
                            else
                            {
                                nextMoveGame->occurrences[fenString]--;
                            }
                        }
                    }
                    for (auto rightCol = col + 1; rightCol < 8; ++rightCol)
                    {
                        auto nextIdx = (row * 8) + rightCol;
                        if (!this->pieces[nextIdx])
                        {
                            nextMoveGame->pieces[nextIdx] = currPiece;
                            fenString = nextMoveGame->BoardIdx();
                            if (nextMoveGame->occurrences.find(fenString) == nextMoveGame->occurrences.end())
                            {
                                nextMoveGame->occurrences[fenString] = 1;
                            }
                            else
                            {
                                nextMoveGame->occurrences[fenString]++;
                            }
                            pseudoLegalMoves.push_back(nextMoveGame);
                            nextMoveGame = new Chess(*nextMoveGame);
                            nextMoveGame->pieces[nextIdx] = nullptr;
                            if (nextMoveGame->occurrences[fenString] == 1)
                            {
                                nextMoveGame->occurrences.erase(fenString);
                            }
                            else
                            {
                                nextMoveGame->occurrences[fenString]--;
                            }
                        }
                        else if (this->pieces[nextIdx]->color != this->turn)
                        {
                            nextMoveGame->pieces[nextIdx] = currPiece;
                            nextMoveGame->lastPawnOrTake = 0;
                            fenString = nextMoveGame->BoardIdx();
                            if (nextMoveGame->occurrences.find(fenString) == nextMoveGame->occurrences.end())
                            {
                                nextMoveGame->occurrences[fenString] = 1;
                            }
                            else
                            {
                                nextMoveGame->occurrences[fenString]++;
                            }
                            pseudoLegalMoves.push_back(nextMoveGame);
                            nextMoveGame = new Chess(*nextMoveGame);
                            nextMoveGame->pieces[nextIdx] = this->pieces[nextIdx];
                            nextMoveGame->lastPawnOrTake = this->lastPawnOrTake + 1;
                            if (nextMoveGame->occurrences[fenString] == 1)
                            {
                                nextMoveGame->occurrences.erase(fenString);
                            }
                            else
                            {
                                nextMoveGame->occurrences[fenString]--;
                            }
                        }
                    }
                    break;
                case Piece::Type::QUEEN:
                    for (auto direction : diagonals)
                    {
                        auto rowIteration = (direction > 0) ? 1 : -1;
                        auto colIteration = (direction % 8 == 1) ? 1 : -1;
                        for (auto newRow = row + rowIteration; newRow >= 0 && newRow < 8; ++newRow)
                        {
                            for (auto newCol = col + colIteration; newCol >= 0 && newCol < 8; ++newCol)
                            {
                                auto nextIdx = (newRow * 8) + newCol;
                                if (!this->pieces[nextIdx] || this->pieces[nextIdx]->color != this->turn)
                                {
                                    nextMoveGame->pieces[nextIdx] = currPiece;
                                    fenString = nextMoveGame->BoardIdx();
                                    if (nextMoveGame->occurrences.find(fenString) == nextMoveGame->occurrences.end())
                                    {
                                        nextMoveGame->occurrences[fenString] = 1;
                                    }
                                    else
                                    {
                                        nextMoveGame->occurrences[fenString]++;
                                    }
                                    if (this->pieces[nextIdx])
                                    {
                                        nextMoveGame->lastPawnOrTake = 0;
                                    }
                                    pseudoLegalMoves.push_back(nextMoveGame);
                                    nextMoveGame = new Chess(*nextMoveGame);
                                    nextMoveGame->pieces[nextIdx] = nullptr;
                                    if (nextMoveGame->occurrences[fenString] == 1)
                                    {
                                        nextMoveGame->occurrences.erase(fenString);
                                    }
                                    else
                                    {
                                        nextMoveGame->occurrences[fenString]--;
                                    }
                                }
                            }
                        }
                    }
                    for (auto downRow = row - 1; downRow >= 0; --downRow)
                    {
                        auto nextIdx = (downRow * 8) + col;
                        if (!this->pieces[nextIdx])
                        {
                            nextMoveGame->pieces[nextIdx] = currPiece;
                            fenString = nextMoveGame->BoardIdx();
                            if (nextMoveGame->occurrences.find(fenString) == nextMoveGame->occurrences.end())
                            {
                                nextMoveGame->occurrences[fenString] = 1;
                            }
                            else
                            {
                                nextMoveGame->occurrences[fenString]++;
                            }
                            pseudoLegalMoves.push_back(nextMoveGame);
                            nextMoveGame = new Chess(*nextMoveGame);
                            nextMoveGame->pieces[nextIdx] = nullptr;
                            if (nextMoveGame->occurrences[fenString] == 1)
                            {
                                nextMoveGame->occurrences.erase(fenString);
                            }
                            else
                            {
                                nextMoveGame->occurrences[fenString]--;
                            }
                        }
                        else if (this->pieces[nextIdx]->color != this->turn)
                        {
                            nextMoveGame->pieces[nextIdx] = currPiece;
                            nextMoveGame->lastPawnOrTake = 0;
                            fenString = nextMoveGame->BoardIdx();
                            if (nextMoveGame->occurrences.find(fenString) == nextMoveGame->occurrences.end())
                            {
                                nextMoveGame->occurrences[fenString] = 1;
                            }
                            else
                            {
                                nextMoveGame->occurrences[fenString]++;
                            }
                            pseudoLegalMoves.push_back(nextMoveGame);
                            nextMoveGame = new Chess(*nextMoveGame);
                            nextMoveGame->pieces[nextIdx] = this->pieces[nextIdx];
                            nextMoveGame->lastPawnOrTake = this->lastPawnOrTake + 1;
                            if (nextMoveGame->occurrences[fenString] == 1)
                            {
                                nextMoveGame->occurrences.erase(fenString);
                            }
                            else
                            {
                                nextMoveGame->occurrences[fenString]--;
                            }
                        }
                    }
                    for (auto upRow = row + 1; upRow < 8; ++upRow)
                    {
                        auto nextIdx = (upRow * 8) + col;
                        if (!this->pieces[nextIdx])
                        {
                            nextMoveGame->pieces[nextIdx] = currPiece;
                            fenString = nextMoveGame->BoardIdx();
                            if (nextMoveGame->occurrences.find(fenString) == nextMoveGame->occurrences.end())
                            {
                                nextMoveGame->occurrences[fenString] = 1;
                            }
                            else
                            {
                                nextMoveGame->occurrences[fenString]++;
                            }
                            pseudoLegalMoves.push_back(nextMoveGame);
                            nextMoveGame = new Chess(*nextMoveGame);
                            nextMoveGame->pieces[nextIdx] = nullptr;
                            if (nextMoveGame->occurrences[fenString] == 1)
                            {
                                nextMoveGame->occurrences.erase(fenString);
                            }
                            else
                            {
                                nextMoveGame->occurrences[fenString]--;
                            }
                        }
                        else if (this->pieces[nextIdx]->color != this->turn)
                        {
                            nextMoveGame->pieces[nextIdx] = currPiece;
                            nextMoveGame->lastPawnOrTake = 0;
                            fenString = nextMoveGame->BoardIdx();
                            if (nextMoveGame->occurrences.find(fenString) == nextMoveGame->occurrences.end())
                            {
                                nextMoveGame->occurrences[fenString] = 1;
                            }
                            else
                            {
                                nextMoveGame->occurrences[fenString]++;
                            }
                            pseudoLegalMoves.push_back(nextMoveGame);
                            nextMoveGame = new Chess(*nextMoveGame);
                            nextMoveGame->pieces[nextIdx] = this->pieces[nextIdx];
                            nextMoveGame->lastPawnOrTake = this->lastPawnOrTake + 1;
                            if (nextMoveGame->occurrences[fenString] == 1)
                            {
                                nextMoveGame->occurrences.erase(fenString);
                            }
                            else
                            {
                                nextMoveGame->occurrences[fenString]--;
                            }
                        }
                    }
                    for (auto leftCol = col - 1; leftCol >= 0; --leftCol)
                    {
                        auto nextIdx = (row * 8) + leftCol;
                        if (!this->pieces[nextIdx])
                        {
                            nextMoveGame->pieces[nextIdx] = currPiece;
                            fenString = nextMoveGame->BoardIdx();
                            if (nextMoveGame->occurrences.find(fenString) == nextMoveGame->occurrences.end())
                            {
                                nextMoveGame->occurrences[fenString] = 1;
                            }
                            else
                            {
                                nextMoveGame->occurrences[fenString]++;
                            }
                            pseudoLegalMoves.push_back(nextMoveGame);
                            nextMoveGame = new Chess(*nextMoveGame);
                            nextMoveGame->pieces[nextIdx] = nullptr;
                            if (nextMoveGame->occurrences[fenString] == 1)
                            {
                                nextMoveGame->occurrences.erase(fenString);
                            }
                            else
                            {
                                nextMoveGame->occurrences[fenString]--;
                            }
                        }
                        else if (this->pieces[nextIdx]->color != this->turn)
                        {
                            nextMoveGame->pieces[nextIdx] = currPiece;
                            nextMoveGame->lastPawnOrTake = 0;
                            fenString = nextMoveGame->BoardIdx();
                            if (nextMoveGame->occurrences.find(fenString) == nextMoveGame->occurrences.end())
                            {
                                nextMoveGame->occurrences[fenString] = 1;
                            }
                            else
                            {
                                nextMoveGame->occurrences[fenString]++;
                            }
                            pseudoLegalMoves.push_back(nextMoveGame);
                            nextMoveGame = new Chess(*nextMoveGame);
                            nextMoveGame->pieces[nextIdx] = this->pieces[nextIdx];
                            nextMoveGame->lastPawnOrTake = this->lastPawnOrTake + 1;
                            if (nextMoveGame->occurrences[fenString] == 1)
                            {
                                nextMoveGame->occurrences.erase(fenString);
                            }
                            else
                            {
                                nextMoveGame->occurrences[fenString]--;
                            }
                        }
                    }
                    for (auto rightCol = col + 1; rightCol < 8; ++rightCol)
                    {
                        auto nextIdx = (row * 8) + rightCol;
                        if (!this->pieces[nextIdx])
                        {
                            nextMoveGame->pieces[nextIdx] = currPiece;
                            fenString = nextMoveGame->BoardIdx();
                            if (nextMoveGame->occurrences.find(fenString) == nextMoveGame->occurrences.end())
                            {
                                nextMoveGame->occurrences[fenString] = 1;
                            }
                            else
                            {
                                nextMoveGame->occurrences[fenString]++;
                            }
                            pseudoLegalMoves.push_back(nextMoveGame);
                            nextMoveGame = new Chess(*nextMoveGame);
                            nextMoveGame->pieces[nextIdx] = nullptr;
                            if (nextMoveGame->occurrences[fenString] == 1)
                            {
                                nextMoveGame->occurrences.erase(fenString);
                            }
                            else
                            {
                                nextMoveGame->occurrences[fenString]--;
                            }
                        }
                        else if (this->pieces[nextIdx]->color != this->turn)
                        {
                            nextMoveGame->pieces[nextIdx] = currPiece;
                            nextMoveGame->lastPawnOrTake = 0;
                            fenString = nextMoveGame->BoardIdx();
                            if (nextMoveGame->occurrences.find(fenString) == nextMoveGame->occurrences.end())
                            {
                                nextMoveGame->occurrences[fenString] = 1;
                            }
                            else
                            {
                                nextMoveGame->occurrences[fenString]++;
                            }
                            pseudoLegalMoves.push_back(nextMoveGame);
                            nextMoveGame = new Chess(*nextMoveGame);
                            nextMoveGame->pieces[nextIdx] = this->pieces[nextIdx];
                            nextMoveGame->lastPawnOrTake = this->lastPawnOrTake + 1;
                            if (nextMoveGame->occurrences[fenString] == 1)
                            {
                                nextMoveGame->occurrences.erase(fenString);
                            }
                            else
                            {
                                nextMoveGame->occurrences[fenString]--;
                            }
                        }
                    }
                    // Sliding with bishop + rook
                    break;
                default:
                    // King goes one any direction
                    if (currPiece->color == Piece::Color::WHITE)
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
                    auto nextIdx = -1;
                    if (row != 0 && col != 0)
                    {
                        nextIdx = ((row - 1) * 8) + (col - 1);
                        if (!this->pieces[nextIdx] || this->pieces[nextIdx]->color != this->turn)
                        {
                            nextMoveGame->pieces[nextIdx] = currPiece;
                            fenString = nextMoveGame->BoardIdx();
                            if (nextMoveGame->occurrences.find(fenString) == nextMoveGame->occurrences.end())
                            {
                                nextMoveGame->occurrences[fenString] = 1;
                            }
                            else
                            {
                                nextMoveGame->occurrences[fenString]++;
                            }
                            if (this->pieces[nextIdx])
                            {
                                nextMoveGame->lastPawnOrTake = 0;
                            }
                            pseudoLegalMoves.push_back(nextMoveGame);
                            nextMoveGame = new Chess(*nextMoveGame);
                            nextMoveGame->pieces[nextIdx] = nullptr;
                            if (nextMoveGame->lastPawnOrTake != (this->lastPawnOrTake + 1))
                            {
                                nextMoveGame->lastPawnOrTake = this->lastPawnOrTake + 1;
                            }
                            if (nextMoveGame->occurrences[fenString] == 1)
                            {
                                nextMoveGame->occurrences.erase(fenString);
                            }
                            else
                            {
                                nextMoveGame->occurrences[fenString]--;
                            }
                        }
                    }
                    else if (row != 0 && col != 7)
                    {
                        nextIdx = ((row - 1) * 8) + (col + 1);
                        if (!this->pieces[nextIdx] || this->pieces[nextIdx]->color != this->turn)
                        {
                            nextMoveGame->pieces[nextIdx] = currPiece;
                            fenString = nextMoveGame->BoardIdx();
                            if (nextMoveGame->occurrences.find(fenString) == nextMoveGame->occurrences.end())
                            {
                                nextMoveGame->occurrences[fenString] = 1;
                            }
                            else
                            {
                                nextMoveGame->occurrences[fenString]++;
                            }
                            if (this->pieces[nextIdx])
                            {
                                nextMoveGame->lastPawnOrTake = 0;
                            }
                            pseudoLegalMoves.push_back(nextMoveGame);
                            nextMoveGame = new Chess(*nextMoveGame);
                            nextMoveGame->pieces[nextIdx] = nullptr;
                            if (nextMoveGame->lastPawnOrTake != (this->lastPawnOrTake + 1))
                            {
                                nextMoveGame->lastPawnOrTake = this->lastPawnOrTake + 1;
                            }
                            if (nextMoveGame->occurrences[fenString] == 1)
                            {
                                nextMoveGame->occurrences.erase(fenString);
                            }
                            else
                            {
                                nextMoveGame->occurrences[fenString]--;
                            }
                        }
                    }
                    else if (row != 0)
                    {
                        nextIdx = ((row - 1) * 8) + col;
                        if (!this->pieces[nextIdx] || this->pieces[nextIdx]->color != this->turn)
                        {
                            nextMoveGame->pieces[nextIdx] = currPiece;
                            fenString = nextMoveGame->BoardIdx();
                            if (nextMoveGame->occurrences.find(fenString) == nextMoveGame->occurrences.end())
                            {
                                nextMoveGame->occurrences[fenString] = 1;
                            }
                            else
                            {
                                nextMoveGame->occurrences[fenString]++;
                            }
                            if (this->pieces[nextIdx])
                            {
                                nextMoveGame->lastPawnOrTake = 0;
                            }
                            pseudoLegalMoves.push_back(nextMoveGame);
                            nextMoveGame = new Chess(*nextMoveGame);
                            nextMoveGame->pieces[nextIdx] = nullptr;
                            if (nextMoveGame->lastPawnOrTake != (this->lastPawnOrTake + 1))
                            {
                                nextMoveGame->lastPawnOrTake = this->lastPawnOrTake + 1;
                            }
                            if (nextMoveGame->occurrences[fenString] == 1)
                            {
                                nextMoveGame->occurrences.erase(fenString);
                            }
                            else
                            {
                                nextMoveGame->occurrences[fenString]--;
                            }
                        }
                    }

                    if (row != 7 && col != 0)
                    {
                        nextIdx = ((row + 1) * 8) + (col - 1);
                        if (!this->pieces[nextIdx] || this->pieces[nextIdx]->color != this->turn)
                        {
                            nextMoveGame->pieces[nextIdx] = currPiece;
                            fenString = nextMoveGame->BoardIdx();
                            if (nextMoveGame->occurrences.find(fenString) == nextMoveGame->occurrences.end())
                            {
                                nextMoveGame->occurrences[fenString] = 1;
                            }
                            else
                            {
                                nextMoveGame->occurrences[fenString]++;
                            }
                            if (this->pieces[nextIdx])
                            {
                                nextMoveGame->lastPawnOrTake = 0;
                            }
                            pseudoLegalMoves.push_back(nextMoveGame);
                            nextMoveGame = new Chess(*nextMoveGame);
                            nextMoveGame->pieces[nextIdx] = nullptr;
                            if (nextMoveGame->lastPawnOrTake != (this->lastPawnOrTake + 1))
                            {
                                nextMoveGame->lastPawnOrTake = this->lastPawnOrTake + 1;
                            }
                            if (nextMoveGame->occurrences[fenString] == 1)
                            {
                                nextMoveGame->occurrences.erase(fenString);
                            }
                            else
                            {
                                nextMoveGame->occurrences[fenString]--;
                            }
                        }
                    }
                    else if (row != 7 && col != 7)
                    {
                        nextIdx = ((row + 1) * 8) + (col + 1);
                        if (!this->pieces[nextIdx] || this->pieces[nextIdx]->color != this->turn)
                        {
                            nextMoveGame->pieces[nextIdx] = currPiece;
                            fenString = nextMoveGame->BoardIdx();
                            if (nextMoveGame->occurrences.find(fenString) == nextMoveGame->occurrences.end())
                            {
                                nextMoveGame->occurrences[fenString] = 1;
                            }
                            else
                            {
                                nextMoveGame->occurrences[fenString]++;
                            }
                            if (this->pieces[nextIdx])
                            {
                                nextMoveGame->lastPawnOrTake = 0;
                            }
                            pseudoLegalMoves.push_back(nextMoveGame);
                            nextMoveGame = new Chess(*nextMoveGame);
                            nextMoveGame->pieces[nextIdx] = nullptr;
                            if (nextMoveGame->lastPawnOrTake != (this->lastPawnOrTake + 1))
                            {
                                nextMoveGame->lastPawnOrTake = this->lastPawnOrTake + 1;
                            }
                            if (nextMoveGame->occurrences[fenString] == 1)
                            {
                                nextMoveGame->occurrences.erase(fenString);
                            }
                            else
                            {
                                nextMoveGame->occurrences[fenString]--;
                            }
                        }
                    }
                    else if (row != 7)
                    {
                        nextIdx = ((row + 1) * 8) + col;
                        if (!this->pieces[nextIdx] || this->pieces[nextIdx]->color != this->turn)
                        {
                            nextMoveGame->pieces[nextIdx] = currPiece;
                            fenString = nextMoveGame->BoardIdx();
                            if (nextMoveGame->occurrences.find(fenString) == nextMoveGame->occurrences.end())
                            {
                                nextMoveGame->occurrences[fenString] = 1;
                            }
                            else
                            {
                                nextMoveGame->occurrences[fenString]++;
                            }
                            if (this->pieces[nextIdx])
                            {
                                nextMoveGame->lastPawnOrTake = 0;
                            }
                            pseudoLegalMoves.push_back(nextMoveGame);
                            nextMoveGame = new Chess(*nextMoveGame);
                            nextMoveGame->pieces[nextIdx] = nullptr;
                            if (nextMoveGame->lastPawnOrTake != (this->lastPawnOrTake + 1))
                            {
                                nextMoveGame->lastPawnOrTake = this->lastPawnOrTake + 1;
                            }
                            if (nextMoveGame->occurrences[fenString] == 1)
                            {
                                nextMoveGame->occurrences.erase(fenString);
                            }
                            else
                            {
                                nextMoveGame->occurrences[fenString]--;
                            }
                        }
                    }

                    if (col != 0)
                    {
                        nextIdx = idx - 1;
                        if (!this->pieces[nextIdx] || this->pieces[nextIdx]->color != this->turn)
                        {
                            nextMoveGame->pieces[nextIdx] = currPiece;
                            fenString = nextMoveGame->BoardIdx();
                            if (nextMoveGame->occurrences.find(fenString) == nextMoveGame->occurrences.end())
                            {
                                nextMoveGame->occurrences[fenString] = 1;
                            }
                            else
                            {
                                nextMoveGame->occurrences[fenString]++;
                            }
                            if (this->pieces[nextIdx])
                            {
                                nextMoveGame->lastPawnOrTake = 0;
                            }
                            pseudoLegalMoves.push_back(nextMoveGame);
                            nextMoveGame = new Chess(*nextMoveGame);
                            nextMoveGame->pieces[nextIdx] = nullptr;
                            if (nextMoveGame->lastPawnOrTake != (this->lastPawnOrTake + 1))
                            {
                                nextMoveGame->lastPawnOrTake = this->lastPawnOrTake + 1;
                            }
                            if (nextMoveGame->occurrences[fenString] == 1)
                            {
                                nextMoveGame->occurrences.erase(fenString);
                            }
                            else
                            {
                                nextMoveGame->occurrences[fenString]--;
                            }
                        }
                    }
                    if (col != 7)
                    {
                        nextIdx = idx + 1;
                        if (!this->pieces[nextIdx] || this->pieces[nextIdx]->color != this->turn)
                        {
                            nextMoveGame->pieces[nextIdx] = currPiece;
                            fenString = nextMoveGame->BoardIdx();
                            if (nextMoveGame->occurrences.find(fenString) == nextMoveGame->occurrences.end())
                            {
                                nextMoveGame->occurrences[fenString] = 1;
                            }
                            else
                            {
                                nextMoveGame->occurrences[fenString]++;
                            }
                            if (this->pieces[nextIdx])
                            {
                                nextMoveGame->lastPawnOrTake = 0;
                            }
                            pseudoLegalMoves.push_back(nextMoveGame);
                            nextMoveGame = new Chess(*nextMoveGame);
                            nextMoveGame->pieces[nextIdx] = nullptr;
                            if (nextMoveGame->lastPawnOrTake != (this->lastPawnOrTake + 1))
                            {
                                nextMoveGame->lastPawnOrTake = this->lastPawnOrTake + 1;
                            }
                            if (nextMoveGame->occurrences[fenString] == 1)
                            {
                                nextMoveGame->occurrences.erase(fenString);
                            }
                            else
                            {
                                nextMoveGame->occurrences[fenString]--;
                            }
                        }
                    }
                    break;
                }
            }
        }
    }
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
        if (blankSpaces == 8)
        {
            fenString += "8";
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
        fenString += "- ";
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