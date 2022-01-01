#ifndef TRAP_H
#define TRAP_H

#include <QtCore/QElapsedTimer>
#include <QtCore/qnamespace.h>

#include "GraphBase.h"

class QuestBoard;
class QPainter;
class SaveContext;
class LoadContext;

// =============================================================================

class Trap
{
public:
    Trap();
    Trap(const NodeID& node_id);
	virtual ~Trap();

    virtual void redraw(QPainter& painter, bool highlight, Qt::GlobalColor color);

	bool getVisible() const;
	void setVisible(bool visible);

	const NodeID& getNodeID() const;
	void setNodeID(const NodeID& node_id);

	virtual bool isSpearTrap() const;

    static QString getName(const Trap* trap);

    virtual bool save(SaveContext& save_context) const;
    virtual bool load(LoadContext& load_context);

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

    virtual void redraw(QPainter& painter, bool highlight, Qt::GlobalColor color);

	virtual bool isSpearTrap() const;

    virtual bool save(SaveContext& save_context) const;
    virtual bool load(LoadContext& load_context);
};

// =============================================================================

class SpearTrap : public Trap
{
public:
    typedef Trap Parent;

    SpearTrap();
    SpearTrap(const NodeID& node_id);
	virtual ~SpearTrap();

    virtual void redraw(QPainter& painter, bool highlight, Qt::GlobalColor color);

	virtual bool isSpearTrap() const;

    virtual bool save(SaveContext& save_context) const;
    virtual bool load(LoadContext& load_context);

private:
	QElapsedTimer _redraw_start_time;
	bool _first_redraw;
};

// =============================================================================

#endif
