#ifndef TRAP_H
#define TRAP_H

#include <QtCore/QTime>

#include "GraphBase.h"

class QuestBoard;
class QPainter;

// =============================================================================

class Trap
{
public:
    Trap();
    Trap(const NodeID& node_id);
	virtual ~Trap();

	virtual void redraw(QPainter& painter);

	bool getVisible() const;
	void setVisible(bool visible);

	const NodeID& getNodeID() const;
	void setNodeID(const NodeID& node_id);

	virtual bool isSpearTrap() const;

    static QString getName(const Trap* trap);

    virtual bool save(std::ostream& stream) const;
    virtual bool load(std::istream& stream);

protected:
	bool _visible;

	//! position of the trap on the board
	NodeID _node_id;

};

// =============================================================================

class PitTrap : public Trap
{
public:
    typedef Trap Parent;

    PitTrap();
    PitTrap(const NodeID& node_id);
	virtual ~PitTrap();

	virtual void redraw(QPainter& painter);

	virtual bool isSpearTrap() const;

    virtual bool save(std::ostream& stream) const;
    virtual bool load(std::istream& stream);
};

// =============================================================================

class SpearTrap : public Trap
{
public:
    typedef Trap Parent;

    SpearTrap();
    SpearTrap(const NodeID& node_id);
	virtual ~SpearTrap();

	virtual void redraw(QPainter& painter);

	virtual bool isSpearTrap() const;

    virtual bool save(std::ostream& stream) const;
    virtual bool load(std::istream& stream);

private:
	QTime _redraw_start_time;
	bool _first_redraw;
};

// =============================================================================

#endif
