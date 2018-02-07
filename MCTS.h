#pragma once

class Node;
class GameState;

GameState* MCTS_BestMove(GameState* game);
Node* MCTS_Selection(Node* current, GameState* game);
Node* MCTS_UCT(Node* current, float C);
// Probability UCT
Node* MCTS_UCTPlus(Node* current, float C1, float C2);
Node* MCTS_Expand(Node* current, GameState* game);
float MCTS_Simulate(Node* current, GameState* rootState);
void MCTS_Backpropagate(Node* current, float value);