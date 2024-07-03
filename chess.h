#ifndef CHESS_H
#define CHESS_H

#include <cstdint>
#include <string>

constexpr uint64_t FILE_A = 0x8080808080808080;
constexpr uint64_t FILE_B = 0x4040404040404040;
constexpr uint64_t FILE_C = 0x2020202020202020;
constexpr uint64_t FILE_D = 0x1010101010101010;
constexpr uint64_t FILE_E = 0x0808080808080808;
constexpr uint64_t FILE_F = 0x0404040404040404;
constexpr uint64_t FILE_G = 0x0202020202020202;
constexpr uint64_t FILE_H = 0x0101010101010101;

constexpr uint64_t RANK_1 = 0xFF00000000000000;
constexpr uint64_t RANK_2 = 0x00FF000000000000;
constexpr uint64_t RANK_3 = 0x0000FF0000000000;
constexpr uint64_t RANK_4 = 0x000000FF00000000;
constexpr uint64_t RANK_5 = 0x00000000FF000000;
constexpr uint64_t RANK_6 = 0x0000000000FF0000;
constexpr uint64_t RANK_7 = 0x000000000000FF00;
constexpr uint64_t RANK_8 = 0x00000000000000FF;

constexpr uint64_t up(uint64_t board) { return (board & ~RANK_8) >> 8; }
constexpr uint64_t down(uint64_t board) { return (board & ~RANK_1) << 8; }
constexpr uint64_t left(uint64_t board) { return (board & ~FILE_A) << 1; }
constexpr uint64_t right(uint64_t board) { return (board & ~FILE_H) >> 1; }

constexpr uint64_t upleft(uint64_t board) { return (board & ~RANK_8 & ~FILE_A) >> 7; }
constexpr uint64_t upright(uint64_t board) { return (board & ~RANK_8 & ~FILE_H) >> 9; }
constexpr uint64_t downleft(uint64_t board) { return (board & ~RANK_1 & ~FILE_A) << 9; }
constexpr uint64_t downright(uint64_t board) { return (board & ~RANK_1 & ~FILE_H) << 7; }

class Chess {

};

#endif