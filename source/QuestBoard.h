#ifndef QUEST_BOARD_H
#define QUEST_BOARD_H

#include <string>
#include <map>
#include <list>
#include "Math.h"
#include "BoardGraph.h"
#include <QtGui/QPixmap>

class Playground;
class Door;
class Hero;
class QMouseEvent;

class QuestBoard
{
public:
	static const QString QUEST_BOARD_FILENAME;
	static const int FIELDS_X;
	static const int FIELDS_Y;

	/*!
	 * The board image is placed at the top of the display. The height is set such that there
	 * is a given percentage (1 - board_height_percentage) of pixels free at the bottom.
	 * The width is then computed from the height and source width/height ratio of the image
	 * (see create() method).
	 *
	 * \param source_width Width of the parent display in pixels.
	 * \param source_height Height of the parent display in pixels.
	 * \param board_height_percentage Height percentage of the parent display which will used for drawing the QuestBoard.
	 */
    QuestBoard();
	virtual ~QuestBoard();

	bool create();

	void redraw(QPainter& painter);

	Vec2i origToScreen(const Vec2i& coord_orig) const;

	// Get absolute screen coordinates
	//
	// 0           1
	//
	//    center
	//
	// 2           3
	Vec2i getFieldCenter(const NodeID& node_id) const;
	Vec2i getFieldCorner0(const NodeID& node_id) const;
	Vec2i getFieldCorner1(const NodeID& node_id) const;
	Vec2i getFieldCorner2(const NodeID& node_id) const;
	Vec2i getFieldCorner3(const NodeID& node_id) const;
	Vec2i getFieldCorner(const NodeID& node_id, unsigned int corner_num) const;
	/*
	void computeViewedFields(const NodeID& field, std::vector<NodeID>* viewed_fields) const;
	*/
	bool fieldCanBeViewedFromField(const NodeID& field1, const NodeID& field2, bool respect_field2_borders) const;

	bool getNodeID(const Vec2i& screen_coords, NodeID* node_id) const;
	bool getTransitionByNearestNeighbours(const Vec2i& screen_coords, std::pair<NodeID, NodeID>* transition) const;

	//! width and height of a field in screen pixels
	int getFieldWidth() const;
	int getFieldHeight() const;

	unsigned int getRoomID(const NodeID& field) const;

	const std::list<NodeID>& getRoomFields(unsigned int room_id) const;

	void mouseEvent(QMouseEvent* event);

	BoardGraph* getBoardGraph() const
	{
		return _board_graph;
	}

	bool getAttackDestinationValid() const
	{
		return _attack_destination_valid;
	}

	const std::vector<NodeID>& getMovementPath() const
	{
		return _movement_path;
	}

	const NodeID& getAttackDestination() const
	{
		return _attack_destination;
	}

	void getGeometricNeighbors(const NodeID& node_id, std::vector<NodeID>* geometric_neighbors) const;
	bool isDestinationDoor(const Door& door) const;

	bool contains(const NodeID& node_id) const;

    virtual bool save(std::ostream& stream) const;
    virtual bool load(std::istream& stream);

    void computeReachableArea();
    void clearReachableArea();

private:
    void computeMovementPath(const NodeID& node_id);

	void setAttackDestination(int x, int y);

	void setDestinationDoor(int x, int y);

	void removeOccupiedFieldsFromEnd(std::vector<NodeID>* path) const;

	void getBorderLinesOfNode(const NodeID& node_id, std::vector<Line>* node_borders, bool only_wall_borders, const NodeID* ignore_field) const;

	void handleMouseEventInActionModeMoveOrAttackOrOpenDoor(QMouseEvent* event);
    void handleMouseEventInActionModeSelectFieldOrDoor(QMouseEvent* event);

	void assignRoomIDs();
	void assignRoomIDs(const NodeID& node_id_top_left, unsigned int width, unsigned int height, unsigned int id);

    // the board image
	QPixmap* _orig_board_image;
	QPixmap _scaled_board_image;

	// the positions (in orig pixels) of the top left corners of the board fields
	StorageMatrix<Vec2i> _field_pos;

	// width and height of a field in orig pixels
	int _orig_field_width;
	int _orig_field_height;

	// the rectangular board with fields as nodes, and connections between fields as edges
	BoardGraph* _board_graph;

	// mapping from QuestBoard fields to rooms
	std::map<NodeID, unsigned int> _room_ids;
	std::map<unsigned int, std::list<NodeID> > _room_ids_reverse;

	// shows the movement path of the currently acting hero; the movement path is created
	// from a mouse click to a destination board field
	std::vector<NodeID> _movement_path;

    // set of nodes reachable by the currently acting hero considering the current movement dice result
    std::set<NodeID> _reachable_area;

	// attacking
	bool _attack_destination_valid;
	NodeID _attack_destination;

	// opening doors
	bool _destination_door_valid;
	const Door* _destination_door;
};

#endif
