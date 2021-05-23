#include "SecretDoor.h"

#include <iostream>

#include <QtGui/QPainter>

#include "Playground.h"
#include "QuestBoard.h"
#include "StreamUtils.h"
#include "HeroQuestLevelWindow.h"
#include "ParameterStorage.h"


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

void SecretDoor::redraw(QPainter& painter)
{
	if (!_visible)
		return;

    QPixmap* image = HeroQuestLevelWindow::_hero_quest->getPlayground()->getSecretDoorImage();
    if (image == 0)
        return;

    QuestBoard* quest_board = HeroQuestLevelWindow::_hero_quest->getPlayground()->getQuestBoard();

    // original image size
    DV(("================= %s ==============", qPrintable(getName(this))));
    Vec2i image_orig_size(image->width(), image->height());
    Vec2f image_orig_center(image_orig_size.x / float(2), image_orig_size.y / float(2));
    DV(("image_orig_size: %d x %d", image_orig_size.x, image_orig_size.y));
    DV(("image_orig_center: %f x %f", image_orig_center.x, image_orig_center.y));

    const NodeID& draw_node_id = getDrawPosNodeID();

    // on the board, the SecretDoor shall cover as much as possible of the rectangle defined by used_board_fields_size
    Vec2f used_board_fields_size(
            quest_board->getFieldCorner3(draw_node_id).x - quest_board->getFieldCorner0(draw_node_id).x + 1,
            quest_board->getFieldCorner3(draw_node_id).y - quest_board->getFieldCorner0(draw_node_id).y + 1);
    DV(("used_board_fields_size: %f x %f", used_board_fields_size.x, used_board_fields_size.y));

    // leave some pixels free at the 4 sides of the SecretDoor
    const int FIELD_MARGIN = int(ParameterStorage::instance->getFieldSize() / float(11) + 0.5);
    DV(("FIELD_MARGIN: %d", FIELD_MARGIN));
    DV(("field size: %d", ParameterStorage::instance->getFieldSize()));
    // shrink used_board_fields_size a bit to have some free space at the sides
    Vec2i image_max_size(int(used_board_fields_size.x + 0.5) - 2 * FIELD_MARGIN,
            int(used_board_fields_size.y + 0.5) - 2 * FIELD_MARGIN);
    DV(("image_max_size: %d x %d", image_max_size.x, image_max_size.y));
    Vec2f image_transformed_size(image_orig_size);

#ifdef DEBUG_TRAFO
    cout << "field corner0: " << quest_board->getFieldCorner0(_node_top_left).x << " " << quest_board->getFieldCorner0(_node_top_left).x << endl;
    cout << "image_pos_top_left: " << image_pos_top_left.x << "; " << image_pos_top_left.y << endl;
#endif

    // determine scale factor
    float scale = 1;

    // try to scale x
    float scale_x = image_max_size.x / float(image_orig_size.x);
    int height_after_scale_applied = int(round(image_orig_size.y * scale_x));
    if (height_after_scale_applied > image_max_size.y)
    {
        // scale y
        float scale_y = image_max_size.y / float(image_orig_size.y);
        scale = scale_y;
    }
    else
    {
        // scale x
        scale = scale_x;
    }

    image_transformed_size *= scale;
    DV(("image_transformed_size: %f x %f", image_transformed_size.x, image_transformed_size.y));

    // determine top left pixel of transformed image, respecting the centering relatively to the used_board_fields_size
    Vec2f image_pos_top_left = Vec2f(quest_board->getFieldCorner0(draw_node_id)) + used_board_fields_size * 0.5
            - image_transformed_size * 0.5;
    DV(("image_pos_top_left: %f x %f", image_pos_top_left.x, image_pos_top_left.y));

    painter.save();

    // 1. translate to center of image
    QTransform trafo_01_translate_to_center_of_image;
    trafo_01_translate_to_center_of_image.translate(-image_orig_center.x, -image_orig_center.y);
    // 2. rotate
    QTransform trafo_02_rotate;
    trafo_02_rotate.rotate(getRotationAngle());
    // 3. translate back to origin
    QTransform trafo_03_translate_back_to_origin;
    trafo_03_translate_back_to_origin.translate(image_orig_center.x, image_orig_center.y);
    // 4. scale
    QTransform trafo_04_scale;
    trafo_04_scale.scale(scale, scale);
    // 5. translate to draw position
    QTransform trafo_05_translate_to_draw_position;
    trafo_05_translate_to_draw_position.translate(image_pos_top_left.x, image_pos_top_left.y);

    QTransform accumulated_transform = trafo_01_translate_to_center_of_image * trafo_02_rotate
            * trafo_03_translate_back_to_origin * trafo_04_scale * trafo_05_translate_to_draw_position;


    painter.setTransform(accumulated_transform);

#ifdef DEBUG_TRAFO
    cout << "transform: " << accumulated_transform.m11() << " " << accumulated_transform.m12() << " " << accumulated_transform.m13() << endl;
    cout << "           " << accumulated_transform.m21() << " " << accumulated_transform.m22() << " " << accumulated_transform.m23() << endl;
    cout << "           " << accumulated_transform.m31() << " " << accumulated_transform.m32() << " " << accumulated_transform.m33() << endl;
#endif

    painter.drawPixmap(0, 0, *image);

    painter.restore();
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

/*!
 * The secret door image in 0° rotation angle has its connection to the top.
 * Depending on _f1, _f2 and _draw_pos, this method computes how to rotate the SecretDoor image
 * for correct display.
 */
float SecretDoor::getRotationAngle() const
{
    float angle = 0;

    const NodeID& draw_node = (_draw_pos == DRAW_POS_F1) ? _f1 : _f2;
    const NodeID& other_node = (_draw_pos == DRAW_POS_F1) ? _f2 : _f1;

    if (draw_node._ix == other_node._ix)
    {
        if (draw_node._iy > other_node._iy)
            angle = 0;
        if (draw_node._iy < other_node._iy)
            angle = 180;
    }
    else if (draw_node._iy == other_node._iy)
    {
        if (draw_node._ix > other_node._ix)
            angle = 90;
        if (draw_node._ix < other_node._ix)
            angle = 270;
    }

    return angle;
}
