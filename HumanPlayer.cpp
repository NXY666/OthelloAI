//
// Created by NXY666 on 2023/6/5.
//

#include <iostream>
#include "HumanPlayer.h"

Point HumanPlayer::selectAction(Board &board, Tile self_tile, std::vector<Point> valid_moves) {
    int x, y;
    do {
        std::cout << "ÇëÊäÈë×ø±ê£º";
        std::cin >> y >> x;
        std::cout << std::endl;
        x--;
        y--;
    } while (is_move_valid(board, self_tile, x, y).empty());
    return std::make_pair(x, y);
}
