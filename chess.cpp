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

Chess::Chess(Piece *newBoard[64], Piece::Color currTurn, bool wCastle, bool wQueenCastle, bool bCastle, bool bQueenCastle, short enPassantIdx, short lastPawnOrTake, int fullTurns, std::unordered_map<std::string, short> newOccurrences)
{
    for (auto i = 0; i < 64; ++i)
    {
        this->pieces[i] = newBoard[i];
    }
    this->turn = currTurn;
    this->wCastle = wCastle;
    this->wQueenCastle = wQueenCastle;
    this->bCastle = bCastle;
    this->bQueenCastle = bQueenCastle;
    this->enPassantIdx = enPassantIdx;
    this->lastPawnOrTake = lastPawnOrTake;
    this->fullTurns = fullTurns;
    for (auto const &elem : newOccurrences)
    {
        this->occurrences[elem.first] = elem.second;
    }
}

std::vector<Chess *> Chess::LegalMoves()
{
    // PseudoLegalMoves
    // See if move is actually legal
    std::vector<Chess *> pseudoLegalMoves;
    Piece::Type promotions[4] = {Piece::Type::QUEEN, Piece::Type::ROOK, Piece::Type::BISHOP, Piece::Type::KNIGHT};
    for (auto idx = 0; idx < 64; ++idx)
    {
        Piece *currPiece = this->pieces[idx];
        if (currPiece->color == this->turn)
        {
            Chess *nextMoveGame = new Chess(*this);
            nextMoveGame->turn = (this->turn == Piece::Color::WHITE) ? Piece::Color::BLACK : Piece::Color::WHITE;
            ++nextMoveGame->lastPawnOrTake;
            if (!nextMoveGame->turn)
            {
                ++this->fullTurns;
            }
            if (nextMoveGame->enPassantIdx != -1)
            {
                nextMoveGame->enPassantIdx = -1;
            }
            nextMoveGame->pieces[idx] = nullptr;
            switch (currPiece->type)
            {
            case Piece::Type::PAWN:
                nextMoveGame->lastPawnOrTake = 0;
                // White Pawns
                // - White Forward
                //   - If pawn promotes, try all
                //   - Else 2 forward
                // - White Takes Left
                //   - If pawn promotes, try all
                //   - Else just move pawn
                // - White Takes Right
                //   - If pawn promotes, try all
                //   - Else just move pawn
                if (currPiece->color == Piece::Color::WHITE)
                {
                    if (!this->pieces[idx + 8])
                    {
                        if (idx + 8 >= 56)
                        {
                        }
                        else
                        {
                            nextMoveGame->pieces[idx + 8] = currPiece;
                            std::string boardFEN = nextMoveGame->ConvertToFEN();
                            nextMoveGame->occurrences[boardFEN] = 1;
                            pseudoLegalMoves.push_back(nextMoveGame);
                            nextMoveGame = new Chess(*nextMoveGame);
                            nextMoveGame->pieces[idx + 8] = nullptr;
                            nextMoveGame->occurrences.erase(boardFEN);
                            if (idx < 16 && !this->pieces[idx + 16])
                            {
                                nextMoveGame->pieces[idx + 16] = currPiece;
                                nextMoveGame->enPassantIdx = idx + 8;
                                boardFEN = nextMoveGame->BoardIdx();
                                nextMoveGame->occurrences[boardFEN] = 1;
                                pseudoLegalMoves.push_back(nextMoveGame);
                                nextMoveGame = new Chess(*nextMoveGame);
                                nextMoveGame->pieces[idx + 16] = nullptr;
                                nextMoveGame->enPassantIdx = -1;
                                nextMoveGame->occurrences.erase(boardFEN);
                            }
                        }
                    }
                }
                else
                {
                }

                // Black Pawns
                // - Black Forward
                //   - If pawn promotes, try all
                //   - Else 2 forward
                // - Black Takes Left
                //   - If pawn promotes, try all
                //   - Else just move pawn
                // - Black Takes Right
                //   - If pawn promotes, try all
                //   - Else just move pawn
                break;
            case Piece::Type::KNIGHT:
                // So much modular math rip
                break;
            case Piece::Type::BISHOP:
                // Sliding with increments of -9, -7, 7, 9
                break;
            case Piece::Type::ROOK:
                // Sliding with increments of -8, -1, 1, 8
                break;
            case Piece::Type::QUEEN:
                // Sliding with bishop + rook
                break;
            default:
                // King goes one any direction
                break;
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