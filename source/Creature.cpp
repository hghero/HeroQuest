#include "Creature.h"
#include "Hero.h"
#include <iostream>
#include <QtGui/QPainter>

#include "StreamUtils.h"
#include "Debug.h"
#include "PainterContext.h"

using namespace std;

// ==================================

const QString Creature::ICON_FILENAME = "THIS_STRING_MAY_NOT_BE_USED";

Creature::Creature(const QString& name, int num_dice_attack, int num_dice_defend, int life_points, int life_points_max, int intelligence_points)
: Reference(),
  _created(false),
  _name(name),
  _num_dice_attack(num_dice_attack),
  _num_dice_defend(num_dice_defend),
  _life_points(life_points),
  _life_points_max(life_points_max),
  _intelligence_points(intelligence_points),
  _skip_rounds(0),
  _creature_icon(0), // set in create()
  _creature_icon_orig_size_x(0), // set in create()
  _creature_icon_orig_size_y(0) // set in create()
{
    ;
}


Creature::~Creature()
{
	if (_creature_icon != 0)
	    delete _creature_icon;
}

bool Creature::create()
{
    _creature_icon_orig_size_x = 0;
    _creature_icon_orig_size_y = 0;

    // load icon
    // (note that virtual calling of function "getIconFilename" is NOT possible in the Creature class creator;
    // that would result in the Creature::getIconFilename" being called, which is not intended)
    _creature_icon = new QPixmap(getIconFilename().toUtf8().constData());
    if (!_creature_icon) {
       cout << qPrintable(ICON_FILENAME) << " not found or failed to load" << endl;
       return false;
    }

    _creature_icon_orig_size_x = _creature_icon->width();
    _creature_icon_orig_size_y = _creature_icon->height();

    _created = true;

    return true;
}

const QString& Creature::getName() const
{
	return _name;
}

QPixmap* Creature::getAvatar()
{
	return _creature_icon;
}

const QString& Creature::getIconFilename() const
{
	return ICON_FILENAME;
}

void Creature::redrawCentered(QPainter& painter, const Vec2i& pos_center, int max_width, int max_height, bool highlight,
        Qt::GlobalColor highlight_color)
{
	if (!_created)
	{
		DVX(("Creature not created!"));
		return;
	}

    PainterContext painter_context(painter);

	// determine actual pixel width and height
	float scale_factor = max_width / float(_creature_icon_orig_size_x);
	int width = max_width;
	int height = int(scale_factor * _creature_icon_orig_size_y);
	if (height > max_height)
	{
		scale_factor = max_height / float(_creature_icon_orig_size_y);
		width = int(scale_factor * _creature_icon_orig_size_x);
		height = max_height;
	}

	painter.drawPixmap(pos_center.x - int(round(width / float(2))), pos_center.y - int(round(height / float(2))), width, height, *_creature_icon);

    if (highlight)
    {
        QPen pen(highlight_color);
        pen.setWidth(3);
        painter.setPen(pen);

        painter.drawRect(pos_center.x - int(round(width / float(2))), pos_center.y - int(round(height / float(2))),
                width, height);
    }
}

int Creature::getNumDiceAttack() const
{
	return _num_dice_attack;
}

int Creature::getHighestNumDiceRegularAttack() const
{
    return getNumDiceAttack();
}

void Creature::setNumDiceAttack(int num_dice_attack)
{
	_num_dice_attack = num_dice_attack;
}

int Creature::getNumDiceDefend() const
{
	return _num_dice_defend;
}

int Creature::getHighestNumDiceDefend() const
{
    return getNumDiceDefend();
}

void Creature::setNumDiceDefend(int num_dice_defend)
{
	_num_dice_defend = num_dice_defend;
}

int Creature::getLifePoints() const
{
	return _life_points;
}

void Creature::setLifePoints(int life_points)
{
    life_points = qMin(_life_points_max, life_points);
    life_points = qMax(0, life_points);

	_life_points = life_points;
}

void Creature::addLifePoints(int life_points_add)
{
    int life_points = _life_points;
    life_points += life_points_add;

    life_points = qMin(_life_points_max, life_points);
    life_points = qMax(0, life_points);

    setLifePoints(life_points);
}

int Creature::getIntelligencePoints() const
{
	return _intelligence_points;
}

void Creature::setIntelligencePoints(int intelligence_points)
{
	_intelligence_points = intelligence_points;
}

uint Creature::getSkipRounds() const
{
    return _skip_rounds;
}

void Creature::setSkipRounds(uint skip_rounds)
{
    _skip_rounds = skip_rounds;
}

void Creature::increaseSkipRounds(uint inc_skip_rounds)
{
    _skip_rounds += inc_skip_rounds;
}

void Creature::decreaseSkipRounds(uint dec_skip_rounds)
{
    if (_skip_rounds >= dec_skip_rounds)
        _skip_rounds -= dec_skip_rounds;
    else
        _skip_rounds = 0;
}

//! dummy method in this parent class
bool Creature::isHero() const
{
	return false;
}

//! dummy method in this parent class
bool Creature::isMonster() const
{
	return false;
}

//! dummy method in this parent class
bool Creature::canCastSpells() const
{
	return false;
}

/*!
 * Copies all members but the ones concerning the creature icon.
 *
 * WARNING: ::create must be called afterwards, so that the creature icon is set up.
 */
Creature& Creature::operator=(const Creature& other)
{
	this->_created = other._created;
	this->_name = other._name;
	this->_num_dice_attack = other._num_dice_attack;
	this->_num_dice_defend = other._num_dice_defend;
	this->_life_points = other._life_points;
	this->_intelligence_points = other._intelligence_points;
	return *this;
}

// may never be executed
Creature* Creature::copy() const
{
	cout << "Error: Monster::copy() called" << endl;
	return 0;
}

bool Creature::save(ostream& stream) const
{
    Reference::save(stream);

    StreamUtils::writeBool(stream, _created);

    // name of the creature
    StreamUtils::write(stream, _name);

    // fighting
    StreamUtils::writeInt(stream, _num_dice_attack);
    StreamUtils::writeInt(stream, _num_dice_defend);

    // life and intelligence
    StreamUtils::writeInt(stream, _life_points);
    StreamUtils::writeInt(stream, _life_points_max);
    StreamUtils::writeInt(stream, _intelligence_points);

    // round behaviour
    StreamUtils::writeUInt(stream, _skip_rounds);

    // the creature icon
    // => is restored from program context
    //QPixmap* _creature_icon;
    //int _creature_icon_orig_size_x;
    //int _creature_icon_orig_size_y;

    return !stream.fail();
}

bool Creature::load(istream& stream)
{
    Reference::load(stream);

    StreamUtils::readBool(stream, &_created);

    // name of the creature
    StreamUtils::read(stream, &_name);

    // fighting
    StreamUtils::readInt(stream, &_num_dice_attack);
    StreamUtils::readInt(stream, &_num_dice_defend);

    // life and intelligence
    StreamUtils::readInt(stream, &_life_points);
    StreamUtils::readInt(stream, &_life_points_max);
    StreamUtils::readInt(stream, &_intelligence_points);

    // round behaviour
    StreamUtils::readUInt(stream, &_skip_rounds);

    // the creature icon is restored from program context via ::create
    create();

    return !stream.fail();
}
