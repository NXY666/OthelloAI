//
// Created by NXY666 on 2023/6/4.
//

#include <valarray>
#include <iostream>
#include <random>
#include <memory>
#include "Othello.h"

int random_int(int min, int max) {
    // ʹ�� std::random_device ��ȡ������������豸
    std::random_device rd;

    // ʹ�� std::mt19937 ���棬��ʹ�� std::random_device �ṩ����������ӽ��г�ʼ��
    std::mt19937 gen(rd());

    // ʹ�� std::uniform_int_distribution ����ָ����Χ�ڵľ��ȷֲ�������
    std::uniform_int_distribution<int> dist(min, max);

    // �������ɵ������
    return dist(gen);
}

Tile get_oppo_tile(Tile tile);

// ����һ���µ�����
std::shared_ptr<Board> get_new_board_ptr() {
    return std::make_shared<Board>(8, std::vector<Tile>(8, Tile::EMPTY));
}

// ��������
std::shared_ptr<Board> get_copy_board_ptr(const std::shared_ptr<Board>& board) {
    std::shared_ptr<Board> dupe_board_ptr = get_new_board_ptr();

    for (int x = 0; x < 8; ++x) {
        for (int y = 0; y < 8; ++y) {
            (*dupe_board_ptr)[x][y] = board->at(x)[y];
        }
    }

    return dupe_board_ptr;
}

// ��ʼ������
void init_board(Board &board) {
    for (int x = 0; x < 8; ++x) {
        for (int y = 0; y < 8; ++y) {
            board[x][y] = Tile::EMPTY;
        }
    }

    // ����ʱ���ó�ʼ����
    board[3][3] = Tile::WHITE;
    board[3][4] = Tile::BLACK;
    board[4][3] = Tile::BLACK;
    board[4][4] = Tile::WHITE;
}

// ��ȡ�����Ϻڰ�˫����������
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

// ��ȡ�����ӵ�λ��
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
 * @brief ȷ��ָ����λ���Ƿ�Ϊ��Ч������λ�á�
 *
 * @param board ���ڼ����Ч���ӵ���Ϸ���̡�
 * @param self_tile Ҫ�����Ч���ӵ����ӡ�
 * @param x_start ��ʼλ�õ� x ���ꡣ
 * @param y_start ��ʼλ�õ� y ���ꡣ
 * @return ������Ӽ����ӵ�λ�á�����������ӣ��򷵻ؿ�������
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
 * @brief ���ָ��λ���Ƿ��������ϡ�
 *
 * @param x ���� x ���ꡣ
 * @param y ���� y ���ꡣ
 * @return ����������ϣ��򷵻� true�����򷵻� false��
 */
bool is_on_board(int x, int y) {
    return x >= 0 && x < 8 && y >= 0 && y < 8;
}

/**
 * @brief ����
 *
 * @param board �������ӵ���Ϸ���̡�
 * @param tile Ҫ���ӵ����ӡ�
 * @param x_start ��ʼλ�õ� x ���ꡣ
 * @param y_start ��ʼλ�õ� y ���ꡣ
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

// �����Ϸ�Ƿ����
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