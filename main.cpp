#include <iostream>
#include <tuple>
#include "MctsPlayer.h"
#include "RandomPlayer.h"

// ���˭����
bool rand_goes_first() {
    int random_num = random_int(0, 1);

    std::cout << "[���] P" << random_num + 1 << " ���С�" << std::endl;

    return random_num == 1;
}

// ָ��˭����
bool who_goes_first() {
    int first;
    std::cout << "[ָ��] ���������ַ���0����P1��1����P2����";
    std::cin >> first;
    return first == 1;
}

void print_board(Board &board, Tile player_tile) {
    std::cout << "    1   2   3   4   5   6   7   8" << std::endl;
    std::cout << "  +---+---+---+---+---+---+---+---+" << std::endl;
    for (int row = 0; row < 8; ++row) {
        std::string output = std::to_string(row + 1) + " ";
        for (int col = 0; col < 8; ++col) {
            if (board[col][row] == Tile::BLACK) {
                output += "| �� ";
            } else if (board[col][row] == Tile::WHITE) {
                output += "| �� ";
            } else if (!is_move_valid(board, player_tile, col, row).empty()) {
                output += "| �� ";
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
        std::cout << "========================= �� " << i + 1 << " �� =========================" << std::endl;
        Board *boardPtr = get_new_board_ptr();
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
                    std::cout << "* P1 ��ʤ��" << scores.first << ":" << scores.second << std::endl;
                    ++p1_wins;
                } else if (scores.first == scores.second) {
                    std::cout << "* ˫��ƽ�֣�" << scores.first << ":" << scores.second << std::endl;
                    ++draw;
                } else {
                    std::cout << "* P2 ��ʤ " << scores.first << ":" << scores.second << std::endl;
                    ++p2_wins;
                }
                break;
            }

            std::cout << "* �ֵ� P" << who + 1 << " �ˡ�" << std::endl;

            std::vector<Point> valid_moves = get_valid_moves(board, curr_tile);

            std::cout << "* �Ϸ����ӣ�";
            for (const auto &move: valid_moves) {
                std::cout << "(" << move.second + 1 << " " << move.first + 1 << ") ";
            }
            std::cout << std::endl;

            int x, y;
            std::tie(x, y) = current_player.selectAction(board, curr_tile, valid_moves);
            go_for_move(board, curr_tile, x, y);

            std::cout << "* P" << who + 1 << " ִ " << (curr_tile == Tile::BLACK ? "��" : "��") << " �� (" << y + 1 << ", " << x + 1 << ") �����ӡ�" << std::endl;

            // ����Է��кϷ����ӣ����ֵ��Է���
            if (!get_valid_moves(board, oppo_tile).empty()) {
                who = !who;
            }

            std::cout << "-------------------- �غϽ��� --------------------" << std::endl;
        }
        std::cout << "* ������Ϸ������" << std::endl;
    }

    std::cout << "========================= ȫ������ =========================" << std::endl;
    std::cout << "P1 ʤ��������" << p1_wins << std::endl;
    std::cout << "P2 ʤ��������" << p2_wins << std::endl;
    std::cout << "ƽ�ִ�����" << draw << std::endl;
}

int main() {
    MctsPlayer mcts_player_1(5, get_random_action);
    MctsPlayer mcts_player_2(1, get_roxanne_action);

    RandomPlayer random_player_1;
    RandomPlayer random_player_2;

    game_loop(1, random_player_1, mcts_player_2, rand_goes_first);

    system("pause");

    return 0;
}
