//
// Created by NXY666 on 2023/6/5.
//

#ifndef OTHELLOAI_TYPES_H
#define OTHELLOAI_TYPES_H

#include <utility>
#include <vector>

enum class Tile {
    EMPTY = ' ',
    BLACK = 'X',
    WHITE = 'O'
};

typedef std::pair<int, int> Point;

typedef std::vector<std::vector<Tile>> Board;

#endif //OTHELLOAI_TYPES_H
