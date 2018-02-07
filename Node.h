#pragma once

#include <vector>

class GameState;

class Node
{
public:
	Node(GameState* step);
	~Node();

	void addChild(Node* child);
	int getVisits() const { return m_visits; }
	void visit() { m_visits++; }
	float getValue() const { return m_value; }
	void addValue(const float val) { m_value += val; }
	float getProbability() const { return m_probablity; }
	void setProbability(const float val) { m_probablity = val; }
	bool isTerminal() const { return m_isTerminal; }
	std::vector<Node*> children() const { return m_children; }
	Node* getParent() const { return m_parent; }
	GameState* getState() const { return m_card; }
	int getChildCount() const { return m_children.size(); }
private:
	std::vector<Node*> m_children;
	Node* m_parent;
	float m_visits;
	float m_value;
	float m_probablity;
	bool m_isTerminal;
	GameState* m_card;
};