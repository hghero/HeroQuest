#include "SecretDoor.h"

#include <iostream>

#include "Playground.h"
#include "QuestBoard.h"
#include "StreamUtils.h"


using namespace std;


SecretDoor::SecretDoor()
:
Door(),
_visible(false),
_draw_pos(DRAW_POS_F1)
{
	// NIX
}

SecretDoor::SecretDoor(const NodeID& f1, const NodeID& f2, const DrawPos& draw_pos)
:
Door(f1, f2),
_visible(false),
_draw_pos(draw_pos)
{
	// NIX
}

//! This constructor is not useful for SecretDoor.
SecretDoor::SecretDoor(const NodeID& f1, const NodeID& f2)
:
Door(f1, f2),
_visible(false),
_draw_pos(DRAW_POS_F1)
{
	cout << "Warning: disabled constructor called: SecretDoor(const NodeID& f1, const NodeID& f2)" << endl;
}

SecretDoor::~SecretDoor()
{
	// NIX
}

/*!
 * As soon as a SecretDoor is opened, it is visible.
 */
void SecretDoor::setOpen(bool open)
{
	Door::setOpen(open);

	if (open)
		setVisible(true);
}

void SecretDoor::redraw()
{
	if (!_visible)
		return;
/*
	QPixmap* secret_door_image = _quest_board->getPlayground()->getSecretDoorImage();

	// --- We assume a square image! ---

	// original image size
	int secret_door_orig_size = al_get_bitmap_width(secret_door_image);

	const int FIELD_MARGIN = int(_quest_board->getFieldWidth() / float(11) + 0.5);
	// destination image size = min(field_width, field_height) - MARGIN
	int secret_door_size = min(_quest_board->getFieldWidth(), _quest_board->getFieldHeight()) - FIELD_MARGIN;

	Vec2i secret_door_center_pos(_quest_board->getFieldCenter(getDrawPosNodeID()));

	float scale = secret_door_size / float(secret_door_orig_size);

	// the bitmap must be rotated, so that the white border matches the wall between _f1 and _f2;
	// with angle = 0, the border is at the top;
	// angle must be given clockwise in radians
	float angle = 0; // ALLEGRO_PI / 2
	if (((_f1._ix < _f2._ix) && (_draw_pos == DRAW_POS_F1)) ||
		((_f2._ix < _f1._ix) && (_draw_pos == DRAW_POS_F2)))
	{
		// border at right side => rotate by 90 degress clockwise
		angle = ALLEGRO_PI / 2;
	}
	else if (((_f1._ix < _f2._ix) && (_draw_pos == DRAW_POS_F2)) ||
		     ((_f2._ix < _f1._ix) && (_draw_pos == DRAW_POS_F1)))
	{
		// border at left side => rotate by 270 degress clockwise
		angle = ALLEGRO_PI * 1.5;
	}
	else if (((_f1._iy < _f2._iy) && (_draw_pos == DRAW_POS_F1)) ||
		     ((_f2._iy < _f1._iy) && (_draw_pos == DRAW_POS_F2)))
	{
		// border at bottom side => rotate by 180 degress clockwise
		angle = ALLEGRO_PI;
	}

	al_draw_scaled_rotated_bitmap(
		secret_door_image,
		secret_door_orig_size / float(2), secret_door_orig_size / float(2),
		secret_door_center_pos.x, secret_door_center_pos.y,
		scale, scale,
		angle,
		0);
		*/
}

bool SecretDoor::save(std::ostream& stream) const
{
    Parent::save(stream);

    StreamUtils::writeBool(stream, _visible);
    StreamUtils::writeUInt(stream, uint(_draw_pos));

    return !stream.fail();
}

bool SecretDoor::load(std::istream& stream)
{
    Parent::load(stream);

    StreamUtils::readBool(stream, &_visible);

    uint draw_pos_tmp;
    StreamUtils::readUInt(stream, &draw_pos_tmp);
    _draw_pos = (DrawPos)(draw_pos_tmp);

    return !stream.fail();
}
