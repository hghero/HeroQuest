#include "Trap.h"

#include <iostream>
#include <QtGui/QPainter>
#include <QtCore/QString>

#include "QuestBoard.h"
#include "Playground.h"
#include "Debug.h"
#include "HeroQuestLevelWindow.h"
#include "Level.h"
#include "ParameterStorage.h"
#include "StreamUtils.h"
#include "PainterContext.h"
#include "SaveContext.h"
#include "LoadContext.h"

using namespace std;

//#define DEBUG_TRAFO


// =============================================================================

Trap::Trap()
:
_visible(false),
_node_id(0, 0)
{
    // NIX
}

Trap::Trap(const NodeID& node_id)
:
_visible(false),
_node_id(node_id)
{
	// NIX
}

Trap::~Trap()
{
	// NIX
}

void Trap::redraw(QPainter& /*painter*/, bool /*highlight*/, Qt::GlobalColor /*color*/)
{
    DVX(("Internal program error: function \"Trap::redraw()\" called!"));
}

bool Trap::getVisible() const
{
	return _visible;
}

void Trap::setVisible(bool visible)
{
	_visible = visible;
}

const NodeID& Trap::getNodeID() const
{
	return _node_id;
}

void Trap::setNodeID(const NodeID& node_id)
{
	_node_id = node_id;
}

bool Trap::isSpearTrap() const
{
	return false;
}

QString Trap::getName(const Trap* trap)
{
    Trap* trap_non_const = const_cast<Trap*>(trap);
    QString name("Trap");
    if (dynamic_cast<PitTrap*>(trap_non_const) != 0)
        name = "PitTrap";
    else if (dynamic_cast<SpearTrap*>(trap_non_const) != 0)
        name = "SpearTrap";
    return name;
}

bool Trap::save(SaveContext& save_context) const
{
    SaveContext::OpenChapter open_chapter(save_context, "Trap");

    save_context.writeBool(_visible, "_visible");
    _node_id.save(save_context);

    return !save_context.fail();
}

bool Trap::load(LoadContext& load_context)
{
    load_context.readBool(&_visible, "_visible");

    _node_id.load(load_context);

    return !load_context.fail();
}

// =============================================================================

PitTrap::PitTrap()
:
Trap()
{
    // NIX
}

PitTrap::PitTrap(const NodeID& node_id)
:
        Trap(node_id)
{
	// NIX
}

PitTrap::~PitTrap()
{
	// NIX
}

// TODO: centralize such drawings of highlighted pixmaps and re-use code
void PitTrap::redraw(QPainter& painter, bool highlight, Qt::GlobalColor highlight_color)
{
	if (!_visible)
		return;

    Playground* playground = HeroQuestLevelWindow::_hero_quest->getPlayground();
    QuestBoard* quest_board = playground->getQuestBoard();
    QPixmap* image = playground->getPitTrapImage();
    if (image == 0)
        return;

    PainterContext painter_context(painter);

    const int FIELD_MARGIN = int(ParameterStorage::instance->getFieldSize() / float(11) + 0.5);

    // original image size
    int pit_trap_orig_width = image->width();
    int pit_trap_orig_height = image->height();

    // image size in pixels
    Vec2i pit_trap_corner0 = quest_board->getFieldCorner0(_node_id);
    Vec2i pit_trap_corner3 = quest_board->getFieldCorner3(_node_id);
    int width = pit_trap_corner3.x - pit_trap_corner0.x - FIELD_MARGIN;
    int height = pit_trap_corner3.y - pit_trap_corner0.y - FIELD_MARGIN;

    Vec2i pos_center = (pit_trap_corner3 + pit_trap_corner0) / 2;

    painter.drawPixmap(pos_center.x - int(round(width / float(2))), pos_center.y - int(round(height / float(2))), width,
            height, *image);

    if (highlight)
    {
        QPen pen(highlight_color);
        pen.setWidth(3);
        painter.setPen(pen);

        painter.drawRect(pos_center.x - int(round(width / float(2))), pos_center.y - int(round(height / float(2))),
                width, height);
    }
}

bool PitTrap::isSpearTrap() const
{
	return false;
}

bool PitTrap::save(SaveContext& save_context) const
{
    SaveContext::OpenChapter open_chapter(save_context, "PitTrap");

    Parent::save(save_context);
    return !save_context.fail();
}

bool PitTrap::load(LoadContext& load_context)
{
    LoadContext::OpenChapter open_chapter(load_context, "PitTrap");

    Parent::load(load_context);
    return !load_context.fail();
}

// =============================================================================

SpearTrap::SpearTrap()
:
Trap(),
_redraw_start_time(),
_first_redraw(false)
{
    _visible = false;
}

SpearTrap::SpearTrap(const NodeID& node_id)
:
        Trap(node_id),
_redraw_start_time(),
_first_redraw(true)
{
	_visible = false;
}

SpearTrap::~SpearTrap()
{
	// NIX
}

void SpearTrap::redraw(QPainter& painter, bool /*highlight*/, Qt::GlobalColor /*color*/)
{
	if (!_visible)
		return;

	if (_first_redraw)
	{
	    _redraw_start_time = QTime::currentTime();
	    _first_redraw = false;
	}

    Playground* playground = HeroQuestLevelWindow::_hero_quest->getPlayground();
    QuestBoard* quest_board = playground->getQuestBoard();

	int time_delta_ms = min(_redraw_start_time.elapsed(), int(Level::TRAP_TRIGGER_DELAY));
	// alpha: 1 -> 0 in VANISHING_TIME_MS time
	double alpha = 1 - double(time_delta_ms) / Level::TRAP_TRIGGER_DELAY;

	DV(("time_delta_ms = %d, alpha = %lf", time_delta_ms, alpha));

    QPixmap* image = playground->getSpearTrapImage();

	// --- We assume a square image! ---

	// original image size
	int spear_trap_orig_size = image->width();

    const int FIELD_MARGIN = int(ParameterStorage::instance->getFieldSize() / float(11) + 0.5);
	// destination image size = min(field_width, field_height) - MARGIN
    int spear_trap_size = min(ParameterStorage::instance->getFieldSize(), ParameterStorage::instance->getFieldSize())
            - FIELD_MARGIN;

    Vec2i spear_trap_corner0(quest_board->getFieldCorner0(_node_id));

	float scale = spear_trap_size / float(spear_trap_orig_size);

    painter.save();

    // 1. scale
    QTransform trafo_01_scale;
    trafo_01_scale.scale(scale, scale);
    // 2. translate to draw position
    QTransform trafo_02_translate_to_draw_position;
    trafo_02_translate_to_draw_position.translate(spear_trap_corner0.x, spear_trap_corner0.y);

    QTransform accumulated_transform =
            trafo_01_scale *
            trafo_02_translate_to_draw_position;

    painter.setTransform(accumulated_transform);

#ifdef DEBUG_TRAFO
    cout << "transform: " << accumulated_transform.m11() << " " << accumulated_transform.m12() << " " << accumulated_transform.m13() << endl;
    cout << "           " << accumulated_transform.m21() << " " << accumulated_transform.m22() << " " << accumulated_transform.m23() << endl;
    cout << "           " << accumulated_transform.m31() << " " << accumulated_transform.m32() << " " << accumulated_transform.m33() << endl;
#endif

    painter.setOpacity(alpha);

    painter.drawPixmap(0, 0, *image);

    painter.restore();

    // trigger a follow-up playground redraw until the spear trap has been removed
    HeroQuestLevelWindow::_hero_quest->getPlayground()->update();
}

bool SpearTrap::isSpearTrap() const
{
	return true;
}

bool SpearTrap::save(SaveContext& save_context) const
{
    SaveContext::OpenChapter open_chapter(save_context, "SpearTrap");

    Parent::save(save_context);

    //save_context.write???(stream, _redraw_start_time); // not needed
    save_context.writeBool(_first_redraw, "_first_redraw"); // should be always true here

    return !save_context.fail();
}

bool SpearTrap::load(LoadContext& load_context)
{
    LoadContext::OpenChapter open_chapter(load_context, "SpearTrap");

    Parent::load(load_context);

    //load_context.read???(stream, &_redraw_start_time); // not needed
    load_context.readBool(&_first_redraw, "_first_redraw"); // should be always true here

    return !load_context.fail();
}

// =============================================================================

