#include "BoardGraph.h"
#include "QuestBoard.h"
#include "Playground.h"
#include "HeroQuestLevelWindow.h"
#include "limits.h"
#include <list>
#include "Hero.h"
#include "Monster.h"
#include "Debug.h"
#include "Level.h"

using namespace std;

BoardGraph::BoardGraph(int width, int height)
:
	_nodes(),
	_walls()
{
	resize(width, height);
}
	
BoardGraph::~BoardGraph()
{
	; // nix
}

/*!
 * Resizes _nodes, and sets for each node: _node_id, _graph, _occupied, and _neighbors (connecting each node to each
 * of its direct, non-diagonal neighbors.
 */
void BoardGraph::resize(int width, int height)
{
	_nodes.resize(width, height);
	for (int y = 0; y < height; ++y)
		for (int x = 0; x < width; ++x)
		{
			_nodes[y][x].setNodeID(NodeID(x, y));
			_nodes[y][x].setGraph(this);

			_nodes[y][x].clearNeighbors();
			if (x > 0)
				_nodes[y][x].addNeighborRelative(-1, 0);
			if (x < _nodes.getWidth() - 1)
				_nodes[y][x].addNeighborRelative(1, 0);
			if (y > 0)
				_nodes[y][x].addNeighborRelative(0, -1);
			if (y < _nodes.getHeight() - 1)
				_nodes[y][x].addNeighborRelative(0, 1);
		}
}

/*!
 * To be overridden by derived classes.
 */
void BoardGraph::insertWalls()
{
	; // nix
}

/*!
 * Inserts a horizontal wall between the field rows (row) and (row + 1), starting
 * at field column start_col, and reaching up to field column end_col.
 */
void BoardGraph::insertWallHorizontal(int row, int start_col, int end_col)
{
	for (int col = start_col; col <= end_col; ++col)
	{
		_nodes[row][col].removeNeighborRelative(0, 1);
		_nodes[row + 1][col].removeNeighborRelative(0, -1);

		_walls.insert(make_pair(NodeID(col, row), NodeID(col, row + 1)));
	}
}

/*!
 * Inserts a vertical wall between the field columns (col) and (col + 1), starting
 * at field row start_row, and reaching up to field row end_row.
 */
void BoardGraph::insertWallVertical(int col, int start_row, int end_row)
{
	for (int row = start_row; row <= end_row; ++row)
	{
		_nodes[row][col].removeNeighborRelative(1, 0);
		_nodes[row][col + 1].removeNeighborRelative(-1, 0);

		_walls.insert(make_pair(NodeID(col, row), NodeID(col + 1, row)));
	}
}

Node& BoardGraph::getNode(const NodeID& node_id)
{
	return _nodes[node_id._iy][node_id._ix];
}

const Node& BoardGraph::getNode(const NodeID& node_id) const
{
	return _nodes[node_id._iy][node_id._ix];
}

bool BoardGraph::isEdge(const Node& from, const Node& to) const
{
	return from.isNeighbor(to.getNodeID());
}

bool BoardGraph::isEdge(const NodeID& from, const NodeID& to) const
{
	const Node& from_node = _nodes[from._iy][from._ix];
	return from_node.isNeighbor(to);
}

/*!
 * Computes a shortest path from from to to using Dijkstra's algorithm.
 * Note that there can be more than one shortest path.
 *
 * \param from The node from which a path to to shall be found.
 * \param to The node to which a path from from shall be found.
 * \param pass_heros True if heros may be in the way of the resulting path.
 * \param pass_monsters True if monsters may be in the way of the resulting path.
 * \param through_walls If true, walls are no obstacle.
 * \param max_path_length Only paths are found which have a length <= max_path_length.
 * \param path A shortest path from from to to. If no (shortest) path exists, path is empty. Path does contain the target node "to", but does not contain the source node "from".
 */
void BoardGraph::getShortestPath(const NodeID& from, const NodeID& to, bool pass_heros, bool pass_monsters,
        bool through_walls, unsigned int max_path_length, vector<NodeID>* path) const
{
	path->clear();

	if (from == to)
		return;

    Playground* playground = HeroQuestLevelWindow::_hero_quest->getPlayground();
    QuestBoard* quest_board = playground->getQuestBoard();

	StorageMatrix<int> dist(_nodes.getWidth(), _nodes.getHeight(), INT_MAX);
	dist[to._iy][to._ix] = 0;

	// Dijkstra: start at the target node ("to")
	list<NodeID> work_list;
	work_list.push_back(to);
	while (!work_list.empty())
	{
		NodeID current_node(work_list.front());
		work_list.pop_front();
		int current_node_dist = dist[current_node._iy][current_node._ix];

        const vector<NodeID> neighbors =
                (!through_walls) ? getNode(current_node).getNeighbors() : getNeighborNodesIgnoringWalls(current_node); // (non-walkable decoration is already excluded here)
		for (vector<NodeID>::const_iterator it = neighbors.begin(); it != neighbors.end(); ++it)
		{
			const NodeID& current_neighbor = *it;
			int current_neighbor_dist = dist[current_neighbor._iy][current_neighbor._ix];

			// is current_neighbor_node occupied by someone else?
            Creature* neighbor_creature = playground->getCreature(current_neighbor);
			if (neighbor_creature != 0 && HeroQuestLevelWindow::_hero_quest->getLevel()->getCurrentlyActingCreature() != neighbor_creature)
			{
				// Monster?
				if (neighbor_creature->isMonster() && !pass_monsters)
				{
					// may not pass this creature
					continue;
				}

				// Hero? (note: Heroes are generally not passable if they are caught in a trap!)
				if (neighbor_creature->isHero() &&
					(!pass_heros || playground->containsTrap(current_neighbor)))
				{
					// may not pass this creature
					continue;
				}
			}

			// update dist of neighbor, and add neighbor to working list if shorter path has been found
			int new_dist_to_neighbor = current_node_dist + 1;
			if (new_dist_to_neighbor < current_neighbor_dist)
			{
				work_list.push_back(current_neighbor);
				dist[current_neighbor._iy][current_neighbor._ix] = new_dist_to_neighbor;
			}
		}
	}

	// does a path exist?
	if (dist[from._iy][from._ix] == INT_MAX)
	{
		// no path having a maximum length of max_path_length exists from from to to
		return;
	}

	int min_dist_value = 0;
	if (max_path_length != UINT_MAX)
		min_dist_value = dist[from._iy][from._ix] - int(max_path_length);
	if (min_dist_value < 0)
		min_dist_value = 0;

	// build up a shortest path, not containing from; stopping when max_path_length is reached
	int next_dist_value = dist[from._iy][from._ix] - 1;
	NodeID next_node = from;
	while (next_dist_value >= min_dist_value)
	{
		NodeID current_node = next_node;
        const vector<NodeID> current_neighbors =
                (!through_walls) ? getNode(current_node).getNeighbors() : getNeighborNodesIgnoringWalls(current_node);
		
		vector<NodeID>::const_iterator it = current_neighbors.begin();
		NodeID best_neighbor = *it;
		for (; it != current_neighbors.end(); ++it)
		{
			if (dist[it->_iy][it->_ix] < dist[best_neighbor._iy][best_neighbor._ix])
				best_neighbor = *it;
		}
	    path->push_back(best_neighbor);
		next_node = best_neighbor;
		--next_dist_value;
	}
}

/*!
 * Computes a (unoccupied) nearest_neighbor_node for target_node.
 *
 * \param target_node A board graph node.
 * \param nearest_neighbor_node Set to one of the nearest neighbor nodes of node.
 * \return True, iff a nearest neighbor node could be found.
 */
bool BoardGraph::getNearestNeighborNode(const NodeID& target_node_id, NodeID* nearest_neighbor_node)
{
    Playground* playground = HeroQuestLevelWindow::_hero_quest->getPlayground();
    QuestBoard* quest_board = playground->getQuestBoard();

	Node& target_node = getNode(target_node_id);
	const vector<NodeID>& neighbors = target_node.getNeighbors();
	for (vector<NodeID>::const_iterator it = neighbors.begin(); it != neighbors.end(); ++it)
	{
        if (!playground->isFieldOccupied(*it))
		{
			*nearest_neighbor_node = *it;
			return true;
		}
	}

	return false;
}

/*!
 * Computes the set of nodes reachable (and visible) from node_id_from by moving up to movement_points fields along x or y
 * and saves it in *reachable_area.
 *
 * @param node_id_from start node
 * @param movement_points
 * @param pass_monsters if true, monsters don't block the way
 * @param through_walls if true, walls don't block the way
 * @param reachable_area resulting set of reachable nodes
 */
void BoardGraph::getReachableArea(const NodeID& node_id_from, uint movement_points, bool pass_monsters,
        bool through_walls, std::set<NodeID>* reachable_area)
{
    Playground* playground = HeroQuestLevelWindow::_hero_quest->getPlayground();
    QuestBoard* quest_board = playground->getQuestBoard();

    StorageMatrix<uint> points_left(_nodes.getWidth(), _nodes.getHeight(), 0);
    points_left[node_id_from._iy][node_id_from._ix] = movement_points + 1; // points_left > 0 means "reachable", points_left == 0 means "not reachable"

    // Dijkstra principle; starting at node_id_from
    list<NodeID> work_list;
    work_list.push_back(node_id_from);
    while (!work_list.empty())
    {
        NodeID current_node(work_list.front());
        work_list.pop_front();
        int current_node_points_left = points_left[current_node._iy][current_node._ix]; // current_node_points_left > 1 by definition

        const vector<NodeID> neighbors =
                (!through_walls) ? getNode(current_node).getNeighbors() : getNeighborNodesIgnoringWalls(current_node); // (non-walkable decoration is already excluded here)
        for (vector<NodeID>::const_iterator it = neighbors.begin(); it != neighbors.end(); ++it)
        {
            const NodeID& current_neighbor = *it;

            // ignore invisible neighbors
            if (!through_walls && !playground->getNodeVisibility(current_neighbor))
                continue;

            // ignore neighbors which are occupied by monsters
            if (!pass_monsters && playground->getMonster(current_neighbor) != 0)
                continue;

            uint current_neighbor_points_left = points_left[current_neighbor._iy][current_neighbor._ix];

            // update points_left of neighbor, and add neighbor to working list if some movement points are still left
            uint new_points_left_neighbor = current_node_points_left - 1;
            if (new_points_left_neighbor > current_neighbor_points_left)
            {
                points_left[current_neighbor._iy][current_neighbor._ix] = new_points_left_neighbor;
                reachable_area->insert(current_neighbor);
                if (new_points_left_neighbor > 1)
                {
                    work_list.push_back(current_neighbor);
                }
            }
        }
    }
}

/*!
 * @param node_id
 * @return vector of NodeIDs which are walkable neighbors of node_id, ignoring walls
 */
const vector<NodeID> BoardGraph::getNeighborNodesIgnoringWalls(const NodeID& node_id) const
{
    vector<NodeID> neighbor_nodes;

    int width = _nodes.getWidth();
    int height = _nodes.getHeight();

    NodeID left_neighbor(node_id._ix - 1, node_id._iy);
    if (node_id._ix > 0 && HeroQuestLevelWindow::_hero_quest->getPlayground()->isWalkable(left_neighbor))
        neighbor_nodes.push_back(left_neighbor);

    NodeID right_neighbor(node_id._ix + 1, node_id._iy);
    if (node_id._ix < width - 1 && HeroQuestLevelWindow::_hero_quest->getPlayground()->isWalkable(right_neighbor))
        neighbor_nodes.push_back(right_neighbor);

    NodeID upper_neighbor(node_id._ix, node_id._iy - 1);
    if (node_id._iy > 0 && HeroQuestLevelWindow::_hero_quest->getPlayground()->isWalkable(upper_neighbor))
        neighbor_nodes.push_back(upper_neighbor);

    NodeID lower_neighbor(node_id._ix, node_id._iy + 1);
    if (node_id._iy < height - 1 && HeroQuestLevelWindow::_hero_quest->getPlayground()->isWalkable(lower_neighbor))
        neighbor_nodes.push_back(lower_neighbor);

    return neighbor_nodes;
}

bool BoardGraph::contains(const NodeID& node_id) const
{
	return node_id._ix >= 0 &&  node_id._ix < _nodes.getWidth() &&
		node_id._iy >= 0 &&  node_id._iy < _nodes.getHeight();
}

/*!
 * Returns true if there is a wall between node1 and node2 which is not invisible due to an open door.
 */
bool BoardGraph::isWall(const NodeID& node1, const NodeID& node2) const
{
	pair<NodeID, NodeID> potential_wall1(node1, node2);
	pair<NodeID, NodeID> potential_wall2(node2, node1);

	// no wall at all?
	if (_walls.find(potential_wall1) == _walls.end() && _walls.find(potential_wall2) == _walls.end())
		return false;

	// wall found; open door?
	const Door* door = HeroQuestLevelWindow::_hero_quest->getLevel()->getDoor(make_pair(node1, node2));
	if (door == 0)
		return true;
	
	return !door->getOpen();
}

bool BoardGraph::save(ostream& stream) const
{
    DV(("save: nodes height x width: %d x %d", _nodes.getHeight(), _nodes.getWidth()));
    // _nodes
    for (int y = 0; y < _nodes.getHeight(); ++y)
        for (int x = 0; x < _nodes.getWidth(); ++x)
        {
            _nodes[y][x].save(stream);
        }

    return !stream.fail();
}

bool BoardGraph::load(istream& stream)
{
    DV(("load: nodes height x width: %d x %d", _nodes.getHeight(), _nodes.getWidth()));
    // _nodes
    for (int y = 0; y < _nodes.getHeight(); ++y)
        for (int x = 0; x < _nodes.getWidth(); ++x)
        {
            _nodes[y][x].load(stream);
        }

    return !stream.fail();
}
