#include <iostream>
#include <tuple>
#include <limits>
#include <stringapiset.h>
#include <windows.h>
#include "MctsPlayer.h"
#include "RandomPlayer.h"
#include "HumanPlayer.h"

// 随机谁先走
bool rand_goes_first() {
    int random_num = random_int(0, 1);

    std::cout << "[随机] P" << random_num + 1 << " 先行。" << std::endl;

    return random_num == 1;
}

// 指定谁先走
bool who_goes_first() {
    int first;
    std::cout << "[指定] 请输入先手方（0代表P1，1代表P2）：";
    std::cin >> first;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    return first == 1;
}

void print_board(Board &board, Tile player_tile) {
    std::cout << "    1   2   3   4   5   6   7   8" << std::endl;
    std::cout << "  +---+---+---+---+---+---+---+---+" << std::endl;
    for (int row = 0; row < 8; ++row) {
        std::string output = std::to_string(row + 1) + " ";
        for (int col = 0; col < 8; ++col) {
            if (board[col][row] == Tile::BLACK) {
                output += "| ● ";
            } else if (board[col][row] == Tile::WHITE) {
                output += "| ◎ ";
            } else if (!is_move_valid(board, player_tile, col, row).empty()) {
                output += "| · ";
            } else {
                output += "|   ";
            }
        }
        output += "|";
        std::cout << output << std::endl;
        std::cout << "  +---+---+---+---+---+---+---+---+" << std::endl;
    }
}

void game_loop(int eval_rounds, Player &player_1, Player &player_2, bool (*select_goes_first_func)()) {
    int p1_wins = 0, p2_wins = 0, draw = 0;
    for (int i = 0; i < eval_rounds; ++i) {
        std::cout << "========================= 第 " << i + 1 << " 局 =========================" << std::endl
                  << std::endl;
        std::shared_ptr<Board> boardPtr = get_new_board_ptr();
        Board &board = *boardPtr;
        init_board(board);

        int who = select_goes_first_func();

        Tile player_1_tile, player_2_tile;
        if (who == 0) {
            player_1_tile = Tile::BLACK;
            player_2_tile = Tile::WHITE;
        } else {
            player_1_tile = Tile::WHITE;
            player_2_tile = Tile::BLACK;
        }

        while (true) {
            Tile curr_tile = who == 0 ? player_1_tile : player_2_tile;
            Tile oppo_tile = who == 0 ? player_2_tile : player_1_tile;
            Player &current_player = who == 0 ? player_1 : player_2;

            print_board(board, curr_tile);

            if (is_game_over(board)) {
                std::pair<int, int> scores = get_board_score(board, player_1_tile);
                if (scores.first > scores.second) {
                    std::cout << "* P1 获胜：" << scores.first << ":" << scores.second << std::endl;
                    ++p1_wins;
                } else if (scores.first == scores.second) {
                    std::cout << "* 双方平局：" << scores.first << ":" << scores.second << std::endl;
                    ++draw;
                } else {
                    std::cout << "* P2 获胜 " << scores.first << ":" << scores.second << std::endl;
                    ++p2_wins;
                }
                break;
            }

            std::cout << "* 轮到 P" << who + 1 << " 了。" << std::endl;

            std::vector<Point> valid_moves = get_valid_moves(board, curr_tile);

            std::cout << "* 合法落子：";
            for (const auto &move: valid_moves) {
                std::cout << "(" << move.second + 1 << " " << move.first + 1 << ") ";
            }
            std::cout << std::endl;

            int x, y;
            std::tie(x, y) = current_player.selectAction(board, curr_tile, valid_moves);
            go_for_move(board, curr_tile, x, y);

            std::cout << "* P" << who + 1 << " 执 " << (curr_tile == Tile::BLACK ? "●" : "◎") << " 在 (" << y + 1
                      << ", " << x + 1 << ") 处落子。" << std::endl;

            // 如果对方有合法落子，则轮到对方走
            if (!get_valid_moves(board, oppo_tile).empty()) {
                who = !who;
            }

            std::cout << std::endl << "-------------------- 回合结束 --------------------" << std::endl << std::endl;
        }
        std::cout << "* 本局游戏结束。" << std::endl;
    }

    std::cout << std::endl << "========================= 全部结束 =========================" << std::endl << std::endl;
    std::cout << "P1 胜利次数：" << p1_wins << std::endl;
    std::cout << "P2 胜利次数：" << p2_wins << std::endl;
    std::cout << "平局次数：" << draw << std::endl;
}

int main() {
    SetConsoleOutputCP(CP_UTF8);

    MctsPlayer mcts_player_1(1, get_random_action);
    MctsPlayer mcts_player_2(1, get_roxanne_action);

    RandomPlayer random_player_1;
    RandomPlayer random_player_2;

    HumanPlayer human_player_1;

    game_loop(5, mcts_player_1, random_player_1, rand_goes_first);

    system("pause");

    return 0;
}