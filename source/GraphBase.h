#ifndef GRAPHBASE_H
#define GRAPHBASE_H

#include <vector>
#include <iostream>

class SaveContext;
class LoadContext;

// ===============================================

/*!
 * Plain node, comprising x and y coordinate index.
 */
class NodeID
{
public:
	NodeID();
	NodeID(const NodeID& other);
	NodeID(int ix, int iy);
	virtual ~NodeID();

	bool operator==(const NodeID& other) const;
	bool operator!=(const NodeID& other) const;
	bool operator<(const NodeID& other) const;
	NodeID operator+(const NodeID& other) const;
	NodeID operator-(const NodeID& other) const;
	NodeID& operator=(const NodeID& other);

	static unsigned int numArgsSerialized();
	void serialize(std::vector<void*>* target) const;
	
    virtual bool save(SaveContext& save_context) const;
    virtual bool load(LoadContext& load_context);

public:
	int _ix;
	int _iy;
};

std::ostream& operator<<(std::ostream& s, const NodeID& node_id);

// ===============================================

class BoardGraph;

/*!
 * Node: NodeID plus properties and incident edges.
 */
class Node
{
public:
	Node();
    Node(BoardGraph* graph, const NodeID& node_id);
	virtual ~Node();

	bool operator==(const Node& other) const;
	bool operator!=(const Node& other) const;
	const Node& operator=(const Node& other);

	void setNodeID(const NodeID& node_id);
	const NodeID& getNodeID() const;

	void setGraph(BoardGraph* graph);
	const BoardGraph* getGraph() const;

	void setNeighbors(const std::vector<NodeID>& neighbors);
	const std::vector<NodeID>& getNeighbors() const;
	void addNeighborAbsolute(const NodeID& node_id);
	void addNeighborRelative(int rx, int ry);
	void clearNeighbors();
	bool removeNeighborAbsolute(const NodeID& node_id);
	bool removeNeighborRelative(int rx, int ry);
	bool isNeighbor(const NodeID& node_id) const;
    bool hasCommonNeighborWith(const Node& other_node) const;

    virtual bool save(SaveContext& save_context) const;
    virtual bool load(LoadContext& load_context);

private:
	// board matrix indices (0..width-1) x (0..height-1)
	NodeID _node_id;

	// Parent graph
	BoardGraph* _graph;

	// neighbors
	std::vector<NodeID> _neighbors;
};

// ===============================================

#endif
