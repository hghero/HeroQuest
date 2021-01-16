#ifndef DOOR_H
#define DOOR_H

#include <QtCore/QString>

#include "GraphBase.h"

class QuestBoard;
class QPainter;

/*!
 * A door connects a room to another room, or a room to a hallway.
 */
class Door
{
public:
	Door();

	Door(const NodeID& f1, const NodeID& f2);

	virtual ~Door();

	bool getOpen() const;
	virtual void setOpen(bool open);

	virtual void redraw(QPainter& painter, bool selected);

	const NodeID& getF1() const
	{
		return _f1;
	}

	void setF1(const NodeID& f1)
	{
		_f1 = f1;
	}

	const NodeID& getF2() const
	{
		return _f2;
	}

	void setF2(const NodeID& f2)
	{
		_f2 = f2;
	}

	bool connects(const NodeID& n1, const NodeID& n2) const
	{
		return (n1 == _f1 && n2 == _f2) || (n1 == _f2 && n2 == _f1);
	}

	bool isIncidentTo(const NodeID& node_id) const
	{
		return _f1 == node_id || _f2 == node_id;
	}

	bool operator==(const Door& other) const
	{
		return _f1 == other._f1 && _f2 == other._f2;
	}

	bool operator!=(const Door& other) const
	{
		return !(*this == other);
	}

    static QString getName(const Door* door);

    virtual bool save(std::ostream& stream) const;
    virtual bool load(std::istream& stream);

protected:
	//! The door connects nodes f1 and f2 on the quest board.
	NodeID _f1;
	NodeID _f2;

	//! True: door is open; false: door is closed
	bool _open;
};

#endif
