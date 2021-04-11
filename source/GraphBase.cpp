#include "GraphBase.h"
#include <QtCore/QtGlobal>

#include "StreamUtils.h"

using namespace std;

// ===============================================

NodeID::NodeID()
:
_ix(0),
_iy(0)
{
	// NIX
}

NodeID::NodeID(const NodeID& other)
:
_ix(other._ix),
_iy(other._iy)
{
	// NIX
}

NodeID::NodeID(int ix, int iy)
:
_ix(ix),
_iy(iy)
{
	// NIX
}

NodeID::~NodeID()
{
	// NIX
}

bool NodeID::operator==(const NodeID& other) const
{
	return _ix == other._ix && _iy == other._iy;
}

bool NodeID::operator!=(const NodeID& other) const
{
	return !operator==(other);
}

bool NodeID::operator<(const NodeID& other) const
{
	if (_iy < other._iy)
		return true;
	else if (_iy > other._iy)
		return false;
	else
		return _ix < other._ix;
}

NodeID NodeID::operator+(const NodeID& other) const
{
	NodeID result(*this);
	result._ix += other._ix;
	result._iy += other._iy;
	return result;
}

NodeID NodeID::operator-(const NodeID& other) const
{
	NodeID result(*this);
	result._ix -= other._ix;
	result._iy -= other._iy;
	return result;
}

NodeID& NodeID::operator=(const NodeID& other)
{
	_ix = other._ix;
	_iy = other._iy;
	return *this;
}

unsigned int NodeID::numArgsSerialized()
{
	return 2;
}

void NodeID::serialize(vector<void*>* target) const
{
	target->push_back((void*)(_ix));
	target->push_back((void*)(_iy));
}

ostream& operator<<(ostream& s, const NodeID& node_id)
{
	s << "(" << node_id._ix << ", " << node_id._iy << ")";
	return s;
}

bool NodeID::save(ostream& stream) const
{
    stream.write((char*)&_ix, sizeof(_ix));
    stream.write((char*)&_iy, sizeof(_iy));
    return !stream.fail();
}

bool NodeID::load(istream& stream)
{
    char buffer_int[sizeof(int)];
    stream.read((char*)buffer_int, sizeof(int));
    _ix = *((uint*)buffer_int);
    stream.read((char*)buffer_int, sizeof(int));
    _iy = *((uint*)buffer_int);
    return !stream.fail();
}

// ===============================================

Node::Node()
:
_node_id(0, 0),
_graph(0),
_neighbors()
{
	; // nix
}

Node::Node(BoardGraph* graph, const NodeID& node_id)
:
_node_id(node_id),
_graph(graph),
_neighbors()
{
	; // nix
}

Node::~Node()
{
	; // nix
};

bool Node::operator==(const Node& other) const
{
	return _node_id == other._node_id;
}

bool Node::operator!=(const Node& other) const
{
	return !operator==(other);
}

const Node& Node::operator=(const Node& other)
{
	setNodeID(other._node_id);
	setGraph(other._graph);
	setNeighbors(other._neighbors); // copy the whole vector
	return *this;
}

void Node::setNodeID(const NodeID& node_id)
{
	_node_id = node_id;
}

const NodeID& Node::getNodeID() const
{
	return _node_id;
}

void Node::setGraph(BoardGraph* graph)
{
	_graph = graph;
}

const BoardGraph* Node::getGraph() const
{
	return _graph;
}

void Node::setNeighbors(const std::vector<NodeID>& neighbors)
{
	clearNeighbors();
	_neighbors = neighbors; // copy all
}

const std::vector<NodeID>& Node::getNeighbors() const
{
	return _neighbors;
}

void Node::addNeighborAbsolute(const NodeID& node_id)
{
	_neighbors.push_back(node_id);
}

void Node::addNeighborRelative(int rx, int ry)
{
	NodeID neighbor_id(_node_id._ix + rx, _node_id._iy + ry);
	addNeighborAbsolute(neighbor_id);
}

void Node::clearNeighbors()
{
	_neighbors.clear();
}

/*!
* \return True, if node_id has been removed successfully; false, if node_id was not found in _neighbors.
*/
bool Node::removeNeighborAbsolute(const NodeID& node_id)
{
	for (std::vector<NodeID>::iterator it = _neighbors.begin(); it != _neighbors.end(); ++it)
	{
		if (*it == node_id)
		{
			_neighbors.erase(it);
			return true;
		}
	}

	return false;
}

/*!
* \return True, if relative neighbor (rx, ry) has been removed successfully; false, if that neighbor
* was not found in _neighbors.
*/
bool Node::removeNeighborRelative(int rx, int ry)
{
	return removeNeighborAbsolute(NodeID(_node_id._ix + rx, _node_id._iy + ry));
}

bool Node::isNeighbor(const NodeID& node_id) const
{
	for (vector<NodeID>::const_iterator it = _neighbors.begin(); it != _neighbors.end(); ++it)
	{
		if (*it == node_id)
			return true;
	}

	return false;
}

/*!
 * @param node_id
 * @return True iff other_node_id has a common neighbour with this node.
 */
bool Node::hasCommonNeighborWith(const Node& other_node) const
{
    const vector<NodeID>& other_node_neighbors = other_node.getNeighbors();
    for (vector<NodeID>::const_iterator it_neighbor = _neighbors.begin(); it_neighbor != _neighbors.end();
            ++it_neighbor)
    {
        const NodeID& neighbor = *it_neighbor;
        for (vector<NodeID>::const_iterator it_other_node_neighbor = other_node_neighbors.begin();
                it_other_node_neighbor != other_node_neighbors.end(); ++it_other_node_neighbor)
        {
            const NodeID& other_neighbor = *it_other_node_neighbor;
            if (other_neighbor == neighbor)
                return true;
        }
    }

    // no matching neighbor found
    return false;
}

bool Node::save(ostream& stream) const
{
    // _neighbors
    StreamUtils::writeUInt(stream, _neighbors.size());
    for (uint i = 0; i < _neighbors.size(); ++i)
        _neighbors[i].save(stream);

    return !stream.fail();
}

bool Node::load(istream& stream)
{
    // _neighbors
    _neighbors.clear();
    uint num_neighbors = 0;
    StreamUtils::readUInt(stream, &num_neighbors);
    _neighbors.reserve(num_neighbors);
    for (uint i = 0; i < num_neighbors; ++i)
    {
        NodeID neighbor(0, 0);
        neighbor.load(stream);
        _neighbors.push_back(neighbor);
    }

    return !stream.fail();
}

// ===============================================
