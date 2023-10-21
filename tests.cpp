#include <iostream>

#include "board.h"

class TestBoard : public Board {
public:
    TestBoard() {
        Board();
    }
    void setTurn(TURN_COLORS newTurn) {
        turn = newTurn;
    }
    void setWhiteCastle(bool x) {
        whiteCastle = x;
    }
    void setWhiteLongCastle(bool x) {
        whiteLongCastle = x;
    }
    void setBlackCastle(bool x) {
        blackCastle = x;
    }
    void setBlackLongCastle(bool x) {
        blackLongCastle = x;
    }
    void setBitBoard(unsigned long b) {
        bitBoard = b;
    }
    void addPiece(short x, Piece* p) {
        if (isEmpty(x)) {
            activePieces[x] = p;
        }
    }
    void deletePiece(short x) {
        if (!isEmpty(x)) {
            delete activePieces[x];
            activePieces.erase(x);
        }
    }
};

bool firstTest() {
    TestBoard* testBoard = new TestBoard();

}


int main() {
}