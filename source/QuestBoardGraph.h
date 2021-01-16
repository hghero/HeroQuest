#ifndef QUESTBOARDGRAPH_H
#define QUESTBOARDGRAPH_H

#include "BoardGraph.h"

/*!
 * The directed graph of the standard quest board, without additional elements such as
 * obstacles, doors, furnature or monsters.
 * Edges are implicit: nodes have neighbors.
 */
class QuestBoardGraph : public BoardGraph
{
public:
	// node extent of the quest board in x and y
	static const int NODES_X;
	static const int NODES_Y;

	QuestBoardGraph(QuestBoard* quest_board);
	~QuestBoardGraph();

protected:
	void insertWalls();
};

#endif
