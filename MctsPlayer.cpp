//
// Created by NXY666 on 2023/6/5.
//

#include <limits>
#include <random>
#include <utility>
#include <valarray>
#include <iostream>
#include <ctime>
#include <stack>
#include <iomanip>
#include "MctsPlayer.h"

struct Node {
    // 访问次数
    int visits;

    // 胜利次数
    double reward;

    // 当前棋盘状态
    std::shared_ptr<Board> now_board;

    // 子节点们
    std::vector<Node *> children;

    // 父节点
    Node *parent;

    // 落子位置
    Point action;

    // 落子颜色
    Tile tile;

    explicit Node(std::shared_ptr<Board> now_board, Node *parent = nullptr, Point action = std::make_pair(0, 0),
                  Tile tile = Tile::EMPTY)
            : visits(0),
              reward(0.0),
              now_board(std::move(now_board)),
              parent(parent),
              action(std::move(action)),
              tile(tile) {}

    explicit Node(std::shared_ptr<Board> now_board, Tile tile = Tile::EMPTY)
            : visits(0),
              reward(0.0),
              now_board(std::move(now_board)),
              parent(nullptr),
              action(std::make_pair(0, 0)),
              tile(tile) {}

    [[nodiscard]] double get_ucb() const {
        if (visits == 0) {
            // 如果节点没有被访问过，那么就返回无限大
            return std::numeric_limits<double>::max();
        }

        double explore = std::sqrt(2.0 * std::log(parent->visits) / static_cast<double>(visits));
        double now_ucb = reward / visits + explore;
        return now_ucb;
    }

    void add_child(std::shared_ptr<Board> child_now_board, Point child_action, Tile child_tile) {
        children.push_back(new Node(std::move(child_now_board), this, child_action, child_tile));
    }

    bool full_expanded() {
        // 如果没有子节点，那么返回false
        if (children.empty()) {
            return false;
        }

        // 所有子节点都被访问过则返回true，否则返回false
        return std::all_of(children.begin(), children.end(), [](Node *child) {
            return child->visits != 0;
        });
    }

    ~Node() {
        for (Node *child: children) {
            delete child;
        }
    }
};

Tile get_oppo_tile(Tile tile) {
    if (tile == Tile::BLACK) {
        return Tile::WHITE;
    } else if (tile == Tile::WHITE) {
        return Tile::BLACK;
    } else {
        return Tile::EMPTY;
    }
}

Point MCTS(
        Node *root,
        Tile self_tile,
        const std::shared_ptr<Board> &board_ptr,
        unsigned long difficulty,
        Point (*get_action_func)(std::vector<Point> &)
) {
    std::cout << "> MCTS AI 决定思考 " << difficulty << " 秒。" << std::endl;
    unsigned long end_time = std::time(nullptr) + difficulty;
    int play_out = 0;

    while (true) {
        // 选择节点
        Node *selected_node = select(root);
        // 扩展节点
        Node *leaf_node = expand(selected_node, board_ptr);
        // 模拟
        int reward = simulate(leaf_node, self_tile, get_action_func);
        // 回溯
        backup(reward, leaf_node, self_tile);

        play_out++;

        if (std::time(nullptr) > end_time) {
            std::cout << "> MCTS AI 在此期间模拟了 " << play_out << " 局。" << std::endl;
            break;
        }
    }

    Node *max_node = root->children.empty() ? root : root->children[0];
    double max_visit = -1;

    for (Node *child: root->children) {
        double child_visit = child->visits;
        if (max_visit < child_visit) {
            max_visit = child_visit;
            max_node = child;
        }
    }

    return max_node->action;
}

Node *select(Node *node) {
    while (true) {
        if (node->children.empty()) {
            return node;
        }
        if (node->full_expanded()) {
            Node *max_node = nullptr;
            double max_ucb = -1;

            for (Node *child: node->children) {
                double child_ucb = child->get_ucb();
                if (max_ucb < child_ucb) {
                    max_ucb = child_ucb;
                    max_node = child;
                }
            }

            node = max_node;
        } else {
            for (Node *child: node->children) {
                if (child->visits == 0) {
                    return child;
                }
            }
        }
    }
}

Node *expand(Node *node, const std::shared_ptr<Board> &board_ptr) {
    if (node->visits == 0) {
        return node;
    } else {
        Tile new_tile = get_oppo_tile(node->tile);

        Board &board = *board_ptr;

        if (get_valid_moves(board, new_tile).empty()) {
            new_tile = get_oppo_tile(new_tile);
        }

        for (const auto &action: get_valid_moves(board, new_tile)) {
            std::shared_ptr<Board> new_board_ptr = get_copy_board_ptr(node->now_board);
            go_for_move(*new_board_ptr, new_tile, action.first, action.second);
            node->add_child(new_board_ptr, action, new_tile);
        }

        if (node->children.empty()) {
            return node;
        }

        return node->children[0];
    }
}

std::vector<std::vector<int>> const board_gravity = {
        {9, 1, 8, 6, 6, 8, 1, 9},
        {1, 0, 2, 3, 3, 2, 0, 1},
        {8, 2, 7, 5, 5, 7, 2, 8},
        {6, 3, 5, 4, 4, 5, 3, 6},
        {6, 3, 5, 4, 4, 5, 3, 6},
        {8, 2, 7, 5, 5, 7, 2, 8},
        {1, 0, 2, 3, 3, 2, 0, 1},
        {9, 1, 8, 6, 6, 8, 1, 9}
};

Point get_roxanne_action(std::vector<Point> &action_list) {
    std::shuffle(action_list.begin(), action_list.end(), std::mt19937(std::random_device()()));

    Point action = action_list[0];
    int max_value = board_gravity[action.first][action.second];

    for (const auto &now_action: action_list) {
        int now_value = board_gravity[now_action.first][now_action.second];
        if (now_value > max_value) {
            max_value = now_value;
            action = now_action;
        }
    }

    return action;
}

Point get_random_action(std::vector<Point> &action_list) {
    return action_list[random_int(0, static_cast<int>(action_list.size()) - 1)];
}

int simulate(Node *node, Tile self_tile, Point (*get_action_func)(std::vector<Point> &)) {
    std::shared_ptr<Board> board_ptr = get_copy_board_ptr(node->now_board);
    Board &board = *board_ptr;

    Tile tile = node->tile;

    while (!is_game_over(board)) {
        std::vector<Point> action_list = get_valid_moves(board, tile);
        if (!action_list.empty()) {
            Point action = get_action_func(action_list);
            go_for_move(board, tile, action.first, action.second);
        } else {
            tile = get_oppo_tile(tile);

            action_list = get_valid_moves(board, tile);
            if (!action_list.empty()) {
                Point action = get_action_func(action_list);
                go_for_move(board, tile, action.first, action.second);
            } else {
                break;
            }
        }
        tile = get_oppo_tile(tile);
    }

    std::pair<int, int> scores = get_board_score(board, self_tile);
    int self_score = scores.first;
    int oppo_score = scores.second;

    return oppo_score < self_score ? 1 : 0;
}

void backup(double reward, Node *node, Tile self_tile) {
    while (node != nullptr) {
        node->visits++;
        if (node->tile == self_tile) {
            node->reward += reward;
        } else {
            node->reward -= reward;
        }
        node = node->parent;
    }
}

MctsPlayer::MctsPlayer(
        unsigned long difficulty,
        Point (*get_action_func)(std::vector<Point> &)
) : difficulty(difficulty),
    get_action_func(get_action_func) {}

Point MctsPlayer::selectAction(Board &board, Tile self_tile, std::vector<Point> valid_moves) {
    Tile oppo_tile = get_oppo_tile(self_tile);

    std::shared_ptr<Board> board_ptr = std::make_shared<Board>(board);

    std::shared_ptr<Board> copied_board_ptr = get_copy_board_ptr(board_ptr);

    Node *root = new Node(copied_board_ptr, oppo_tile);

    Point action = MCTS(root, self_tile, board_ptr, difficulty, get_action_func);

    std::cout << "+----------+----------+----------+----------+" << std::endl;
    std::cout << "| 有效落子 |   奖励   |   访问   |   胜率   |" << std::endl;
    std::cout << "+----------+----------+----------+----------+" << std::endl;

    for (Node *child: root->children) {
        bool is_bold = child->action.first == action.first && child->action.second == action.second;
        double winRate = static_cast<double>(child->reward) / child->visits * 100;
        std::cout << "|  " << (is_bold ? "\033[1m\033[5m" : "")
                  << "(" << std::setw(1) << child->action.second + 1 << ", " << std::setw(1) << child->action.first + 1
                  << ") \033[0m"
                  << " |" << std::setw(9) << std::fixed << std::setprecision(0) << child->reward
                  << " |" << std::setw(9) << child->visits
                  << " |" << std::setw(8) << std::fixed << std::setprecision(2) << winRate << "% |" << std::endl;
    }

    std::cout << "+----------+----------+----------+----------+" << std::endl;

    delete root;

    return action;
}