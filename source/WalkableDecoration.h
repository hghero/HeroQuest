#ifndef WALKABLE_DECORATION_H
#define WALKABLE_DECORATION_H

#include "Decoration.h"

// =============================================================================

class WalkableDecoration : public Decoration
{
public:
	WalkableDecoration();
	WalkableDecoration(const NodeID& node_top_left, const NodeID& node_bottom_right, const Orientation& orientation);
	virtual ~WalkableDecoration();

	virtual QPixmap* getImage() const;

	virtual bool isWalkable() const;
};

// =============================================================================

class Stairs : public WalkableDecoration
{
public:
	Stairs();
	Stairs(const NodeID& node_top_left, const NodeID& node_bottom_right, const Orientation& orientation);
	Stairs(const NodeID& node_top_left, const Orientation& orientation);
	virtual ~Stairs();

	virtual QPixmap* getImage() const;
};

// =============================================================================

#endif
