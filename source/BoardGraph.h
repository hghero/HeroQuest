#ifndef BOARDGRAPH_H
#define BOARDGRAPH_H

#include <vector>
#include "GraphBase.h"
#include "Math.h"

class QuestBoard;
class SaveContext;
class LoadContext;

/*!
 * The directed graph of a board, without additional elements such as
 * obstacles, doors, furnature or monsters.
 * Edges are implicit: nodes have neighbors.
 */
class BoardGraph
{
public:
	BoardGraph(int width, int height);
	
	virtual ~BoardGraph();
	
	void resize(int width, int height);

	Node& getNode(const NodeID& node_id);
	const Node& getNode(const NodeID& node_id) const;

	bool isEdge(const Node& from, const Node& to) const;
	bool isEdge(const NodeID& from, const NodeID& to) const;
    bool isCorner(const NodeID& from, const NodeID& to) const;

    void getShortestPath(const NodeID& from, const NodeID& to, bool pass_heros, bool pass_monsters, bool through_walls,
            unsigned int max_path_length, std::vector<NodeID>* path) const;
	bool getNearestNeighborNode(const NodeID& node, NodeID* nearest_neighbor_node);

    void getReachableArea(const NodeID& node_id_from, uint movement_points, bool pass_monsters, bool through_walls,
            std::set<NodeID>* reachable_area);

	bool contains(const NodeID& node_id) const;

	bool isWall(const NodeID& node1, const NodeID& node2) const;

    virtual bool save(SaveContext& save_context) const;
    virtual bool load(LoadContext& load_context);

protected:
	void insertWalls();

	void insertWallHorizontal(int row, int start_col, int end_col);
	void insertWallVertical(int col, int start_row, int end_row);

    const std::vector<NodeID> getNeighborNodesIgnoringWalls(const NodeID& node_id) const;

	//! each field is a node; _nodes simply contains all nodes;
	//! each node knows it's neighbors explicitly as a list of neighbors
	StorageMatrix<Node> _nodes;

	//! to be able to distinguish between non-visibility caused by walls and non-visibility caused by
	//! debris, we need to explicitly save the wall positions
	std::set<std::pair<NodeID, NodeID> > _walls;
};

#endif
