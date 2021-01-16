#include "NonWalkableDecoration.h"

#include <vector>

#include "Level.h"
#include "QuestBoard.h"
#include "Playground.h"
#include "StreamUtils.h"


using namespace std;


// =============================================================================

NonWalkableDecoration::NonWalkableDecoration()
: Decoration()
{
	// NIX
}

NonWalkableDecoration::NonWalkableDecoration(const NodeID& node_top_left, const NodeID& node_bottom_right, const Orientation& orientation)
: Decoration(node_top_left, node_bottom_right, orientation)
{
	// NIX
}

NonWalkableDecoration::~NonWalkableDecoration()
{
	// NIX
}

QPixmap* NonWalkableDecoration::getImage() const
{
	cout << "Warning: NonWalkableDecoration::getImage() called" << endl;
	return 0;
}

bool NonWalkableDecoration::isWalkable() const
{
	return false;
}

bool NonWalkableDecoration::save(std::ostream& stream) const
{
    return Parent::save(stream);
}

bool NonWalkableDecoration::load(std::istream& stream)
{
    return Parent::load(stream);
}

// =============================================================================

Chest::Chest()
        :
        NonWalkableDecoration(), _open(false)
{
}

Chest::Chest(const NodeID& node_top_left, const NodeID& node_bottom_right, const Orientation& orientation)
        :
        NonWalkableDecoration(node_top_left, node_bottom_right, orientation), _open(false)
{
}

Chest::Chest(const NodeID& node_top_left, const Orientation& orientation)
: NonWalkableDecoration(
	node_top_left,
	node_top_left,
    orientation), _open(false)
{
}

Chest::~Chest()
{
	// NIX
}

QPixmap* Chest::getImage() const
{
	return HeroQuestLevelWindow::_hero_quest->getPlayground()->getChestImage();
}

bool Chest::isOpen() const
{
    return _open;
}

void Chest::setOpen(bool value)
{
    _open = value;
}

bool Chest::save(std::ostream& stream) const
{
    if (!Parent::save(stream))
    {
        DVX(("ERROR: Chest::save"));
        return false;
    }

    return StreamUtils::writeBool(stream, _open);
}

bool Chest::load(std::istream& stream)
{
    if (!Parent::load(stream))
    {
        DVX(("ERROR: Chest::load"));
        return false;
    }

    return StreamUtils::readBool(stream, &_open);
}

// =============================================================================

AlchemistTable::AlchemistTable()
: NonWalkableDecoration()
{
	// NIX
}

AlchemistTable::AlchemistTable(const NodeID& node_top_left, const NodeID& node_bottom_right, const Orientation& orientation)
: NonWalkableDecoration(node_top_left, node_bottom_right, orientation)
{
	// NIX
}

AlchemistTable::AlchemistTable(const NodeID& node_top_left, const Orientation& orientation)
: NonWalkableDecoration(
	node_top_left,
	node_top_left + (orientation == TURN_0 || orientation == TURN_180 ? NodeID(2, 1) : NodeID(1, 2)),
	orientation)
{
	// NIX
}

AlchemistTable::~AlchemistTable()
{
	// NIX
}

QPixmap* AlchemistTable::getImage() const
{
	return HeroQuestLevelWindow::_hero_quest->getPlayground()->getAlchemistTableImage();
}

// =============================================================================

Armory::Armory()
: NonWalkableDecoration()
{
	// NIX
}

Armory::Armory(const NodeID& node_top_left, const NodeID& node_bottom_right, const Orientation& orientation)
: NonWalkableDecoration(node_top_left, node_bottom_right, orientation)
{
	// NIX
}

Armory::Armory(const NodeID& node_top_left, const Orientation& orientation)
: NonWalkableDecoration(
	node_top_left,
	node_top_left + (orientation == TURN_0 || orientation == TURN_180 ? NodeID(2, 0) : NodeID(0, 2)),
	orientation)
{
	// NIX
}

Armory::~Armory()
{
	// NIX
}

QPixmap* Armory::getImage() const
{
	return HeroQuestLevelWindow::_hero_quest->getPlayground()->getArmoryImage();
}

// =============================================================================

Cupboard::Cupboard()
: NonWalkableDecoration()
{
	// NIX
}

Cupboard::Cupboard(const NodeID& node_top_left, const NodeID& node_bottom_right, const Orientation& orientation)
: NonWalkableDecoration(node_top_left, node_bottom_right, orientation)
{
	// NIX
}

Cupboard::Cupboard(const NodeID& node_top_left, const Orientation& orientation)
: NonWalkableDecoration(
	node_top_left,
	node_top_left + (orientation == TURN_0 || orientation == TURN_180 ? NodeID(2, 0) : NodeID(0, 2)),
	orientation)
{
	// NIX
}

Cupboard::~Cupboard()
{
	// NIX
}

QPixmap* Cupboard::getImage() const
{
	return HeroQuestLevelWindow::_hero_quest->getPlayground()->getCupboardImage();
}

// =============================================================================

Fireplace::Fireplace()
: NonWalkableDecoration()
{
	// NIX
}

Fireplace::Fireplace(const NodeID& node_top_left, const NodeID& node_bottom_right, const Orientation& orientation)
: NonWalkableDecoration(node_top_left, node_bottom_right, orientation)
{
	// NIX
}

Fireplace::Fireplace(const NodeID& node_top_left, const Orientation& orientation)
: NonWalkableDecoration(
	node_top_left,
	node_top_left + (orientation == TURN_0 || orientation == TURN_180 ? NodeID(2, 0) : NodeID(0, 2)),
	orientation)
{
	// NIX
}

Fireplace::~Fireplace()
{
	// NIX
}

QPixmap* Fireplace::getImage() const
{
	return HeroQuestLevelWindow::_hero_quest->getPlayground()->getFireplaceImage();
}

// =============================================================================

Sarcophagus::Sarcophagus()
: NonWalkableDecoration()
{
	// NIX
}

Sarcophagus::Sarcophagus(const NodeID& node_top_left, const NodeID& node_bottom_right, const Orientation& orientation)
: NonWalkableDecoration(node_top_left, node_bottom_right, orientation)
{
	// NIX
}

Sarcophagus::Sarcophagus(const NodeID& node_top_left, const Orientation& orientation)
: NonWalkableDecoration(
	node_top_left,
	node_top_left + (orientation == TURN_0 || orientation == TURN_180 ? NodeID(1, 2) : NodeID(2, 1)),
	orientation)
{
	// NIX
}

Sarcophagus::~Sarcophagus()
{
	// NIX
}

QPixmap* Sarcophagus::getImage() const
{
	return HeroQuestLevelWindow::_hero_quest->getPlayground()->getSarcophagusImage();
}

// =============================================================================

Table::Table()
: NonWalkableDecoration()
{
	// NIX
}

Table::Table(const NodeID& node_top_left, const NodeID& node_bottom_right, const Orientation& orientation)
: NonWalkableDecoration(node_top_left, node_bottom_right, orientation)
{
	// NIX
}

Table::Table(const NodeID& node_top_left, const Orientation& orientation)
: NonWalkableDecoration(
	node_top_left,
	node_top_left + (orientation == TURN_0 || orientation == TURN_180 ? NodeID(2, 1) : NodeID(1, 2)),
	orientation)
{
	// NIX
}

Table::~Table()
{
	// NIX
}

QPixmap* Table::getImage() const
{
	return HeroQuestLevelWindow::_hero_quest->getPlayground()->getTableImage();
}

// =============================================================================

Throne::Throne()
: NonWalkableDecoration()
{
	// NIX
}

Throne::Throne(const NodeID& node_top_left, const NodeID& node_bottom_right, const Orientation& orientation)
: NonWalkableDecoration(node_top_left, node_bottom_right, orientation)
{
	// NIX
}

Throne::Throne(const NodeID& node_top_left, const Orientation& orientation)
: NonWalkableDecoration(
	node_top_left,
	node_top_left,
	orientation)
{
	// NIX
}

Throne::~Throne()
{
	// NIX
}

QPixmap* Throne::getImage() const
{
	return HeroQuestLevelWindow::_hero_quest->getPlayground()->getThroneImage();
}

// =============================================================================

TortureRack::TortureRack()
: NonWalkableDecoration()
{
	// NIX
}

TortureRack::TortureRack(const NodeID& node_top_left, const NodeID& node_bottom_right, const Orientation& orientation)
: NonWalkableDecoration(node_top_left, node_bottom_right, orientation)
{
	// NIX
}

TortureRack::TortureRack(const NodeID& node_top_left, const Orientation& orientation)
: NonWalkableDecoration(
	node_top_left,
	node_top_left + (orientation == TURN_0 || orientation == TURN_180 ? NodeID(1, 2) : NodeID(2, 1)),
	orientation)
{
	// NIX
}

TortureRack::~TortureRack()
{
	// NIX
}

QPixmap* TortureRack::getImage() const
{
	return HeroQuestLevelWindow::_hero_quest->getPlayground()->getTortureRackImage();
}

// =============================================================================

WarlockTable::WarlockTable()
: NonWalkableDecoration()
{
	// NIX
}

WarlockTable::WarlockTable(const NodeID& node_top_left, const NodeID& node_bottom_right, const Orientation& orientation)
: NonWalkableDecoration(node_top_left, node_bottom_right, orientation)
{
	// NIX
}

WarlockTable::WarlockTable(const NodeID& node_top_left, const Orientation& orientation)
: NonWalkableDecoration(
	node_top_left,
	node_top_left + (orientation == TURN_0 || orientation == TURN_180 ? NodeID(2, 1) : NodeID(1, 2)),
	orientation)
{
	// NIX
}

WarlockTable::~WarlockTable()
{
	// NIX
}

QPixmap* WarlockTable::getImage() const
{
	return HeroQuestLevelWindow::_hero_quest->getPlayground()->getWarlockTableImage();
}

// =============================================================================

Debris::Debris()
: NonWalkableDecoration()
{
	// NIX
}

Debris::Debris(const NodeID& node_top_left, const NodeID& node_bottom_right, const Orientation& orientation)
: NonWalkableDecoration(node_top_left, node_bottom_right, orientation)
{
	// NIX
}

Debris::Debris(const NodeID& node_top_left, const Orientation& orientation)
: NonWalkableDecoration(
	node_top_left,
	node_top_left,
	orientation)
{
	// NIX
}

Debris::~Debris()
{
	// NIX
}

QPixmap* Debris::getImage() const
{
	return HeroQuestLevelWindow::_hero_quest->getPlayground()->getDebrisImage();
}

// =============================================================================
