//#define HG_DEBUG

#include "Monster.h"

#include <iostream>

#include "HeroQuestLevelWindow.h"
#include "StreamUtils.h"

#include "Debug.h"
#include "Level.h"

using namespace std;

// ==================================

const QString Monster::ICON_FILENAME = "THIS_STRING_MAY_NOT_BE_USED";

Monster::Monster(const QString& name, int num_move_points, int num_dice_attack, int num_dice_defend, int life_points, int life_points_max, int intelligence_points)
:
Creature(name, num_dice_attack, num_dice_defend, life_points, life_points_max, intelligence_points),
_num_move_points(num_move_points)
{
	; // nix
}


Monster::~Monster()
{
    DV(("~Monster: %s", qPrintable(Creature::getName())));
}

const QString& Monster::getIconFilename() const
{
	return ICON_FILENAME;
}

int Monster::getNumMovePoints() const
{
	return _num_move_points;
}

bool Monster::isHero() const
{
	return false;
}

bool Monster::isMonster() const
{
	return true;
}

/*!
 * Copies all members but the ones concerning the creature icon.
 *
 * WARNING: ::create must be called afterwards, so that the creature icon is set up.
 */
Creature& Monster::operator=(const Monster& other)
{
	Creature::operator=(other);
	this->_num_move_points = other._num_move_points;
	return *this;
}

// may never be executed
Creature* Monster::copy() const
{
	cout << "Error: Monster::copy() called" << endl;
	return 0;
}

/*!
 * \return The name of monster.
 * TODO: Warum ist dies statisch?
 */
QString Monster::getName(const Monster* monster)
{
    Monster* monster_non_const = const_cast<Monster*>(monster);

    QString monster_name("Unknown");

    if (dynamic_cast<Goblin*>(monster_non_const) != 0)
        monster_name = "Goblin";
    else if (dynamic_cast<Orc*>(monster_non_const) != 0)
        monster_name = "Orc";
    else if (dynamic_cast<Fimir*>(monster_non_const) != 0)
        monster_name = "Fimir";
    else if (dynamic_cast<Skeleton*>(monster_non_const) != 0)
        monster_name = "Skeleton";
    else if (dynamic_cast<Zombie*>(monster_non_const) != 0)
        monster_name = "Zombie";
    else if (dynamic_cast<Mummy*>(monster_non_const) != 0)
        monster_name = "Mummy";
    else if (dynamic_cast<ChaosWarrior*>(monster_non_const) != 0)
        monster_name = "ChaosWarrior";
    else if (dynamic_cast<Gargoyle*>(monster_non_const) != 0)
        monster_name = "Gargoyle";

    return monster_name;
}

bool Monster::save(ostream& stream) const
{
    Parent::save(stream);

    // movement
    StreamUtils::writeInt(stream, _num_move_points);

    DV(("saved monster with ref id %d", getReferencingID()));

    return !stream.fail();
}

bool Monster::load(istream& stream)
{
    Parent::load(stream);

    // movement
    StreamUtils::readInt(stream, &_num_move_points);

    DV(("loaded monster with ref id %d", getReferencingID()));

    return !stream.fail();
}

// ==================================

const QString Goblin::ICON_FILENAME = ":/graphics/goblin.jpg";

Goblin::Goblin()
:
Monster(
	"Goblin",
	10, // num_move_points
	2, // num_dice_attack
	1, // num_dice_defend
	1, // life_points
	1, // life_points_max
	1  // intelligence_points
)
{
}

Goblin::~Goblin()
{
}

const QString& Goblin::getIconFilename() const
{
	return ICON_FILENAME;
}

/*!
 * Allocates memory for a new Goblin, and copies all members of this monster to the newly
 * allocated one, except the members concerning the creature icon.
 *
 * WARNING: ::create must be called afterwards, so that the creature icon is set up.
 *
 * \return The newly allocated monster.
 */
Creature* Goblin::copy() const
{
	Goblin* monster_copy = new Goblin;
	*monster_copy = *this;
	return monster_copy;
}

// ==================================

const QString Orc::ICON_FILENAME = ":/graphics/orc.jpg";

Orc::Orc()
:
Monster(
	"Orc",
	8, // num_move_points
	3, // num_dice_attack
	2, // num_dice_defend
	1, // life_points
	1, // life_points_max
	2  // intelligence_points
)
{
}

Orc::~Orc()
{
}

const QString& Orc::getIconFilename() const
{
	return ICON_FILENAME;
}

/*!
 * Allocates memory for a new Orc, and copies all members of this monster to the newly
 * allocated one, except the members concerning the creature icon.
 *
 * WARNING: ::create must be called afterwards, so that the creature icon is set up.
 *
 * \return The newly allocated monster.
 */
Creature* Orc::copy() const
{
	Orc* monster_copy = new Orc;
	*monster_copy = *this;
	return monster_copy;
}

// ==================================

const QString Fimir::ICON_FILENAME = ":/graphics/fimir.jpg";

Fimir::Fimir()
:
Monster(
	"Fimir",
	6, // num_move_points
	3, // num_dice_attack
	3, // num_dice_defend
	1, // life_points
	1, // life_points_max
	3  // intelligence_points
)
{
}

Fimir::~Fimir()
{
}

const QString& Fimir::getIconFilename() const
{
	return ICON_FILENAME;
}

/*!
 * Allocates memory for a new Fimir, and copies all members of this monster to the newly
 * allocated one, except the members concerning the creature icon.
 *
 * WARNING: ::create must be called afterwards, so that the creature icon is set up.
 *
 * \return The newly allocated monster.
 */
Creature* Fimir::copy() const
{
	Fimir* monster_copy = new Fimir;
	*monster_copy = *this;
	return monster_copy;
}

// ==================================

const QString Skeleton::ICON_FILENAME = ":/graphics/skeleton.jpg";

Skeleton::Skeleton()
:
Monster(
	"Skeleton",
	6, // num_move_points
	2, // num_dice_attack
	2, // num_dice_defend
	1, // life_points
	1, // life_points_max
	0  // intelligence_points
)
{
}

Skeleton::~Skeleton()
{
}

const QString& Skeleton::getIconFilename() const
{
	return ICON_FILENAME;
}

/*!
 * Allocates memory for a new Skeleton, and copies all members of this monster to the newly
 * allocated one, except the members concerning the creature icon.
 *
 * WARNING: ::create must be called afterwards, so that the creature icon is set up.
 *
 * \return The newly allocated monster.
 */
Creature* Skeleton::copy() const
{
	Skeleton* monster_copy = new Skeleton;
	*monster_copy = *this;
	return monster_copy;
}

// ==================================

const QString Zombie::ICON_FILENAME = ":/graphics/zombie.jpg";

Zombie::Zombie()
:
Monster(
	"Zombie",
	4, // num_move_points
	2, // num_dice_attack
	3, // num_dice_defend
	1, // life_points
	1, // life_points_max
	0  // intelligence_points
)
{
}

Zombie::~Zombie()
{
}

const QString& Zombie::getIconFilename() const
{
	return ICON_FILENAME;
}

/*!
 * Allocates memory for a new Zombie, and copies all members of this monster to the newly
 * allocated one, except the members concerning the creature icon.
 *
 * WARNING: ::create must be called afterwards, so that the creature icon is set up.
 *
 * \return The newly allocated monster.
 */
Creature* Zombie::copy() const
{
	Zombie* monster_copy = new Zombie;
	*monster_copy = *this;
	return monster_copy;
}

// ==================================

const QString Mummy::ICON_FILENAME = ":/graphics/mummy.jpg";

Mummy::Mummy()
:
Monster(
	"Mummy",
	4, // num_move_points
	3, // num_dice_attack
	4, // num_dice_defend
	1, // life_points
	1, // life_points_max
	0  // intelligence_points
)
{
}

Mummy::~Mummy()
{
}

const QString& Mummy::getIconFilename() const
{
	return ICON_FILENAME;
}

/*!
 * Allocates memory for a new Mummy, and copies all members of this monster to the newly
 * allocated one, except the members concerning the creature icon.
 *
 * WARNING: ::create must be called afterwards, so that the creature icon is set up.
 *
 * \return The newly allocated monster.
 */
Creature* Mummy::copy() const
{
	Mummy* monster_copy = new Mummy;
	*monster_copy = *this;
	return monster_copy;
}

// ==================================

const QString ChaosWarrior::ICON_FILENAME = ":/graphics/chaos_warrior.jpg";

ChaosWarrior::ChaosWarrior()
:
Monster(
	"Chaos Warrior",
	6, // num_move_points
	3, // num_dice_attack
	4, // num_dice_defend
	1, // life_points
	1, // life_points_max
	3  // intelligence_points
)
{
}

ChaosWarrior::~ChaosWarrior()
{
}

const QString& ChaosWarrior::getIconFilename() const
{
	return ICON_FILENAME;
}

/*!
 * Allocates memory for a new ChaosWarrior, and copies all members of this monster to the newly
 * allocated one, except the members concerning the creature icon.
 *
 * WARNING: ::create must be called afterwards, so that the creature icon is set up.
 *
 * \return The newly allocated monster.
 */
Creature* ChaosWarrior::copy() const
{
	ChaosWarrior* monster_copy = new ChaosWarrior;
	*monster_copy = *this;
	return monster_copy;
}

// ==================================

const QString Gargoyle::ICON_FILENAME = ":/graphics/gargoyle.jpg";

Gargoyle::Gargoyle()
:
Monster(
	"Gargoyle",
	6, // num_move_points
	4, // num_dice_attack
	4, // num_dice_defend
	1, // life_points
	1, // life_points_max
	4  // intelligence_points
)
{
}

Gargoyle::~Gargoyle()
{
}

const QString& Gargoyle::getIconFilename() const
{
	return ICON_FILENAME;
}

/*!
 * Allocates memory for a new Gargoyle, and copies all members of this monster to the newly
 * allocated one, except the members concerning the creature icon.
 *
 * WARNING: ::create must be called afterwards, so that the creature icon is set up.
 *
 * \return The newly allocated monster.
 */
Creature* Gargoyle::copy() const
{
	Gargoyle* monster_copy = new Gargoyle;
	*monster_copy = *this;
	return monster_copy;
}

// ==================================
