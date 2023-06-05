//
// Created by NXY666 on 2023/6/5.
//

#include "RandomPlayer.h"

Point RandomPlayer::selectAction(Board board, Tile self_tile, std::vector<Point> valid_moves) {
    return valid_moves[random_int(0, static_cast<int>(valid_moves.size()) - 1)];
}
