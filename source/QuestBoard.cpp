#include "QuestBoard.h"

#include <iostream>

#include <QtGui/QPainter>
#include <QtGui/QBrush>
#include <QtGui/QMouseEvent>

#include "Math.h"
#include "QuestBoardGraph.h"
#include "HeroQuestLevelWindow.h"
#include "Playground.h"
#include "Hero.h"
#include "BoardGraph.h"
#include "StreamUtils.h"
#include "Debug.h"
#include "Level.h"
#include "PainterContext.h"
#include "ParameterStorage.h"

using namespace std;

const QString QuestBoard::QUEST_BOARD_FILENAME = ":/graphics/quest_board.jpg";
const int QuestBoard::FIELDS_X = 26;
const int QuestBoard::FIELDS_Y = 19;

QuestBoard::QuestBoard()
	:
	_orig_board_image(0),
	_scaled_board_image(),
	_field_pos(FIELDS_X, FIELDS_Y),
	_orig_field_width(0), // computed in ::create
	_orig_field_height(0), // computed in ::create
	_board_graph(0), // computed in ::create
	_room_ids(), // set in ::create
	_movement_path(),
	_reachable_area(),
	_attack_destination_valid(false),
	_attack_destination(0, 0),
	_destination_door_valid(false),
	_destination_door(0)
{
    DV(("Created new QuestBoard 0x%x", (unsigned int)this));
}

QuestBoard::~QuestBoard()
{
	delete _orig_board_image;
    DV(("Deleted QuestBoard 0x%x", (unsigned int)this));
}

bool QuestBoard::create()
{
	// load board image
    if (_orig_board_image == 0)
    {
        _orig_board_image = new QPixmap(QUEST_BOARD_FILENAME);
        if (_orig_board_image == 0)
            return false;
    }

    Playground* playground = HeroQuestLevelWindow::_hero_quest->getPlayground();

    // save scaled version
    DV(
            ("scaling board image from %d x %d to %d x %d", _orig_board_image->width(), _orig_board_image->height(), playground->width(), playground->height()));
    _scaled_board_image = _orig_board_image->scaled(playground->width(), playground->height()/*, Qt::KeepAspectRatio*/);

	// save positions of fields (positions have been extracted manually)
	const int left_part_orig_field_offset_x = 22;
	const int left_part_orig_field_width = 74;
	const int right_part_orig_field_offset_x = 13;
	const int right_part_orig_field_width = 75;
	const int orig_field_offset_y = 18;
	const int orig_field_height = 74;
	for (int col = 0; col < FIELDS_X; ++col)
	{
		for (int row = 0; row < FIELDS_Y; ++row)
		{
			// x
			if (col <= 12)
				_field_pos[row][col].x = left_part_orig_field_offset_x + col * left_part_orig_field_width; // left part
			if (col > 12)
				_field_pos[row][col].x = right_part_orig_field_offset_x + col * right_part_orig_field_width; // right part

			// x fine adjustment
			if (col == 1)
				_field_pos[row][col].x -= 4;
			if (col == 2)
				_field_pos[row][col].x -= 4;
			if (col == 10)
				_field_pos[row][col].x += 4;
			if (col == 11)
				_field_pos[row][col].x += 2;
			if (col == 12)
				_field_pos[row][col].x += 2;
			if (col == 13)
				_field_pos[row][col].x += 4;
			if (col == 14)
				_field_pos[row][col].x += 2;
			if (col == 25)
				_field_pos[row][col].x -= 2;

			// y
			_field_pos[row][col].y = orig_field_offset_y + row * orig_field_height;

			// y fine adjustment
			if (row == 0)
				_field_pos[row][col].y -= 3;
			if (row == 1)
				_field_pos[row][col].y -= 2;
			if (row == 2)
				_field_pos[row][col].y -= 1;
			if (row == 18)
				_field_pos[row][col].y += 2;
		}
	}

    _orig_field_width = min(left_part_orig_field_width, right_part_orig_field_width);
    _orig_field_height = orig_field_height;

	// create graph of the standard board
	_board_graph = new QuestBoardGraph(this);

	assignRoomIDs();
	
	return true;
}

void QuestBoard::redraw(QPainter& painter)
{
	// the quest board background image
    painter.drawPixmap(QPoint(0, 0), _scaled_board_image);
    DV(("_scaled_board_image: %d x %d", _scaled_board_image.width(), _scaled_board_image.height()));

#if 0
	// debug: draw horizontal lines to represent the walls
	for (int iy = 0; iy < FIELDS_Y - 1; ++iy)
	{
		for (int ix = 0; ix < FIELDS_X; ++ix)
		{
			Node& node = _board_graph->getNode(NodeID(ix, iy));
			if (!node.isNeighbor(NodeID(ix, iy + 1)))
			{
				Vec2i p1_screen = getFieldCorner2(node.getNodeID());
				Vec2i p2_screen = getFieldCorner3(node.getNodeID());
				painter.drawLine(p1_screen.x, p1_screen.y, p2_screen.x, p2_screen.y);
			}
		}
	}

	// debug: draw vertical lines to represent the walls
	for (int iy = 0; iy < FIELDS_Y; ++iy)
	{
		for (int ix = 0; ix < FIELDS_X - 1; ++ix)
		{
			Node& node = _board_graph->getNode(NodeID(ix, iy));
			if (!node.isNeighbor(NodeID(ix + 1, iy)))
			{
				Vec2i p1_screen = getFieldCorner1(node.getNodeID());
				Vec2i p2_screen = getFieldCorner3(node.getNodeID());
				painter.drawLine(p1_screen.x, p1_screen.y, p2_screen.x, p2_screen.y);
			}
		}
	}

	/*cout << "x[0] = " << _field_pos[0][0] << endl;
	cout << "x[1] = " << _field_pos[0][1] << endl;*/
	// debug: vertical lines
	for (int x = 0; x < FIELDS_X; ++x)
	{
		Vec2i p1 = _field_pos[0][x];
		Vec2i p2 = p1;
		p2.y = _field_pos[FIELDS_Y-1][x].y;
		
		Vec2i p1_screen = origToScreen(p1);
		Vec2i p2_screen = origToScreen(p2);

		/*if (x == 0 || x == 1)
		{
			cout << "p1_screen = " << p1_screen << endl;
			cout << "p2_screen = " << p2_screen << endl;
		}*/

		painter.drawLine(p1_screen.x, p1_screen.y, p2_screen.x, p2_screen.y);
	}

	// debug: horizontal lines
	for (int y = 0; y < FIELDS_Y; ++y)
	{
		Vec2i p1 = _field_pos[y][0];
		Vec2i p2 = p1;
		p2.x = _field_pos[y][FIELDS_X-1].x;

		Vec2i p1_screen = origToScreen(p1);
		Vec2i p2_screen = origToScreen(p2);

		painter.drawLine(p1_screen.x, p1_screen.y, p2_screen.x, p2_screen.y);
	}
#endif

	// draw the movement path
	// -> moved to playground, because has to be done after redrawing creatures!

	// draw the attack destination
	// -> moved to playground, because has to be done after redrawing creatures!

    // draw the reachable area
    {
        PainterContext painter_context(painter);
        painter.setBrush(QBrush(QColor(0, 255, 0, 50), Qt::SolidPattern));
        painter.setPen(Qt::NoPen);

        for (set<NodeID>::const_iterator it_reachable_nodes = _reachable_area.begin();
                it_reachable_nodes != _reachable_area.end(); ++it_reachable_nodes)
        {
            const NodeID& reachable_node = *it_reachable_nodes;
            Vec2i corner0 = getFieldCorner0(reachable_node);
            Vec2i corner1 = getFieldCorner1(reachable_node);
            Vec2i corner2 = getFieldCorner2(reachable_node);
            painter.drawRect(corner0.x, corner0.y, corner1.x - corner0.x, corner2.y - corner0.y);
        }
    }
}

/*!
 * Transforms original coordinates (according to the resolution of the board image) to screen coordinates.
 */
Vec2i QuestBoard::origToScreen(const Vec2i& coord_orig) const
{
    HG_ASSERT((_orig_board_image->width() > 1), "_orig_board_image->width() is <= 1");
    HG_ASSERT((_orig_board_image->height() > 1), "_orig_board_image->height() is <= 1");
	float orig_to_screen_factor = _scaled_board_image.width() / float(_orig_board_image->width());
	return Vec2i(
		int(round(coord_orig.x * orig_to_screen_factor)),
		int(round(coord_orig.y * orig_to_screen_factor)));
}

Vec2i QuestBoard::getFieldCenter(const NodeID& node_id) const
{
#if 0
	cout << "corner0: " << getFieldCorner0(node_id) << "; corner3: " << getFieldCorner3(node_id) << endl;
	cout << "center: " << ((
		getFieldCorner0(node_id) +
		getFieldCorner1(node_id) +
		getFieldCorner2(node_id) +
		getFieldCorner3(node_id)) / 4) << endl;
#endif
	return (
		getFieldCorner0(node_id) +
		getFieldCorner1(node_id) +
		getFieldCorner2(node_id) +
		getFieldCorner3(node_id)) / 4;
}

/*
 * \return The pixel (in screen coords) of the top left corner of the node with ID node_id.
 *
 *      0------1
 *   	|      |
 *		|      |
 *		2------3
 */
Vec2i QuestBoard::getFieldCorner0(const NodeID& node_id) const
{
	if (node_id._ix < FIELDS_X && node_id._iy < FIELDS_Y)
	{
		return origToScreen(_field_pos[node_id._iy][node_id._ix]);
	}

	// unsupported position
	if (node_id._ix > FIELDS_X || node_id._iy > FIELDS_Y)
	{
		return Vec2i(0, 0);
	}

	// extrapolate right edge of the quest board
	if (node_id._ix == FIELDS_X && node_id._iy < FIELDS_Y)
	{
		Vec2i p_left(getFieldCorner0(NodeID(node_id._ix - 1, node_id._iy)));
		Vec2i p_left_left(getFieldCorner0(NodeID(node_id._ix - 2, node_id._iy)));
		Vec2i p(p_left + (p_left - p_left_left));
		return p;
	}

	// extrapolate bottom edge of the quest board
	if (node_id._ix < FIELDS_X && node_id._iy == FIELDS_Y)
	{
		Vec2i p_upper(getFieldCorner0(NodeID(node_id._ix, node_id._iy - 1)));
		Vec2i p_upper_upper(getFieldCorner0(NodeID(node_id._ix, node_id._iy - 2)));
		Vec2i p(p_upper + (p_upper - p_upper_upper));
		return p;
	}

	// extrapolate bottom right point of the quest board
	Vec2i p_leftupper(getFieldCorner0(NodeID(node_id._ix - 1, node_id._iy - 1)));
	Vec2i p_leftupper_leftupper(getFieldCorner0(NodeID(node_id._ix - 2, node_id._iy - 2)));
	Vec2i p(p_leftupper + (p_leftupper - p_leftupper_leftupper));
	return p;
}

Vec2i QuestBoard::getFieldCorner1(const NodeID& node_id) const
{
	return getFieldCorner0(NodeID(node_id._ix + 1, node_id._iy));
}

Vec2i QuestBoard::getFieldCorner2(const NodeID& node_id) const
{
	return getFieldCorner0(NodeID(node_id._ix, node_id._iy + 1));
}

Vec2i QuestBoard::getFieldCorner3(const NodeID& node_id) const
{
	return getFieldCorner0(NodeID(node_id._ix + 1, node_id._iy + 1));
}

Vec2i QuestBoard::getFieldCorner(const NodeID& node_id, unsigned int corner_num) const
{
	switch (corner_num)
	{
	case 0:
		return getFieldCorner0(node_id);
	case 1:
		return getFieldCorner1(node_id);
	case 2:
		return getFieldCorner2(node_id);
	case 3:
		return getFieldCorner3(node_id);
	default:
		cout << "Internal program error: QuestBoard::getFieldCorner: invalid corner_num " << corner_num << endl;
		break;
	}

	return Vec2i(INT_MAX, INT_MAX);
}

/*!
 * This is used for viewing field computation.
 *
 * Inserts all border lines (in screen pixel coordinates) of node_id into node_borders.
 *
 * If node_id is occupied by a creature, each border of node_id is inserted into node_borders.
 *
 * Furthermore, a node border exists between two direct geometrically neighboring nodes
 * if these nodes are no direct topological neighbors in the _board_graph (e. g., due to walls).
 * 
 * If use_ignore_field is true, then ignore_field adds only borders due to walls.
 */
void QuestBoard::getBorderLinesOfNode(const NodeID& node_id, vector<Line>* node_borders, bool use_ignore_field, const NodeID* ignore_field) const
{
	const Node& node = _board_graph->getNode(node_id);

	NodeID geometric_neighbors[] = {
		NodeID(node_id._ix, node_id._iy - 1),
		NodeID(node_id._ix - 1, node_id._iy),
		NodeID(node_id._ix + 1, node_id._iy),
		NodeID(node_id._ix, node_id._iy + 1)
	};

	// field corners have indices:
	//
	//    0---------1
	//    |         |
	//    | node_id |
	//    |         |
	//    2---------3
	//
	pair<int, int> border_point_indices[] = {
		make_pair(0, 1), // top edge
		make_pair(0, 2), // left edge
		make_pair(1, 3), // right edge
		make_pair(2, 3)  // bottom edge
	};

    Playground* playground = HeroQuestLevelWindow::_hero_quest->getPlayground();

	// 1) is node_id occupied by a creature?
    if (playground->getCreature(node_id) != 0)
	{
		// insert a border line for each geometric neighbor of node_id;
		// if use_ignore_field is true and node_id == ignore_field, insert only lines due to walls
		for (unsigned int i_geom = 0; i_geom < 4; ++i_geom)
		{
			const NodeID& geometric_neighbor = geometric_neighbors[i_geom];

			// insert border line
			if (use_ignore_field && node_id == *ignore_field)
			{
				if (_board_graph->isWall(node_id, geometric_neighbor))
				{
					// insert border line
					node_borders->push_back(Line(
						getFieldCorner(node_id, border_point_indices[i_geom].first),
						getFieldCorner(node_id, border_point_indices[i_geom].second)));
				}
			}
			else
			{
				node_borders->push_back(Line(
					getFieldCorner(node_id, border_point_indices[i_geom].first),
					getFieldCorner(node_id, border_point_indices[i_geom].second)));
			}
		}

		return; // 4 borders added already; there cannot be more
	}

	// 2) if use_ignore_field is true, and node_id == ignore_field, then only insert lines due to walls
	if (use_ignore_field && node_id == *ignore_field)
	{
		// for each wall between node_id and a geometric neighbor, insert a node border
		for (unsigned int i_geom = 0; i_geom < 4; ++i_geom)
		{
			const NodeID& geometric_neighbor = geometric_neighbors[i_geom];

			// wall?
			if (_board_graph->isWall(node_id, geometric_neighbor))
			{
				// insert border line
				node_borders->push_back(Line(
					getFieldCorner(node_id, border_point_indices[i_geom].first),
					getFieldCorner(node_id, border_point_indices[i_geom].second)));
			}
		}
		return;
	}

	// 3) geometric vs. topologic neighbors
	const vector<NodeID>& topologic_neighbors = node.getNeighbors();

	// for each geometric neighbor which is not a topologic neighbor, insert a node border
	for (unsigned int i_geom = 0; i_geom < 4; ++i_geom)
	{
		const NodeID& geometric_neighbor = geometric_neighbors[i_geom];

		// is geometric_neighbor a topologic neighbor?
		bool is_neighbor = false;
		for (unsigned int i_top = 0; i_top < topologic_neighbors.size(); ++i_top)
		{
			if (geometric_neighbor == topologic_neighbors[i_top])
			{
				is_neighbor = true;
				break;
			}
		}
		if (!is_neighbor)
		{
			// insert border line; exception: if use_ignore_field is true and geometric_neighbor == ignore_field, then only insert lines due to walls
			if (use_ignore_field && geometric_neighbor == *ignore_field)
			{
				if (_board_graph->isWall(node_id, geometric_neighbor))
				{
					node_borders->push_back(Line(
						getFieldCorner(node_id, border_point_indices[i_geom].first),
						getFieldCorner(node_id, border_point_indices[i_geom].second)));
				}
			}
			else
			{
				node_borders->push_back(Line(
					getFieldCorner(node_id, border_point_indices[i_geom].first),
					getFieldCorner(node_id, border_point_indices[i_geom].second)));
			}
		}
	}
}
#if 0
/*!
 * Clears viewed_fields, then computes the fields which can be viewed from field and stores them in viewed_fields.
 */
void QuestBoard::computeViewedFields(const NodeID& field, vector<NodeID>* viewed_fields) const
{
	viewed_fields->clear();

	viewed_fields->push_back(field); // field can be viewed from itself

	for (int y = 0; y < QuestBoard::FIELDS_Y; ++y)
	{
		for (int x = 0; x < QuestBoard::FIELDS_X; ++x)
		{
			const NodeID other_field(x, y);

			if (other_field == field)
				continue;

			if (fieldCanBeViewedFromField(field, other_field, true))
				viewed_fields->push_back(other_field);
		}
	}
}
#endif
/*!
 * A field can be viewed from another field, if both are in the same room, or no obstacle
 * (walls, creatures) are in between the direct line between the two fields.
 *
 * \return True, if field2 can be viewed from field1; false otherwise.
 */
bool QuestBoard::fieldCanBeViewedFromField(const NodeID& field1, const NodeID& field2, bool respect_field2_borders) const
{
	// a field can always see itself
	if (field1 == field2)
		return true;

	// if field1 and field2 are in the same room, then they can automatically be viewed from each other
	unsigned int room_id1 = getRoomID(field1);
	unsigned int room_id2 = getRoomID(field2);
	if ((room_id1 != 0) &&      // do not consider hallway fields during this special treatment
		(room_id1 == room_id2)) // same room
		return true;

	// strategy: create line from field1 to field2, then iterate over all fields inside the bounding box of line
	// (TODO: this can be optimized by taking only the fields into account which are covered by the line),
	// compute each field's border lines, and intersect line with each border line.
	// If they intersect, return false.
	// use window pixel coordinates
	Line line(getFieldCenter(field1), getFieldCenter(field2));

	// bounding box of line
	const int y_min = min(field1._iy, field2._iy);
	const int y_max = max(field1._iy, field2._iy);
	const int x_min = min(field1._ix, field2._ix);
	const int x_max = max(field1._ix, field2._ix);

	vector<Line> node_borders; // moved outside the loop for performance reasons
	node_borders.reserve(4);
	for (int y = y_min; y <= y_max; ++y)
	{
		for (int x = x_min; x <= x_max; ++x)
		{
			NodeID field(x, y);

			// field1 must be omitted
			if (field == field1)
				continue;

			node_borders.clear();

			// omit field2, if respect_field2_borders is false
			getBorderLinesOfNode(field, &node_borders, !respect_field2_borders, &field2);

			for (unsigned int i = 0; i < node_borders.size(); ++i)
			{
				const Line& border_line = node_borders[i];
				
				Vec2d s_dummy(0, 0);
				if (intersect(line, border_line, &s_dummy))
					return false;
			}
		}
	}

	return true;
}

bool QuestBoard::nodesAreInSameRowOrColumn(const NodeID& field1, const NodeID& field2) const
{
    return field1._ix == field2._ix || field1._iy == field2._iy;
}

/*!
 * Each field is contained either in a hallway (room ID 0) or in a room (room IDs > 0).
 * Fields which are located in the same room have the same room ID.
 * 
 * \return The room ID of field.
 */
unsigned int QuestBoard::getRoomID(const NodeID& field) const
{	
	map<NodeID, unsigned int>::const_iterator it = _room_ids.find(field);
	if (it == _room_ids.end())
		cout << "Internal program error: QuestBoard::getRoomID(" << field << ")" << endl;
	return it->second;
}

/*!
 * Arrange the rooms as on the original quest board.
 */
void QuestBoard::assignRoomIDs()
{
	// Geometric distribution of the room ID on the QuestBoard:
	//
	//   1  2               7   8
	//         3         6
	//   4  5               9  10
	//
	//              11
	//
	//  12  13 14           19  20
	//                   18
	//  15  16    17        21  22
	
	// set all to 0
	for (unsigned int y = 0; y < FIELDS_Y; ++y)
		for (unsigned int x = 0; x < FIELDS_X; ++x)
			_room_ids[NodeID(x, y)] = 0;
	
	// room 1
	unsigned int room_id = 1;
	assignRoomIDs(NodeID(1, 1), 4, 3, room_id);

	// room 2
	++room_id;
	assignRoomIDs(NodeID(5, 1), 4, 3, room_id);

	// room 3
	++room_id;
	assignRoomIDs(NodeID(9, 1), 3, 5, room_id);
	
	// room 4
	++room_id;
	assignRoomIDs(NodeID(1, 4), 4, 5, room_id);

	// room 5
	++room_id;
	assignRoomIDs(NodeID(5, 4), 4, 5, room_id);



	// room 6
	++room_id;
	assignRoomIDs(NodeID(14, 1), 3, 5, room_id);

	// room 7
	++room_id;
	assignRoomIDs(NodeID(17, 1), 4, 4, room_id);

	// room 8
	++room_id;
	assignRoomIDs(NodeID(21, 1), 4, 4, room_id);

	// room 9
	++room_id;
	assignRoomIDs(NodeID(17, 5), 4, 4, room_id);

	// room 10
	++room_id;
	assignRoomIDs(NodeID(21, 5), 4, 4, room_id);



	// room 11
	++room_id;
	assignRoomIDs(NodeID(10, 7), 6, 5, room_id);



	// room 12
	++room_id;
	assignRoomIDs(NodeID(1, 10), 4, 4, room_id);

	// room 13
	++room_id;
	assignRoomIDs(NodeID(5, 10), 2, 3, room_id);

	// room 14
	++room_id;
	assignRoomIDs(NodeID(7, 10), 2, 3, room_id);
	
	// room 15
	++room_id;
	assignRoomIDs(NodeID(1, 14), 4, 4, room_id);

	// room 16
	++room_id;
	assignRoomIDs(NodeID(5, 13), 4, 5, room_id);

	// room 17
	++room_id;
	assignRoomIDs(NodeID(9, 13), 3, 5, room_id);



	// room 19
	++room_id;
	assignRoomIDs(NodeID(17, 10), 4, 4, room_id); // node_id (17, 13) will be overwritten by room 18

	// room 18
	++room_id;
	assignRoomIDs(NodeID(14, 13), 4, 5, room_id); // (overwrites node_id (17, 13) of room 19)

	// room 20
	++room_id;
	assignRoomIDs(NodeID(21, 10), 4, 4, room_id);

	// room 21
	++room_id;
	assignRoomIDs(NodeID(18, 14), 3, 4, room_id);

	// room 22
	++room_id;
	assignRoomIDs(NodeID(21, 14), 4, 4, room_id);


	// establish the reverse mapping
	for (map<NodeID, unsigned int>::const_iterator it = _room_ids.begin(); it != _room_ids.end(); ++it)
	{
		_room_ids_reverse[it->second].push_back(it->first);
	}
}

/*!
 * Helper method for assignRoomIDs().
 */
void QuestBoard::assignRoomIDs(const NodeID& node_id_top_left, unsigned int width, unsigned int height, unsigned int id)
{
	for (unsigned int y = node_id_top_left._iy; y < node_id_top_left._iy + height; ++y)
		for (unsigned int x = node_id_top_left._ix; x < node_id_top_left._ix + width; ++x)
			_room_ids[NodeID(x, y)] = id;
}

const list<NodeID>& QuestBoard::getRoomFields(unsigned int room_id) const
{
	return _room_ids_reverse.find(room_id)->second;
}

/*!
 * \param screen_coords Absolute screen pixel coordinates TODO: aren't they now relative to the Playground widget?
 * \param node_id The ID of the node which contains screen_coords.
 * \param True, if a node containing screen_coords has been found.
 */
bool QuestBoard::getNodeID(const Vec2i& screen_coords, NodeID* node_id) const
{
	//cout << "screen_coords = " << screen_coords << endl;
	//cout << "getFieldCorner2(NodeID(0, _field_pos.getHeight() - 1)) = " << getFieldCorner2(NodeID(0, _field_pos.getHeight() - 1)) << endl;

	// out of field?
	if (screen_coords.x < getFieldCorner0(NodeID(0, 0)).x)
	{
		//cout << "screen_coords.x < getFieldCorner0(NodeID(0, 0)).x)" << endl;
		return false;
	}
	if (screen_coords.x > getFieldCorner1(NodeID(_field_pos.getWidth() - 1, 0)).x)
	{
		//cout << "screen_coords.x > getFieldCorner1(NodeID(_field_pos.getWidth() - 1, 0)).x" << endl;
		//cout << "getFieldCorner1(NodeID(_field_pos.getWidth() - 1, 0)).x = " << getFieldCorner1(NodeID(_field_pos.getWidth() - 1, 0)).x << endl;
		return false;
	}
	if (screen_coords.y < getFieldCorner0(NodeID(0, 0)).y)
	{
		//cout << "screen_coords.y < getFieldCorner0(NodeID(0, 0)).y" << endl;
		return false;
	}
	if (screen_coords.y > getFieldCorner2(NodeID(0, _field_pos.getHeight() - 1)).y)
	{
		//cout << "screen_coords.y (" << screen_coords.y << ") > getFieldCorner2(NodeID(0, _field_pos.getHeight() - 1)).y) ("
		//	<< getFieldCorner2(NodeID(0, _field_pos.getHeight() - 1)) << ")" << endl;
		return false;
	}

	// find the x part of the node id
	int node_id_x = 0;
	for (int i = 0; i < _field_pos.getWidth(); ++i)
	{
		Vec2i corner0(getFieldCorner0(NodeID(i, 0)));
		Vec2i corner1(getFieldCorner1(NodeID(i, 0)));
		if (screen_coords.x >= corner0.x && screen_coords.x <= corner1.x)
		{
			node_id_x = i;
			break;
		}
	}

	// find the y part of the node id
	int node_id_y = 0;
	for (int i = 0; i < _field_pos.getHeight(); ++i)
	{
		Vec2i corner0(getFieldCorner0(NodeID(0, i)));
		Vec2i corner2(getFieldCorner2(NodeID(0, i)));
		if (screen_coords.y >= corner0.y && screen_coords.y <= corner2.y)
		{
			node_id_y = i;
			break;
		}
	}

	*node_id = NodeID(node_id_x, node_id_y);
	return true;
}

/*!
 * Adds all NodeIDs to geometric_neighbors, which are geometrically incident to node_id,
 * possibly separated by a wall. Note that this is *not* identical to the topological neighbors.
 */
void QuestBoard::getGeometricNeighbors(const NodeID& node_id, vector<NodeID>* geometric_neighbors) const
{
	// top neighbor
	if (node_id._iy > 0)
		geometric_neighbors->push_back(NodeID(node_id._ix, node_id._iy - 1));
	// left neighbor
	if (node_id._ix > 0)
		geometric_neighbors->push_back(NodeID(node_id._ix - 1, node_id._iy));
	// right neighbor
	if (node_id._ix < FIELDS_X - 1)
		geometric_neighbors->push_back(NodeID(node_id._ix + 1, node_id._iy));
	// bottom neighbor
	if (node_id._iy < FIELDS_Y - 1)
		geometric_neighbors->push_back(NodeID(node_id._ix, node_id._iy + 1));
}

/*!
 * Identifies a wall between field 1 and field 2 if the area marked by "x" is represented by screen_coords.
 * The resulting wall is returned, encoded as pair (field 1, field 2).
 *
 *    -------------------- --------------------
 *   |                  /x|x\                  |
 *   |  hero          /x x|x x\                |
 *   |  pos         /x x x|x x x\              |
 *   |            /x x x x|x x x x\            |
 *   |            \x x x x|x x x x/            |
 *   |              \x x x|x x x/              |
 *   |  field 1       \x x|x x/      field 2   |
 *   |                  \x|x/                  |
 *    -------------------- --------------------
 *
 * \param screen_coords Absolute screen pixel coordinates
 * \param wall_id The IDs of the two nodes connected by the selected edge.
 * \param True, if an appropriate edge has been found.
 */
bool QuestBoard::getTransitionByNearestNeighbours(const Vec2i& screen_coords, pair<NodeID, NodeID>* wall_id) const
{
	// 1) clicked_node = find the node which has been clicked
	NodeID clicked_node(0, 0);
	if (!getNodeID(screen_coords, &clicked_node))
		return false;

	// 2) nearest_neighbor = find the nearest neighbor node
	vector<NodeID> neighbors;
	getGeometricNeighbors(clicked_node, &neighbors);
	if (neighbors.size() == 0)
		return false;
	NodeID nearest_neighbor(neighbors[0]);
	double nearest_dist2 = (screen_coords - getFieldCenter(neighbors[0])).length2();
	for (unsigned int i = 1; i < neighbors.size(); ++i)
	{
		const NodeID& neighbor = neighbors[i];
		double dist2 = (screen_coords - getFieldCenter(neighbor)).length2();
		
		if (dist2 < nearest_dist2)
		{
			nearest_neighbor = neighbor;
			nearest_dist2 = dist2;
		}
	}
	
	// 3) (clicked_node_id, nearest_neighbor) is the edge found
	*wall_id = make_pair(clicked_node, nearest_neighbor);

	return true;
}

/*!
 * Computes the shortest path from the currentHero's position to the field node_id,
 * and saves it in _movement_path.
 * If node_id has not yet been discovered (i.e. it is a "black" field),
 * _movement_path is cleared instead.
 *
 * @param node_id node id of the field selected by the mouse click
 */
void QuestBoard::computeMovementPath(const NodeID& node_id)
{
    Playground* playground = HeroQuestLevelWindow::_hero_quest->getPlayground();

	// do not show any path if node_id is already occupied by another creature
    if (playground->getCreature(node_id) != 0)
	{
		//cout << "Cannot move to node ID " << node_id << " because already occupied" << endl;
		_movement_path.clear();
		return;
	}

	// current hero's position
    Hero* current_hero = HeroQuestLevelWindow::_hero_quest->getLevel()->getCurrentlyActingHero();
	if (current_hero == 0)
	    return;
    const NodeID* pos_current_hero = playground->getCreaturePos(current_hero);
	if (pos_current_hero == 0)
	{
		cout << "Internal program error: QuestBoard::computeMovementPath" << endl;
		return;
	}
    Level::HeroActionStates* current_hero_action_states =
            HeroQuestLevelWindow::_hero_quest->getLevel()->getCurrentlyActingHeroActionStates();
    if (current_hero_action_states == 0)
        return;

	// compute the shortest path to the clicked field, obeying current obstacles
    uint movement_points =
            HeroQuestLevelWindow::_hero_quest->getLevel()->getCurrentlyActingHeroActionStates()->_move_points;
    _board_graph->getShortestPath(*pos_current_hero, node_id, true,
            current_hero_action_states->_may_cross_monsters_during_movement, current_hero->getCanMoveThroughWalls(),
            movement_points, &_movement_path);
	// if the path ends on an occupied field, remove the occupied fields from the movement path
	removeOccupiedFieldsFromEnd(&_movement_path);
}

/*!
 * Computes the set of nodes reachable from the position of the current hero by moving up to the current hero's
 * movement_points fields along x or y. The result is saved in _reachable_area.
 *
 * @param node_id_from start node
 * @param movement_points
 */
void QuestBoard::computeReachableArea()
{
    _reachable_area.clear();

    // current hero's position
    Hero* current_hero = HeroQuestLevelWindow::_hero_quest->getLevel()->getCurrentlyActingHero();
    if (current_hero == 0)
        return;
    const NodeID* pos_current_hero = HeroQuestLevelWindow::_hero_quest->getPlayground()->getCreaturePos(current_hero);
    if (pos_current_hero == 0)
    {
        cout << "Internal program error: QuestBoard::computeReachableArea" << endl;
        return;
    }
    Level::HeroActionStates* current_hero_action_states =
            HeroQuestLevelWindow::_hero_quest->getLevel()->getCurrentlyActingHeroActionStates();
    if (current_hero_action_states == 0)
        return;

    // compute the reachable area
    _board_graph->getReachableArea(*pos_current_hero, current_hero_action_states->_move_points,
            current_hero_action_states->_may_cross_monsters_during_movement, current_hero->getCanMoveThroughWalls(),
            &_reachable_area);
}

/*!
 * Clears _reachable_area.
 */
void QuestBoard::clearReachableArea()
{
    _reachable_area.clear();
}

void QuestBoard::setAttackDestination(int x, int y)
{
	_attack_destination_valid = false;

	// which field has been clicked?
	NodeID node_id(0, 0);
	bool valid_field = getNodeID(Vec2i(x, y), &node_id);
	if (valid_field)
	{
		//cout << "Clicked field with node ID " << node_id << endl;
	}
	else
	{
		//cout << "Clicked position out of field" << endl;
		return;
	}

    Playground* playground = HeroQuestLevelWindow::_hero_quest->getPlayground();

	Hero* current_hero = HeroQuestLevelWindow::_hero_quest->getLevel()->getCurrentlyActingHero();
	if (current_hero == 0)
	    return;

    const NodeID* pos_current_hero = playground->getCreaturePos(current_hero);
	if (pos_current_hero == 0)
	{
		cout << "Internal program error: QuestBoard::setAttackDestination" << endl;
		return;
	}
	if (!_board_graph->isEdge(node_id, *pos_current_hero))
    {
        // check for diagonally-adjacent attack
        if (!_board_graph->isCorner(node_id, *pos_current_hero))
            return;

        // fields are diagonally-adjacent; may the hero attack this way?
        if (current_hero->getNumAttackDiceDiagonallyAdjacent() == 0)
        {
            // no, he may not
            return;
        }

        // yes, he may
    }

	// is there another creature at node_id?
    Creature* destination_creature = playground->getCreature(node_id);
	if (destination_creature == 0)
		return;

	_attack_destination_valid = true;
	_attack_destination = node_id;
}

bool QuestBoard::isDestinationDoor(const Door& door) const
{
	return _destination_door_valid && (door == *_destination_door);
}

void QuestBoard::setDestinationDoor(int x, int y)
{
	_destination_door_valid = false;

    // position of current hero
    const NodeID* pos_current_hero = 0;
    {
        Playground* playground = HeroQuestLevelWindow::_hero_quest->getPlayground();
        Hero* current_hero = HeroQuestLevelWindow::_hero_quest->getLevel()->getCurrentlyActingHero();
        pos_current_hero = playground->getCreaturePos(current_hero);
        if (pos_current_hero == 0)
        {
            DVX(("Internal program error: QuestBoard::setDestinationDoor: pos_current_hero == 0 for (%d, %d)!", x, y));
            return;
        }
    }

    // A) Try to get door by nearest neighbour search. Example: hero stands on field 1, user
    //    clicks somewhere in area marked by "x". => Transition (field 1, field 2) will be found.
    //
    //   -------------------- --------------------
    //  |                  /x|x\                  |
    //  |  hero          /x x|x x\                |
    //  |  pos         /x x x|x x x\              |
    //  |            /x x x x|x x x x\            |
    //  |            \x x x x|x x x x/            |
    //  |              \x x x|x x x/              |
    //  |  field 1       \x x|x x/      field 2   |
    //  |                  \x|x/                  |
    //   -------------------- --------------------
    //
	pair<NodeID, NodeID> transition(NodeID(0, 0), NodeID(0, 0));
    bool valid_transition_by_nearest_neighbour = getTransitionByNearestNeighbours(Vec2i(x, y), &transition);
    if (!valid_transition_by_nearest_neighbour)
	{
		return;
	}

	// is there a door connecting the transition's nodes?
	const Door* door = HeroQuestLevelWindow::_hero_quest->getLevel()->getDoor(transition);
    if (door != 0)
    {
        // is door incident to the current hero's position?
        if (((*pos_current_hero) != door->getF1()) && ((*pos_current_hero) != door->getF2()))
        {
            // not incident => door not OK
            door = 0;
        }
    }
	
	if (door == 0)
    {
        // B) Try to get door by taking the field of the current position together with the clicked field.
        //
        //   -------------------- --------------------
        //  |                    |x x x x x x x x x x |
        //  |  hero              |x x x x x x x x x x |
        //  |  pos               |x x x x x x x x x x |
        //  |                    |x x x x x x x x x x |
        //  |                    |x x x x x x x x x x |
        //  |                    |x x x x x x x x x x |
        //  |  field 1           |x x x x x field 2 x |
        //  |                    |x x x x x x x x x x |
        //   -------------------- --------------------
        //

        // determine clicked field (field 2)
        NodeID clicked_node(0, 0);
        if (!getNodeID(Vec2i(x, y), &clicked_node))
            return;

        // is there a door between field 1 and field 2?
        door = HeroQuestLevelWindow::_hero_quest->getLevel()->getDoor(make_pair(*pos_current_hero, clicked_node));
    }

    if (door == 0)
        return;

	_destination_door_valid = true;
	_destination_door = door;
}

bool QuestBoard::contains(const NodeID& node_id) const
{
    if (_board_graph == 0)
        return false;

    return _board_graph->contains(node_id);
}

bool QuestBoard::save(std::ostream& stream) const
{
    // _board_graph
    _board_graph->save(stream);

    // _room_ids
    StreamUtils::writeUInt(stream, _room_ids.size());
    for (map<NodeID, uint>::const_iterator it = _room_ids.begin(); it != _room_ids.end(); ++it)
    {
        it->first.save(stream);
        StreamUtils::writeUInt(stream, it->second);
    }

    // _room_ids_reverse
    StreamUtils::writeUInt(stream, _room_ids_reverse.size());
    for (map<uint, list<NodeID> >::const_iterator it = _room_ids_reverse.begin(); it != _room_ids_reverse.end(); ++it)
    {
        StreamUtils::writeUInt(stream, it->first);
        StreamUtils::write(stream, it->second);
    }

    // _movement_path
    StreamUtils::writeUInt(stream, _movement_path.size());
    for (vector<NodeID>::const_iterator it = _movement_path.begin(); it != _movement_path.end(); ++it)
    {
        it->save(stream);
    }

    // _reachable_area
    StreamUtils::writeUInt(stream, _reachable_area.size());
    for (set<NodeID>::const_iterator it = _reachable_area.begin(); it != _reachable_area.end(); ++it)
    {
        it->save(stream);
    }

    // attacking
    StreamUtils::writeBool(stream, _attack_destination_valid);
    _attack_destination.save(stream);

    // opening doors: irrelevant for save/load
    //StreamUtils::writeBool(stream, _destination_door_valid);
    //StreamUtils::writeUInt(stream, _destination_door->getReferencingID());

    return !stream.fail();
}

bool QuestBoard::load(std::istream& stream)
{
    // _orig_board_image
    // _scaled_board_image
    // _field_pos
    // _orig_field_width
    // _orig_field_height
    create();

    // _board_graph
    _board_graph->load(stream);

    // _room_ids
    uint num_room_ids;
    StreamUtils::readUInt(stream, &num_room_ids);
    for (uint i = 0; i < num_room_ids; ++i)
    {
        NodeID node_id;
        node_id.load(stream);

        uint num;
        StreamUtils::readUInt(stream, &num);

        _room_ids[node_id] = num;
    }

    // _room_ids_reverse
    uint num_room_ids_reverse;
    StreamUtils::readUInt(stream, &num_room_ids_reverse);
    for (uint i = 0; i < num_room_ids_reverse; ++i)
    {
        uint num;
        StreamUtils::readUInt(stream, &num);

        list<NodeID> node_ids;
        StreamUtils::read(stream, &node_ids);

        _room_ids_reverse[num] = node_ids;
    }

    uint num_movement_path;
    StreamUtils::readUInt(stream, &num_movement_path);
    for (uint i = 0; i < num_movement_path; ++i)
    {
        NodeID node_id;
        node_id.load(stream);

        _movement_path.push_back(node_id);
    }

    uint num_reachable_area;
    StreamUtils::readUInt(stream, &num_reachable_area);
    for (uint i = 0; i < num_reachable_area; ++i)
    {
        NodeID node_id;
        node_id.load(stream);

        _reachable_area.insert(node_id);
    }

    // attacking
    StreamUtils::readBool(stream, &_attack_destination_valid);
    _attack_destination.load(stream);

    // irrelevant
    _destination_door_valid = false;
    _destination_door = 0;

    return !stream.fail();
}

void QuestBoard::removeOccupiedFieldsFromEnd(vector<NodeID>* path) const
{
	if (path->empty())
		return;

    Playground* playground = HeroQuestLevelWindow::_hero_quest->getPlayground();

    while (playground->isFieldOccupiedByCreature(path->back()))
	{
		//cout << "removing field " << path->back() << " from path..." << endl;
		path->pop_back();
	}
}

/*!
 * Pressing the right mouse button (RMB) on an empty field will initiate a movement
 * to that field.
 * Pressing the right mouse button (RMB) on a field occupied by another creature will initiate an
 * attack on that creature.
 * Pressing the left mouse button (LMB) on a door adjacent to the current hero's position
 * will open that door.
 */
void QuestBoard::handleMouseEventInActionModeMoveOrAttackOrOpenDoor(QMouseEvent* event)
{
    // get the field which was clicked
    NodeID clicked_field(0, 0);
    bool valid_field = getNodeID(Vec2i(event->pos().x(), event->pos().y()), &clicked_field);
    if (!valid_field)
    {
        _movement_path.clear();
        _attack_destination_valid = false;
        return;
    }

    // RMB pressed / clicked / held down: move or attack actions
    if (((event->buttons() & Qt::RightButton) != Qt::NoButton) || (event->button() == Qt::RightButton))
    {
        // TODO: move this to a sub method

        Hero* current_hero = HeroQuestLevelWindow::_hero_quest->getLevel()->getCurrentlyActingHero();
        if (current_hero == 0)
            return;

        // nothing to do if clicked_field has not yet been discovered (except when hero can move through walls)
        if (!HeroQuestLevelWindow::_hero_quest->getPlayground()->getNodeVisibility(clicked_field)
                && !current_hero->getCanMoveThroughWalls())
        {
            _movement_path.clear();
            _attack_destination_valid = false;
            return;
        }

        // is there another creature on the clicked_field?
        Creature* target_creature = HeroQuestLevelWindow::_hero_quest->getPlayground()->getCreature(clicked_field);
        bool clicked_creature = target_creature != 0;
        // !clicked_creature => MOVE
        // clicked_creature => ATTACK

        // ======== !clicked_creature with right mouse button => move ========

        if (!clicked_creature && event->type() == QEvent::MouseButtonPress && event->button() == Qt::RightButton)
        {
            _attack_destination_valid = false;

            computeMovementPath(clicked_field);
            HeroQuestLevelWindow::_hero_quest->getPlayground()->update();
            return;
        }

        if (!clicked_creature && event->type() == QEvent::MouseButtonRelease && event->button() == Qt::RightButton)
        {
            _attack_destination_valid = false;

            // if the length of the _movement_path is > 0, the hero actually moves
            if (_movement_path.size() > 0)
            {
                vector<NodeID> movement_path_local(_movement_path);
                _movement_path.clear();
                HeroQuestLevelWindow::_hero_quest->getPlayground()->update();

                // execute the movements
                HeroQuestLevelWindow::_hero_quest->getLevel()->moveHeroMovementPath(*current_hero, movement_path_local);
            }
            return;
        }

        if (!clicked_creature && event->type() == QEvent::MouseMove && (event->buttons() & Qt::RightButton))
        {
            _attack_destination_valid = false;

            // update the movement path
            computeMovementPath(clicked_field);
            HeroQuestLevelWindow::_hero_quest->getPlayground()->update();
            return;
        }

        // ======== clicked_creature with right mouse button => attack ========

        if (clicked_creature
                && (target_creature == HeroQuestLevelWindow::_hero_quest->getLevel()->getCurrentlyActingCreature()))
        {
            // clicked currently acting hero => simply reset all stuff
            _attack_destination_valid = false;
            _movement_path.clear();
        }

        if (clicked_creature && event->type() == QEvent::MouseButtonPress && event->button() == Qt::RightButton)
        {
            if (!HeroQuestLevelWindow::_hero_quest->getLevel()->currentHeroMayAttack())
                return;

            _movement_path.clear();

            setAttackDestination(event->pos().x(), event->pos().y());
            HeroQuestLevelWindow::_hero_quest->getPlayground()->update();
            return;
        }

        if (clicked_creature && event->type() == QEvent::MouseButtonRelease && event->button() == Qt::RightButton)
        {
            if (!HeroQuestLevelWindow::_hero_quest->getLevel()->currentHeroMayAttack())
                return;

            _movement_path.clear();

            // if _attack_destination_valid is true, the hero actually attacks
            if (_attack_destination_valid)
            {
                NodeID attack_destination_local = _attack_destination;
                _attack_destination_valid = false;
                HeroQuestLevelWindow::_hero_quest->getPlayground()->update();

                // execute the attack
                HeroQuestLevelWindow::_hero_quest->getLevel()->currentHeroAttacks(_attack_destination);
            }
            return;
        }

        if (clicked_creature && event->type() == QEvent::MouseMove && (event->buttons() & Qt::RightButton))
        {
            if (!HeroQuestLevelWindow::_hero_quest->getLevel()->currentHeroMayAttack())
                return;

            _movement_path.clear();

            // update the attack destination
            setAttackDestination(event->pos().x(), event->pos().y());
            HeroQuestLevelWindow::_hero_quest->getPlayground()->update();
            return;
        }
    }

    // LMB pressed / clicked / held down: open door actions
    if (((event->buttons() & Qt::LeftButton) != Qt::NoButton) || (event->button() == Qt::LeftButton))
    {
        // TODO: move this to a sub method
        if ((event->type() == QEvent::MouseButtonPress && event->button() == Qt::LeftButton)
                || (event->type() == QEvent::MouseMove && (event->buttons() & Qt::LeftButton) != Qt::NoButton))
        {
            setDestinationDoor(event->pos().x(), event->pos().y());
            return;
        }

        if (event->type() == QEvent::MouseButtonRelease && event->button() == Qt::LeftButton)
        {
            // if _destination_door_valid is true, the door is actually opened
            if (_destination_door_valid)
            {
                _destination_door_valid = false;

                // open the door, if not yet open
                if (!_destination_door->getOpen())
                    HeroQuestLevelWindow::_hero_quest->getLevel()->openDoor(_destination_door);
            }
            return;
        }
    }
}

void QuestBoard::handleMouseEventInActionModeSelectFieldOrDoor(QMouseEvent* event)
{
    // RMB pressed / clicked / held down: select field
    if (((event->buttons() & Qt::RightButton) != Qt::NoButton) || (event->button() == Qt::RightButton))
    {
        // get the field which was clicked
        NodeID clicked_field(0, 0);
        bool valid_field = getNodeID(Vec2i(event->pos().x(), event->pos().y()), &clicked_field);
        if (!valid_field)
        {
            HeroQuestLevelWindow::_hero_quest->setUserSelectedNodeID(NodeID(-1, -1));
        }
        else
        {
            HeroQuestLevelWindow::_hero_quest->setUserSelectedNodeID(clicked_field);
        }
    }

    // LMB pressed / clicked / held down: select two fields
    if (((event->buttons() & Qt::LeftButton) != Qt::NoButton) || (event->button() == Qt::LeftButton))
    {
        // get the 2 fields which are nearest to the clicked position
        pair<NodeID, NodeID> selected_fields(make_pair(NodeID(-1, -1), NodeID(-1, -1)));
        bool valid_fields = getTransitionByNearestNeighbours(Vec2i(event->pos().x(), event->pos().y()),
                &selected_fields);
        if (!valid_fields)
        {
            HeroQuestLevelWindow::_hero_quest->setUserSelectedTwoNodeIDs(make_pair(NodeID(-1, -1), NodeID(-1, -1)));
        }
        else
        {
            HeroQuestLevelWindow::_hero_quest->setUserSelectedTwoNodeIDs(selected_fields);
        }
    }
}

void QuestBoard::handleMouseEventInActionModeSelectFieldInVisualLineOfSight(QMouseEvent* event,
        const Hero* related_hero)
{
    const NodeID* related_hero_node_id = HeroQuestLevelWindow::_hero_quest->getPlayground()->getCreaturePos(
            *related_hero);
    
    // RMB pressed / clicked / held down: select field
    if (((event->buttons() & Qt::RightButton) != Qt::NoButton) || (event->button() == Qt::RightButton))
    {
        // get the field which was clicked; check validity
        NodeID clicked_field(0, 0);
        bool valid_field = getNodeID(Vec2i(event->pos().x(), event->pos().y()), &clicked_field);
        if (!valid_field || // valid field?
                !nodesAreInSameRowOrColumn(clicked_field, *related_hero_node_id) || // same row or column?
                !fieldCanBeViewedFromField(clicked_field, *related_hero_node_id, false /*respect_field2_borders*/)) // field can be viewed?
        {
            HeroQuestLevelWindow::_hero_quest->setUserSelectedNodeID(NodeID(-1, -1));
            return;
        }
        
        HeroQuestLevelWindow::_hero_quest->setUserSelectedNodeID(clicked_field);
    }
}

void QuestBoard::handleMouseEventInActionModeSelectAdjacentField(QMouseEvent* event, const Hero* related_hero)
{
    const NodeID* related_hero_node_id = HeroQuestLevelWindow::_hero_quest->getPlayground()->getCreaturePos(
            *related_hero);
    const Node& related_hero_node = _board_graph->getNode(*related_hero_node_id);

    // LMB pressed / clicked / held down: select field
    if (((event->buttons() & Qt::LeftButton) != Qt::NoButton) || (event->button() == Qt::LeftButton))
    {
        // get the field which was clicked; check validity
        NodeID clicked_field(0, 0);
        bool valid_field = getNodeID(Vec2i(event->pos().x(), event->pos().y()), &clicked_field);
        if (!valid_field || // valid field?
                !related_hero_node.isNeighbor(clicked_field)) // neighbors?
        {
            HeroQuestLevelWindow::_hero_quest->setUserSelectedNodeID(NodeID(-1, -1));
            return;
        }

        HeroQuestLevelWindow::_hero_quest->setUserSelectedNodeID(clicked_field);
    }
}

void QuestBoard::mouseEvent(QMouseEvent* event)
{
    Playground* playground = HeroQuestLevelWindow::_hero_quest->getPlayground();

    switch (playground->getActionMode())
	{
        case Playground::ACTION_MODE_MOVE_OR_ATTACK_OR_OPEN_DOOR:
        {
            handleMouseEventInActionModeMoveOrAttackOrOpenDoor(event);
        }
            break;

        case Playground::ACTION_MODE_SELECT_FIELD_OR_DOOR:
        {
            handleMouseEventInActionModeSelectFieldOrDoor(event);
        }
            break;

        case Playground::ACTION_MODE_SELECT_FIELD_IN_VISUAL_LINE_OF_SIGHT:
        {
            handleMouseEventInActionModeSelectFieldInVisualLineOfSight(event, playground->getActionModeRelatedHero());
        }
            break;

        case Playground::ACTION_MODE_SELECT_ADJACENT_FIELD:
        {
            handleMouseEventInActionModeSelectAdjacentField(event, playground->getActionModeRelatedHero());
        }
            break;

        default:
            DVX(("QuestBoard::mouseEvent: action mode NYI"))
            ;
            break;
	}
}
