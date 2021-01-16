#include "Trap.h"

#include <iostream>
#include <QtGui/QPainter>
#include <QtCore/QString>

#include "QuestBoard.h"
#include "Playground.h"
#include "Debug.h"
#include "HeroQuestLevelWindow.h"
#include "Level.h"
#include "StreamUtils.h"

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

void Trap::redraw(QPainter& /*painter*/)
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

bool Trap::save(ostream& stream) const
{
    StreamUtils::writeBool(stream, _visible);

    _node_id.save(stream);

    return !stream.fail();
}

bool Trap::load(istream& stream)
{
    StreamUtils::readBool(stream, &_visible);

    _node_id.load(stream);

    return !stream.fail();
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

void PitTrap::redraw(QPainter& painter)
{
	if (!_visible)
		return;

    Playground* playground = HeroQuestLevelWindow::_hero_quest->getPlayground();
    QuestBoard* quest_board = playground->getQuestBoard();
    QPixmap* image = playground->getPitTrapImage();
    if (image == 0)
        return;

	// --- We assume a square image! ---

	// original image size
	int pit_trap_orig_size = image->width();

    const int FIELD_MARGIN = int(quest_board->getFieldWidth() / float(11) + 0.5);
	// destination image size = min(field_width, field_height) - MARGIN
    int pit_trap_size = min(quest_board->getFieldWidth(), quest_board->getFieldHeight()) - FIELD_MARGIN;

    Vec2i pit_trap_corner0 = quest_board->getFieldCorner0(_node_id) + Vec2i(FIELD_MARGIN / 2, FIELD_MARGIN / 2);

	float scale = pit_trap_size / float(pit_trap_orig_size);

	painter.save();

	// 1. scale
	QTransform trafo_01_scale;
	trafo_01_scale.scale(scale, scale);
	// 2. translate to draw position
	QTransform trafo_02_translate_to_draw_position;
	trafo_02_translate_to_draw_position.translate(pit_trap_corner0.x, pit_trap_corner0.y);

    QTransform accumulated_transform =
            trafo_01_scale *
            trafo_02_translate_to_draw_position;

	painter.setTransform(accumulated_transform);

#ifdef DEBUG_TRAFO
	cout << "transform: " << accumulated_transform.m11() << " " << accumulated_transform.m12() << " " << accumulated_transform.m13() << endl;
	cout << "           " << accumulated_transform.m21() << " " << accumulated_transform.m22() << " " << accumulated_transform.m23() << endl;
	cout << "           " << accumulated_transform.m31() << " " << accumulated_transform.m32() << " " << accumulated_transform.m33() << endl;
#endif

	painter.drawPixmap(0, 0, *image);

	painter.restore();
}

bool PitTrap::isSpearTrap() const
{
	return false;
}

bool PitTrap::save(std::ostream& stream) const
{
    Parent::save(stream);
    return !stream.fail();
}

bool PitTrap::load(std::istream& stream)
{
    Parent::load(stream);
    return !stream.fail();
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

void SpearTrap::redraw(QPainter& painter)
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

    const int FIELD_MARGIN = int(quest_board->getFieldWidth() / float(11) + 0.5);
	// destination image size = min(field_width, field_height) - MARGIN
    int spear_trap_size = min(quest_board->getFieldWidth(), quest_board->getFieldHeight()) - FIELD_MARGIN;

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

bool SpearTrap::save(std::ostream& stream) const
{
    Parent::save(stream);

    //StreamUtils::write???(stream, _redraw_start_time); // not needed
    StreamUtils::writeBool(stream, _first_redraw); // should be always true here

    return !stream.fail();
}

bool SpearTrap::load(std::istream& stream)
{
    Parent::load(stream);

    //StreamUtils::read???(stream, &_redraw_start_time); // not needed
    StreamUtils::readBool(stream, &_first_redraw); // should be always true here

    return !stream.fail();
}

// =============================================================================

