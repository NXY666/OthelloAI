//
// Created by NXY666 on 2023/6/5.
//

#include <iostream>
#include <sstream>
#include <limits>
#include "HumanPlayer.h"

Point HumanPlayer::selectAction(Board &board, Tile self_tile, std::vector<Point> valid_moves) {
    std::string input;
    int x, y;
    while (true) {
        std::cout << "请输入坐标：";
        std::getline(std::cin, input);

        std::istringstream iss(input);
        if (iss >> y >> x && iss.eof()) {
            if (is_move_valid(board, self_tile, x - 1, y - 1).empty()) {
                std::cout << "非法走子。" << std::endl;
            } else {
                break;
            }
        } else {
            std::cout << "格式错误。" << std::endl;
        }
    }

    return std::make_pair(x - 1, y - 1);
}
