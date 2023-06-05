//
// Created by NXY666 on 2023/6/5.
//

#include <limits>
#include <random>
#include <valarray>
#include <iostream>
#include <ctime>
#include "MctsPlayer.h"

struct Node {
    // 访问次数
    int visits;

    // 胜利次数
    double reward;

    // 当前棋盘状态
    Board &now_board;

    // 子节点们
    std::vector<Node *> children;

    // 父节点
    Node *parent;

    // 落子位置
    Point action;

    // 落子颜色
    Tile tile;

    explicit Node(Board &now_board, Node *parent = nullptr, Point action = std::make_pair(0, 0),
                  Tile tile = Tile::EMPTY)
            : visits(0),
              reward(0.0),
              now_board(now_board),
              parent(parent),
              action(std::move(action)),
              tile(tile) {}

    explicit Node(Board &now_board, Tile tile = Tile::EMPTY)
            : visits(0),
              reward(0.0),
              now_board(now_board),
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

    void add_child(Board &child_now_board, Point child_action, Tile child_tile) {
        children.push_back(new Node(child_now_board, this, child_action, child_tile));
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
            std::cout << "delete" << std::endl;
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

Point
MCTS(Node *root, Tile self_tile, Board &board, unsigned long difficulty, Point (*get_action_func)(std::vector<Point> &)) {
    std::cout << "AI Start: " << difficulty << std::endl;
    unsigned long end_time = std::time(nullptr) + difficulty;
    int play_out = 0;

    while (true) {
        // 选择节点
        Node *selected_node = select(root);
        // 扩展节点
        Node *leaf_node = expand(selected_node, board);
        // 模拟
        int reward = simulate(leaf_node, self_tile, get_action_func);
        // 回溯
        backup(reward, leaf_node, self_tile);

        play_out++;

        if (std::time(nullptr) > end_time) {
            std::cout << "AI End" << std::endl;
            std::cout << "PlayOut: " << play_out << std::endl;
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

Node *expand(Node *node, Board &board) {
    if (node->visits == 0) {
        return node;
    } else {
        Tile new_tile = get_oppo_tile(node->tile);

        if (get_valid_moves(board, new_tile).empty()) {
            new_tile = get_oppo_tile(new_tile);
        }

        for (const auto &action: get_valid_moves(board, new_tile)) {
            Board *new_board_ptr = get_copy_board_ptr(node->now_board);
            Board &new_board = *new_board_ptr;
            go_for_move(new_board, new_tile, action.first, action.second);
            node->add_child(new_board, action, new_tile);
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
    // 乱序排列action_list
    std::shuffle(action_list.begin(), action_list.end(), std::mt19937(std::random_device()()));

//    std::cout << "get_roxanne_action: " << action_list.size() << std::endl;
    Point action = action_list[0];
    int max_value = board_gravity[action.first][action.second];

    for (const auto &now_action: action_list) {
        int now_value = board_gravity[now_action.first][now_action.second];
//        std::cout << "now_action: " << now_action.first << " " << now_action.second << " v: " << now_value << std::endl;
        if (now_value > max_value) {
            max_value = now_value;
            action = now_action;
        }
    }

//    std::cout << "action: " << action.first << " " << action.second << std::endl;

    return action;
}

Point get_random_action(std::vector<Point> &action_list) {
    return action_list[random_int(0, static_cast<int>(action_list.size()) - 1)];
}

int simulate(Node *node, Tile self_tile, Point (*get_action_func)(std::vector<Point> &)) {
    Board *board_ptr = get_copy_board_ptr(node->now_board);
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

Point MctsPlayer::selectAction(Board board, Tile self_tile, std::vector<Point> valid_moves) {
    Tile oppo_tile = get_oppo_tile(self_tile);

    Board *copied_board_ptr = get_copy_board_ptr(board);
    Board &copied_board = *copied_board_ptr;

    Node *root = new Node(copied_board, oppo_tile);

    Point action = MCTS(root, self_tile, board, difficulty, get_action_func);

    std::cout << "ValidMoves\tRewards\tVisits" << std::endl;
    for (Node *child: root->children) {
        std::cout << '(' << child->action.second + 1 << ", " << child->action.first + 1 << ')' << '\t'
                  << ':' << '\t'
                  << child->reward << '\t'
                  << child->visits << std::endl;
    }
    return action;
}