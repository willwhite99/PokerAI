#include "Node.h"
#include "GameState.h"
#include "Card.h"
#include <cassert>

Node::Node(GameState* step)
	: m_parent(NULL), m_visits(0), m_value(0), m_isTerminal(false), m_card(step), m_probablity(1.f)
{
	m_isTerminal = step->isTerminalState();
}

void Node::addChild(Node* child)
{
	assert(child->m_parent == NULL); // must be NULL
	assert(std::find(m_children.begin(), m_children.end(), child) == m_children.end()); // cannot already be a child

	m_children.push_back(child);
	child->m_parent = this;
}

Node::~Node()
{
	delete m_card;

	if (m_children.size() > 0)
	for (std::vector<Node*>::iterator it = m_children.begin(); it != m_children.end(); it++)
	{
		Node* n = *it;
		delete n;
	}
}