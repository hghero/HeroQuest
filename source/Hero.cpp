#include <iostream>

#include "Hero.h"

#include "HeroQuestLevelWindow.h"
#include "Level.h"
#include "StreamUtils.h"

using namespace std;

// ==================================

const QString Hero::ICON_FILENAME = "THIS_STRING_MAY_NOT_BE_USED";

Hero::Hero(const QString& name, int num_dice_move, int num_dice_attack, int num_dice_defend, int life_points, int life_points_max, int intelligence_points, bool add_to_level)
:
Creature(name, num_dice_attack, num_dice_defend, life_points, life_points_max, intelligence_points),
_num_dice_move(num_dice_move),
_num_dice_move_extra(0),
 _can_move_through_walls(false),
_num_dice_attack_extra(0),
_num_dice_defend_extra(0),
_inventory()
{
    if (add_to_level)
    {
        // create statistic pane
        HeroQuestLevelWindow::_hero_quest->addHeroStatisticPane(this);
    }
}


Hero::~Hero()
{
	; // nix
}

const QString& Hero::getIconFilename() const
{
	return ICON_FILENAME;
}

int Hero::getNumDiceMove() const
{
	return _num_dice_move + _num_dice_move_extra;
}

void Hero::setNumDiceMoveExtra(int num_dice_move_extra)
{
	_num_dice_move_extra = num_dice_move_extra;
}

bool Hero::getCanMoveThroughWalls() const
{
    return _can_move_through_walls;
}

void Hero::setCanMoveThroughWalls(bool value)
{
    _can_move_through_walls = value;
}

int Hero::getNumDiceAttack() const
{
    return _num_dice_attack + _num_dice_attack_extra;
}

void Hero::setNumDiceAttackExtra(int num_dice_attack_extra)
{
    _num_dice_attack_extra = num_dice_attack_extra;
}

int Hero::getNumDiceDefend() const
{
    return _num_dice_defend + _num_dice_defend_extra;
}

void Hero::setNumDiceDefendExtra(int num_dice_defend_extra)
{
    _num_dice_defend_extra = num_dice_defend_extra;
}

// deprecated
int Hero::getGold() const
{
	return _inventory.getGold();
}

// deprecated
void Hero::setGold(int gold)
{
	_inventory.setGold(gold);
}

Inventory& Hero::getInventory()
{
	return _inventory;
}

const Inventory& Hero::getInventory() const
{
	return _inventory;
}

bool Hero::isHero() const
{
	return true;
}

bool Hero::isMonster() const
{
	return false;
}

bool Hero::canCastSpells() const
{
    return false;
}

/*!
 * This is called when a spell is applied on the Hero. If the hero has a RESISTANCE_POTION treasure card,
 * he is asked whether he wants to use it and this render the spell ineffective.
 */
bool Hero::checkApplySpell()
{
    return HeroQuestLevelWindow::_hero_quest->checkApplySpell(this);
}

/*!
 * This is called when the Hero defends himself. If he has an IMMUNIZATION_POTION treasure card,
 * he is asked if he wants to drink it before throwing the defend dice.
 *
 * \return Always true.
 */
bool Hero::checkDefend()
{
    return HeroQuestLevelWindow::_hero_quest->checkDefend(this);
}

/*!
 * \return True if the hero really dies, false otherwise.
 */
bool Hero::checkDeath()
{
    return HeroQuestLevelWindow::_hero_quest->checkDeath(this);
}

//! It is forbidden to copy Heroes.
Creature& Hero::operator=(const Hero& other)
{
	return *this;
}

//! It is forbidden to copy Heroes.
Creature* Hero::copy() const
{
	cout << "Error: Hero::copy() called" << endl;
	return 0;
}

bool Hero::save(ostream& stream) const
{
    Parent::save(stream);

    // movement
    StreamUtils::writeInt(stream, _num_dice_move);
    StreamUtils::writeInt(stream, _num_dice_move_extra);
    StreamUtils::writeBool(stream, _can_move_through_walls);

    // attack
    StreamUtils::writeInt(stream, _num_dice_attack_extra);

    // defend
    StreamUtils::writeInt(stream, _num_dice_defend_extra);

    // equipment and valuables
    _inventory.save(stream);

    return !stream.fail();
}

bool Hero::load(istream& stream)
{
    Parent::load(stream);

    // movement
    StreamUtils::readInt(stream, &_num_dice_move);
    StreamUtils::readInt(stream, &_num_dice_move_extra);
    StreamUtils::readBool(stream, &_can_move_through_walls);

    // attack
    StreamUtils::readInt(stream, &_num_dice_attack_extra);

    // defend
    StreamUtils::readInt(stream, &_num_dice_defend_extra);

    // equipment and valuables
    _inventory.load(stream);

    return !stream.fail();
}

/*!
 * Called after save game loading: updates treasure card images in
 * std::set<TreasureCard> _treasure_cards (in inventory).
 */
void Hero::updateTreasureImages()
{
    _inventory.updateTreasureImages();
}

// ==================================

const QString Barbarian::ICON_FILENAME = ":/graphics/barbarian.jpg";

Barbarian::Barbarian(bool add_to_level)
:
Hero(
	Barbarian::className(),
	2, // num_dice_move
	3, // num_dice_attack
	2, // num_dice_defend
	8, // life_points
	8, // life_points_max
	2, // intelligence_points
	add_to_level)
{
}

Barbarian::~Barbarian()
{
}

const QString& Barbarian::getIconFilename() const
{
	return ICON_FILENAME;
}

QString Barbarian::className()
{
    return "Barbarian";
}

// ==================================

const QString Dwarf::ICON_FILENAME = ":/graphics/dwarf.jpg";

Dwarf::Dwarf(bool add_to_level)
:
Hero(
	Dwarf::className(),
	2, // num_dice_move
	2, // num_dice_attack
	2, // num_dice_defend
	7, // life_points
	7, // life_points_max
	3, // intelligence_points
	add_to_level)
{
}

Dwarf::~Dwarf()
{
}

const QString& Dwarf::getIconFilename() const
{
	return ICON_FILENAME;
}

QString Dwarf::className()
{
    return "Dwarf";
}

// ==================================

const QString Alb::ICON_FILENAME = ":/graphics/alb.jpg";

Alb::Alb(bool add_to_level)
:
Hero(
	Alb::className(),
	2, // num_dice_move
	2, // num_dice_attack
	2, // num_dice_defend
	6, // life_points
	6, // life_points_max
	4, // intelligence_points
	add_to_level),
_spell_family(SpellCard::AIR)
{
}

Alb::~Alb()
{
}

const QString& Alb::getIconFilename() const
{
	return ICON_FILENAME;
}

bool Alb::canCastSpells() const
{
	return true;
}

void Alb::setSpellFamily(SpellCard::SpellFamily& family)
{
    _spell_family = family;
}

const SpellCard::SpellFamily& Alb::getSpellFamily() const
{
    return _spell_family;
}

QString Alb::className()
{
    return "Alb";
}

// ==================================

const QString Magician::ICON_FILENAME = ":/graphics/magician.jpg";

Magician::Magician(bool add_to_level)
:
Hero(
	Magician::className(),
	2, // num_dice_move
	1, // num_dice_attack
	2, // num_dice_defend
	4, // life_points
	4, // life_points_max
	6, // intelligence_points
	add_to_level)
{
}

Magician::~Magician()
{
}

const QString& Magician::getIconFilename() const
{
	return ICON_FILENAME;
}

bool Magician::canCastSpells() const
{
	return true;
}

QString Magician::className()
{
    return "Magician";
}

// ==================================
