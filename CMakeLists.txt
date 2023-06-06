cmake_minimum_required(VERSION 3.22.1)
project(OthelloAI)

set(CMAKE_CXX_STANDARD 23)

add_executable(OthelloAI main.cpp types.h Othello.h Othello.cpp Player.h MctsPlayer.h MctsPlayer.cpp RandomPlayer.h RandomPlayer.cpp HumanPlayer.h HumanPlayer.cpp)
