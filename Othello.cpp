//
// Created by NXY666 on 2023/6/4.
//

#include <valarray>
#include <iostream>
#include <random>
#include <memory>
#include "Othello.h"

int random_int(int min, int max) {
    // 使用 std::random_device 获取真正的随机数设备
    std::random_device rd;

    // 使用 std::mt19937 引擎，并使用 std::random_device 提供的随机数种子进行初始化
    std::mt19937 gen(rd());

    // 使用 std::uniform_int_distribution 生成指定范围内的均匀分布的整数
    std::uniform_int_distribution<int> dist(min, max);

    // 返回生成的随机数
    return dist(gen);
}

Tile get_oppo_tile(Tile tile);

// 创建一个新的棋盘
std::shared_ptr<Board> get_new_board_ptr() {
    return std::make_shared<Board>(8, std::vector<Tile>(8, Tile::EMPTY));
}

// 复制棋盘
std::shared_ptr<Board> get_copy_board_ptr(const std::shared_ptr<Board>& board) {
    std::shared_ptr<Board> dupe_board_ptr = get_new_board_ptr();

    for (int x = 0; x < 8; ++x) {
        for (int y = 0; y < 8; ++y) {
            (*dupe_board_ptr)[x][y] = board->at(x)[y];
        }
    }

    return dupe_board_ptr;
}

// 初始化棋盘
void init_board(Board &board) {
    for (int x = 0; x < 8; ++x) {
        for (int y = 0; y < 8; ++y) {
            board[x][y] = Tile::EMPTY;
        }
    }

    // 开局时放置初始棋子
    board[3][3] = Tile::WHITE;
    board[3][4] = Tile::BLACK;
    board[4][3] = Tile::BLACK;
    board[4][4] = Tile::WHITE;
}

// 获取棋盘上黑白双方的棋子数
std::pair<int, int> get_board_score(Board &board, Tile self_tile) {
    int self_score = 0, oppo_score = 0;

    Tile oppo_tile = get_oppo_tile(self_tile);

    for (int x = 0; x < 8; ++x) {
        for (int y = 0; y < 8; ++y) {
            if (board[x][y] == self_tile) {
                self_score++;
            } else if (board[x][y] == oppo_tile) {
                oppo_score++;
            }
        }
    }

    return {self_score, oppo_score};
}

// 获取可落子的位置
std::vector<Point> get_valid_moves(Board &board, Tile tile) {
    std::vector<Point> valid_moves;

    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            if (!is_move_valid(board, tile, row, col).empty()) {
                valid_moves.emplace_back(row, col);
            }
        }
    }

    return valid_moves;
}

/**
 * @brief 确定指定的位置是否为有效的落子位置。
 *
 * @param board 用于检查有效落子的游戏棋盘。
 * @param self_tile 要检查有效落子的棋子。
 * @param x_start 起始位置的 x 坐标。
 * @param y_start 起始位置的 y 坐标。
 * @return 输出落子及吃子的位置。如果不可落子，则返回空向量。
 */
std::vector<Point> is_move_valid(Board &board, Tile self_tile, int x_start, int y_start) {
    if (!is_on_board(x_start, y_start) || board[x_start][y_start] != Tile::EMPTY) {
        return {};
    }

    board[x_start][y_start] = self_tile;

    Tile oppo_tile = get_oppo_tile(self_tile);

    std::vector<Point> ls_flip;

    for (auto [x_direction, y_direction]: std::vector<Point>{{0,  1},
                                                             {1,  1},
                                                             {1,  0},
                                                             {1,  -1},
                                                             {0,  -1},
                                                             {-1, -1},
                                                             {-1, 0},
                                                             {-1, 1}}) {
        int x = x_start + x_direction;
        int y = y_start + y_direction;
        if (is_on_board(x, y) && board[x][y] == oppo_tile) {
            x += x_direction;
            y += y_direction;
            if (!is_on_board(x, y)) {
                continue;
            }
            while (board[x][y] == oppo_tile) {
                x += x_direction;
                y += y_direction;
                if (!is_on_board(x, y)) {
                    break;
                }
            }
            if (!is_on_board(x, y)) {
                continue;
            }
            if (board[x][y] == self_tile) {
                while (x != x_start || y != y_start) {
                    x -= x_direction;
                    y -= y_direction;
                    ls_flip.emplace_back(x, y);
                }
            }
        }
    }

    board[x_start][y_start] = Tile::EMPTY;

    return ls_flip;
}

/**
 * @brief 检查指定位置是否在棋盘上。
 *
 * @param x 检查的 x 坐标。
 * @param y 检查的 y 坐标。
 * @return 如果在棋盘上，则返回 true，否则返回 false。
 */
bool is_on_board(int x, int y) {
    return x >= 0 && x < 8 && y >= 0 && y < 8;
}

/**
 * @brief 落子
 *
 * @param board 用于落子的游戏棋盘。
 * @param tile 要落子的棋子。
 * @param x_start 起始位置的 x 坐标。
 * @param y_start 起始位置的 y 坐标。
 */
bool go_for_move(Board &board, Tile tile, int x_start, int y_start) {
    std::vector<Point> tile_flip = is_move_valid(board, tile, x_start, y_start);

    if (tile_flip.empty()) {
        return false;
    }

    board[x_start][y_start] = tile;
    for (auto [x, y]: tile_flip) {
        board[x][y] = tile;
    }
    return true;
}

// 检查游戏是否结束
bool is_game_over(Board &board) {
    auto score = get_board_score(board, Tile::BLACK);
    if (score.first == 0 || score.second == 0) {
        return true;
    }
    if (get_valid_moves(board, Tile::BLACK).empty() && get_valid_moves(board, Tile::WHITE).empty()) {
        return true;
    }
    return false;
}

//int main() {
//    game_loop(5, mcts_1_move, mcts_2_move);
//    return 0;
//}