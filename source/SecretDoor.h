#ifndef SECRETDOOR_H
#define SECRETDOOR_H

#include "Door.h"

class QPainter;

/*!
 * A door connects a room to another room, or a room to a hallway.
 */
class SecretDoor : public Door
{
public:
    typedef Door Parent;

	enum DrawPos {
		DRAW_POS_F1 = 0,
		DRAW_POS_F2
	};

	SecretDoor();
	SecretDoor(const NodeID& f1, const NodeID& f2, const DrawPos& draw_pos);
	virtual ~SecretDoor();

	virtual void setOpen(bool open);

	bool getVisible() const
	{
		return _visible;
	}

	void setVisible(bool visible)
	{
		_visible = visible;
	}

	const DrawPos& getDrawPos() const
	{
		return _draw_pos;
	}

	const NodeID& getDrawPosNodeID() const
	{
		return _draw_pos == DRAW_POS_F1 ? _f1 : _f2;
	}

	void setDrawPos(const DrawPos& draw_pos)
	{
		_draw_pos = draw_pos;
	}

	//! Just calls redraw(), because the "selected" argument make so sense for SecretDoor.
    virtual void redraw(QPainter& painter, bool selected)
	{
        redraw(painter);
	}

    void redraw(QPainter& painter);

    virtual bool save(std::ostream& stream) const;
    virtual bool load(std::istream& stream);

protected:
	bool _visible;

	//! The node_id where the secret door is shown (if it is visible); "points" to _f1 or to _f2.
	DrawPos _draw_pos;

private:
	// this constructor is not useful for SecretDoor
	SecretDoor(const NodeID& f1, const NodeID& f2);

    float getRotationAngle() const;
};

#endif
