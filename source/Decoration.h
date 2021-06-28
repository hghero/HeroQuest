#ifndef DECORATION_H
#define DECORATION_H

#include <QtGui/QPainter>

#include "GraphBase.h"
#include "Reference.h"

class SaveContext;
class LoadContext;

// =============================================================================

class Decoration : public Reference
{
public:
    typedef Reference Parent;

	enum Orientation {
		TURN_0 = 0,
		TURN_90,
		TURN_180,
		TURN_270
	};

	Decoration();
	Decoration(const NodeID& node_top_left, const NodeID& node_bottom_right, const Orientation& orientation);
	virtual ~Decoration();

	void redraw(QPainter& painter);

	bool getVisible() const;
	void setVisible(bool visible);

	const NodeID& getNodeTopLeft() const;
	void setNodeTopLeft(const NodeID& node_top_left);

	const NodeID& getNodeBottomRight() const;
	void setNodeBottomRight(const NodeID& node_bottom_right);

	bool containsNode(const NodeID& node) const;
	void getNodes(std::vector<NodeID>* nodes) const;

	virtual QPixmap* getImage() const;

	virtual bool isWalkable() const;

	static QString getName(const Decoration* decoration);

    virtual bool save(SaveContext& save_context) const;
    virtual bool load(LoadContext& load_context);

protected:
	float getRotationAngle() const;

	bool _visible;

	NodeID _node_top_left;
	NodeID _node_bottom_right; // already respecting the orientation!

	Orientation _orientation;
};

// =============================================================================

#endif
