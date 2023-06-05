//
// Created by NXY666 on 2023/6/5.
//

#ifndef CPPTEST_PLAYER_H
#define CPPTEST_PLAYER_H

#include "types.h"

class Player {
public:
    virtual Point selectAction(Board &board, Tile self_tile, std::vector<Point> valid_moves) = 0;
};

#endif //CPPTEST_PLAYER_H
