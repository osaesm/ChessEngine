#include "chess.h"

// constexpr int RookHash(short idx, uint64_t blockers) {
//   int leftSide = 0;
//   while (leftSide < 8 && ~(blockers & (1ULL << (idx - leftSide)))) {
//     ++leftSide;
//   }
//   leftSide--;
//   int rightSide = 0;
//   while (rightSide < 8 && ~(blockers & (1ULL << (idx + rightSide)))) {
//     ++rightSide;
//   }
//   rightSide--;
//   int upSide = 0;
//   while (upSide < 8 && ~(blockers & (1ULL << (idx + (upSide * 8))))) {
//     ++upSide;
//   }
//   upSide--;
//   int downSide = 0;
//   while (downSide < 8 && ~(blockers & (1ULL << (idx - (downSide * 8))))) {
//     ++downSide;
//   }
//   downSide--;
//   return (leftSide << 9) + (upSide << 6) + (rightSide << 3) + downSide;
// }

// constexpr int BishopHash(short idx, uint64_t blockers) {
//   int upLeftSide = 0;
//   while (upLeftSide < 8 && ~(blockers & (1ULL << (7 * (idx + upLeftSide))))) {
//     ++upLeftSide;
//   }
//   upLeftSide--;
//   int upRightSide = 0;
//   while (upRightSide < 8 && ~(blockers & (1ULL << (9 * (idx + upRightSide))))) {
//     ++upRightSide;
//   }
//   upRightSide--;
//   int downRightSide = 0;
//   while (downRightSide < 8 && ~(blockers & (1ULL << (7 * (idx - downRightSide))))) {
//     ++downRightSide;
//   }
//   downRightSide--;
//   int downLeftSide = 0;
//   while (downLeftSide < 8 && ~(blockers & (1ULL << (9 * (idx - downLeftSide))))) {
//     ++downLeftSide;
//   }
//   downLeftSide--;
//   return (upLeftSide << 9) + (upRightSide << 6) + (downRightSide << 3) + downLeftSide;
// }