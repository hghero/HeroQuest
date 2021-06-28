#include "Door.h"

#include <QtGui/QPainter>

#include "QuestBoard.h"
#include "SecretDoor.h"
#include "HeroQuestLevelWindow.h"
#include "Playground.h"
#include "SaveContext.h"
#include "LoadContext.h"

using namespace std;


Door::Door()
        :
        _f1(NodeID(0, 0)),
  _f2(NodeID(0, 0)),
  _open(false)
{
	// NIX
}

Door::Door(const NodeID& f1, const NodeID& f2)
        :
        _f1(f1),
  _f2(f2),
  _open(false)
{
	// NIX
}

Door::~Door()
{
	// NIX
}

bool Door::getOpen() const
{
	return _open;
}

void Door::setOpen(bool open)
{
	_open = open;
}

/*!
 * Draws the door as (thick) line from one corner of _f1 to another corner of _f1,
 * the corners depending on the position of _f2 relative to _f1.
 */
void Door::redraw(QPainter& painter, bool selected)
{
    QuestBoard* quest_board = HeroQuestLevelWindow::_hero_quest->getPlayground()->getQuestBoard();
    if (quest_board == 0)
		return;

	// determine the door end points, in screen pixel coordinates
	Vec2i p1(0, 0);
	Vec2i p2(0, 0);
	if (_f2._ix == _f1._ix)
	{
		if (_f2._iy < _f1._iy) // _f2 is above _f1
		{
            p1 = quest_board->getFieldCorner0(_f1);
            p2 = quest_board->getFieldCorner1(_f1);
		}
		else
		{
			// _f2 is below _f1
            p1 = quest_board->getFieldCorner2(_f1);
            p2 = quest_board->getFieldCorner3(_f1);
		}
	}
	else
	{
		if (_f2._ix < _f1._ix) // _f2 is left of _f1
		{
            p1 = quest_board->getFieldCorner0(_f1);
            p2 = quest_board->getFieldCorner2(_f1);
		}
		else
		{
			// _f2 is right of _f1
            p1 = quest_board->getFieldCorner1(_f1);
            p2 = quest_board->getFieldCorner3(_f1);
		}
	}

	// the color of the door depends on its opened state
	QColor color;
	if (_open)
		color.setRgb(0, 150, 0, 255);
	else if (!_open && selected)
		color.setRgb(255, 255, 0, 255);
	else
		// (!_open && !selected)
		color.setRgb(150, 0, 0, 255);
	
	// draw the door as thick line
	painter.save();

	const float THICKNESS = 5;
	QPen pen(color, THICKNESS, Qt::SolidLine, Qt::SquareCap, Qt::BevelJoin);
	painter.setPen(pen);

	painter.drawLine(p1.x, p1.y, p2.x, p2.y);

	painter.restore();
}

QString Door::getName(const Door* door)
{
    Door* door_non_const = const_cast<Door*>(door);
    QString name("Door");
    if (dynamic_cast<SecretDoor*>(door_non_const) != 0)
        name = "SecretDoor";
    return name;
}

bool Door::save(SaveContext& save_context) const
{
    SaveContext::OpenChapter open_chapter(save_context, "Door");

    _f1.save(save_context);
    _f2.save(save_context);
    save_context.writeBool(_open, "_open");

    return !save_context.fail();
}

bool Door::load(LoadContext& load_context)
{
    LoadContext::OpenChapter open_chapter(load_context, "Door");

    _f1.load(load_context);
    _f2.load(load_context);

    load_context.readBool(&_open, "_open");

    return !load_context.fail();
}
