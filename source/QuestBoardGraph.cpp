#include "QuestBoardGraph.h"

const int QuestBoardGraph::NODES_X = 26;
const int QuestBoardGraph::NODES_Y = 19;

/*!
 * Creates nodes and edges representing the quest board including
 * all the walls printed on the board.
 */
QuestBoardGraph::QuestBoardGraph(QuestBoard* quest_board)
:
BoardGraph(NODES_X, NODES_Y) // connects each node to each of its direct, non-diagonal neighbors
{
	insertWalls();
}

QuestBoardGraph::~QuestBoardGraph()
{
	; // nix
}

/*!
 * Removes node connections such that the quest board walls are
 * respected.
 */
void QuestBoardGraph::insertWalls()
{
	// top left quarter
	insertWallHorizontal(0, 1, 11);
	insertWallHorizontal(3, 1, 8);
	insertWallHorizontal(5, 9, 11);
	insertWallHorizontal(8, 1, 8);
	insertWallVertical(0, 1, 8);
	insertWallVertical(4, 1, 8);
	insertWallVertical(8, 1, 8);
	insertWallVertical(11, 1, 5);

	// top right quarter
	insertWallHorizontal(0, 14, 24);
	insertWallHorizontal(4, 17, 24);
	insertWallHorizontal(5, 14, 16);
	insertWallHorizontal(8, 17, 24);
	insertWallVertical(13, 1, 5);
	insertWallVertical(16, 1, 8);
	insertWallVertical(20, 1, 8);
	insertWallVertical(24, 1, 8);

	// bottom left quarter
	insertWallHorizontal(9, 1, 8);
	insertWallHorizontal(12, 5, 11);
	insertWallHorizontal(13, 1, 4);
	insertWallHorizontal(17, 1, 11);
	insertWallVertical(0, 10, 17);
	insertWallVertical(4, 10, 17);
	insertWallVertical(6, 10, 12);
	insertWallVertical(8, 10, 17);
	insertWallVertical(11, 13, 17);

	// bottom right quarter
	insertWallHorizontal(9, 17, 24);
	insertWallHorizontal(12, 14, 17);
	insertWallHorizontal(13, 18, 24);
	insertWallHorizontal(17, 14, 24);
	insertWallVertical(13, 13, 17);
	insertWallVertical(16, 10, 12);
	insertWallVertical(17, 13, 17);
	insertWallVertical(20, 10, 17);
	insertWallVertical(24, 10, 17);

	// center
	insertWallHorizontal(6, 10, 15);
	insertWallHorizontal(11, 10, 15);
	insertWallVertical(9, 7, 11);
	insertWallVertical(15, 7, 11);
}

