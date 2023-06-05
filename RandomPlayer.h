//
// Created by NXY666 on 2023/6/5.
//

#ifndef OTHELLOAI_RANDOMPLAYER_H
#define OTHELLOAI_RANDOMPLAYER_H

#include "Othello.h"
#include "Player.h"

class RandomPlayer : public Player {
public:
    Point selectAction(Board &board, Tile self_tile, std::vector<Point> valid_moves) override;
};


#endif //OTHELLOAI_RANDOMPLAYER_H
