//
// Created by NXY666 on 2023/6/4.
//

#ifndef CPPTEST_OTHELLO_H
#define CPPTEST_OTHELLO_H


#include <utility>
#include <vector>
#include "Othello.h"
#include "types.h"

int random_int(int min, int max);

void init_board(Board &board);

std::vector<Point> is_move_valid(Board &board, Tile self_tile, int x_start, int y_start);

bool is_on_board(int x, int y);

std::vector<Point> get_valid_moves(Board &board, Tile tile);

std::pair<int, int> get_board_score(Board &board, Tile self_tile);

bool go_for_move(Board &board, Tile tile, int x_start, int y_start);

Board *get_new_board_ptr();

Board *get_copy_board_ptr(Board &board);

bool is_game_over(Board &board);


#endif //CPPTEST_OTHELLO_H
