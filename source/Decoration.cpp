#include "Decoration.h"

#include <vector>

#include <QtCore/QString.h>

#include "QuestBoard.h"
#include "Debug.h"
#include "Level.h"
#include "NonWalkableDecoration.h"
#include "WalkableDecoration.h"
#include "StreamUtils.h"
#include "Playground.h"
#include "ParameterStorage.h"
#include "SaveContext.h"
#include "LoadContext.h"

using namespace std;


// =============================================================================

Decoration::Decoration()
: Reference(),
  _visible(true),
  _node_top_left(0, 0),
  _node_bottom_right(0, 0),
  _orientation(TURN_0)
{
	// NIX
}

Decoration::Decoration(const NodeID& node_top_left, const NodeID& node_bottom_right, const Orientation& orientation)
: Reference(),
  _visible(true),
  _node_top_left(node_top_left),
  _node_bottom_right(node_bottom_right),
  _orientation(orientation)
{
	// NIX
}

Decoration::~Decoration()
{
	// NIX
}

bool Decoration::getVisible() const
{
	return _visible;
}

void Decoration::setVisible(bool visible)
{
	_visible = visible;
}

const NodeID& Decoration::getNodeTopLeft() const
{
	return _node_top_left;
}

void Decoration::setNodeTopLeft(const NodeID& node_top_left)
{
	_node_top_left = node_top_left;
}

const NodeID& Decoration::getNodeBottomRight() const
{
	return _node_bottom_right;
}

void Decoration::setNodeBottomRight(const NodeID& node_bottom_right)
{
	_node_bottom_right = node_bottom_right;
}

void Decoration::redraw(QPainter& painter)
{
    //#define DEBUG_TRAFO
	if (!_visible)
		return;

	QPixmap* image = getImage();
	if (image == 0)
	    return;

    QuestBoard* quest_board = HeroQuestLevelWindow::_hero_quest->getPlayground()->getQuestBoard();

	// original image size
    DV(("================= %s ==============", qPrintable(getName(this))));
	Vec2i image_orig_size(image->width(), image->height());
	Vec2f image_orig_center(image_orig_size.x / float(2), image_orig_size.y / float(2));
    DV(("image_orig_size: %d x %d", image_orig_size.x, image_orig_size.y));
    DV(("image_orig_center: %f x %f", image_orig_center.x, image_orig_center.y));

    // on the board, the Decoration shall cover as much as possible of the rectangle defined by used_board_fields_size
    Vec2f used_board_fields_size(
            quest_board->getFieldCorner3(_node_bottom_right).x - quest_board->getFieldCorner0(_node_top_left).x + 1,
            quest_board->getFieldCorner3(_node_bottom_right).y - quest_board->getFieldCorner0(_node_top_left).y + 1);
    DV(("used_board_fields_size: %f x %f", used_board_fields_size.x, used_board_fields_size.y));

	int num_fields_x = _node_bottom_right._ix - _node_top_left._ix + 1;
	int num_fields_y = _node_bottom_right._iy - _node_top_left._iy + 1;
    // leave some pixels free at the 4 sides of the Decoration
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
	if (_orientation == TURN_0 || _orientation == TURN_180)
	{
		// keep orig x/y

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
	}
	else
	{
		// exchange x/y

		// try to scale x (scale_x corresponds to the Playground's x axis, which is the orig image's y axis)
		float scale_x = image_max_size.x / float(image_orig_size.y);
		int height_after_scale_applied = int(round(image_orig_size.x * scale_x));
		if (height_after_scale_applied > image_max_size.y)
		{
			// scale y
			float scale_y = image_max_size.y / float(image_orig_size.x);
			scale = scale_y;
		}
		else
		{
			// scale x
			scale = scale_x;
		}

		image_transformed_size *= scale;
		image_transformed_size = Vec2f(image_transformed_size.y, image_transformed_size.x);
	}
    DV(("image_transformed_size: %f x %f", image_transformed_size.x, image_transformed_size.y));

	// determine top left pixel of transformed image, respecting the centering relatively to the used_board_fields_size
    Vec2f image_pos_top_left = Vec2f(quest_board->getFieldCorner0(_node_top_left))
            + used_board_fields_size * 0.5
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
	if (_orientation == TURN_0 || _orientation == TURN_180)
	    trafo_03_translate_back_to_origin.translate(image_orig_center.x, image_orig_center.y);
	else
	    trafo_03_translate_back_to_origin.translate(image_orig_center.y, image_orig_center.x);
	// 4. scale
	QTransform trafo_04_scale;
	trafo_04_scale.scale(scale, scale);
	// 5. translate to draw position
	QTransform trafo_05_translate_to_draw_position;
	trafo_05_translate_to_draw_position.translate(image_pos_top_left.x, image_pos_top_left.y);

	QTransform accumulated_transform =
	        trafo_01_translate_to_center_of_image *
	        trafo_02_rotate *
	        trafo_03_translate_back_to_origin *
	        trafo_04_scale *
	        trafo_05_translate_to_draw_position;


	painter.setTransform(accumulated_transform);

#ifdef DEBUG_TRAFO
	cout << "transform: " << accumulated_transform.m11() << " " << accumulated_transform.m12() << " " << accumulated_transform.m13() << endl;
	cout << "           " << accumulated_transform.m21() << " " << accumulated_transform.m22() << " " << accumulated_transform.m23() << endl;
	cout << "           " << accumulated_transform.m31() << " " << accumulated_transform.m32() << " " << accumulated_transform.m33() << endl;
#endif

	painter.drawPixmap(0, 0, *image);

	painter.restore();
}

bool Decoration::containsNode(const NodeID& node) const
{
	return
		node._ix >= _node_top_left._ix && node._ix <= _node_bottom_right._ix &&
		node._iy >= _node_top_left._iy && node._iy <= _node_bottom_right._iy;
}

void Decoration::getNodes(vector<NodeID>* nodes) const
{
	nodes->clear();
	nodes->reserve((_node_bottom_right._iy - _node_top_left._iy + 1) * (_node_bottom_right._ix - _node_top_left._ix + 1));
	for (int y = _node_top_left._iy; y <= _node_bottom_right._iy; ++y)
	{
		for (int x = _node_top_left._ix; x <= _node_bottom_right._ix; ++x)
		{
			nodes->push_back(NodeID(x, y));
		}
	}
}

QPixmap* Decoration::getImage() const
{
	cout << "Warning: Decoration::getImage() called" << endl;
	return 0;
}

bool Decoration::isWalkable() const
{
	cout << "WARNING: Decoration::isWalkable() called" << endl;
	return true;
}

QString Decoration::getName(const Decoration* decoration)
{
    Decoration* decoration_non_const = const_cast<Decoration*>(decoration);

    QString name("Unknown");

    // NonWalkable
    if (dynamic_cast<Chest*>(decoration_non_const) != 0)
        name = "Chest";
    else if (dynamic_cast<AlchemistTable*>(decoration_non_const) != 0)
        name = "AlchemistTable";
    else if (dynamic_cast<Armory*>(decoration_non_const) != 0)
        name = "Armory";
    else if (dynamic_cast<Cupboard*>(decoration_non_const) != 0)
        name = "Cupboard";
    else if (dynamic_cast<Fireplace*>(decoration_non_const) != 0)
        name = "Fireplace";
    else if (dynamic_cast<Sarcophagus*>(decoration_non_const) != 0)
        name = "Sarcophagus";
    else if (dynamic_cast<Table*>(decoration_non_const) != 0)
        name = "Table";
    else if (dynamic_cast<Throne*>(decoration_non_const) != 0)
        name = "Throne";
    else if (dynamic_cast<TortureRack*>(decoration_non_const) != 0)
        name = "TortureRack";
    else if (dynamic_cast<WarlockTable*>(decoration_non_const) != 0)
        name = "WarlockTable";
    else if (dynamic_cast<Debris*>(decoration_non_const) != 0)
        name = "Debris";

    // Walkable
    else if (dynamic_cast<Stairs*>(decoration_non_const) != 0)
        name = "Stairs";

    return name;
}

bool Decoration::save(SaveContext& save_context) const
{
    SaveContext::OpenChapter open_chapter(save_context, "Decoration");

    Parent::save(save_context);

    // _visible
    save_context.writeBool(_visible, "_visible");

    // _node_top_left
    _node_top_left.save(save_context);

    // _node_bottom_right
    _node_bottom_right.save(save_context);

    // _orientation
    save_context.writeUInt(uint(_orientation), "_orientation");

    return !save_context.fail();
}

bool Decoration::load(LoadContext& load_context)
{
    LoadContext::OpenChapter open_chapter(load_context, "Decoration");

    Parent::load(load_context);

    // _visible
    load_context.readBool(&_visible, "_visible");

    // _node_top_left
    _node_top_left.load(load_context);

    // _node_bottom_right
    _node_bottom_right.load(load_context);

    // _orientation
    uint orientation_uint = 0;
    load_context.readUInt(&orientation_uint, "_orientation");
    switch (orientation_uint)
    {
        case 0: _orientation = TURN_0;
                break;
        case 1: _orientation = TURN_90;
                break;
        case 2: _orientation = TURN_180;
                break;
        case 3: _orientation = TURN_270;
                break;
        default: DVX(("Unknown Decoration orientation %d!", orientation_uint));
                 return false;
                 break;
    }

    return !load_context.fail();
}

float Decoration::getRotationAngle() const
{
	float angle = 0;
	switch (_orientation)
	{
		case TURN_0:
			angle = 0;
			break;
		case TURN_90:
			angle = 90;
			break;
		case TURN_180:
			angle = 180;
			break;
		case TURN_270:
			angle = 270;
			break;
	}

	return angle;
}

// =============================================================================
