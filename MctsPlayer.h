//
// Created by NXY666 on 2023/6/5.
//

#ifndef OTHELLOAI_MCTSPLAYER_H
#define OTHELLOAI_MCTSPLAYER_H

#include "Othello.h"
#include "Player.h"

struct Node;

Point
MCTS(Node *root, Tile self_tile, Board &board, unsigned long difficulty, Point (*get_action_func)(std::vector<Point> &));

Node *select(Node *node);

Node *expand(Node *node, Board &board);

Point get_roxanne_action(std::vector<Point> &action_list);

Point get_random_action(std::vector<Point> &action_list);

int simulate(Node *node, Tile self_tile, Point (*get_action_func)(std::vector<Point> &));

void backup(double reward, Node *node, Tile self_tile);

class MctsPlayer : public Player {
    unsigned long difficulty;
    Point (*get_action_func)(std::vector<Point> &);
public:
    explicit MctsPlayer(unsigned long difficulty, Point (*get_action_func)(std::vector<Point> &));

    Point selectAction(Board board, Tile self_tile, std::vector<Point> valid_moves) override;
};


#endif //OTHELLOAI_MCTSPLAYER_H
