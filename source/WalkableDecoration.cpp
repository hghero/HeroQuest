#include "WalkableDecoration.h"

#include <vector>

#include "Level.h"
#include "QuestBoard.h"
#include "Playground.h"


using namespace std;


// =============================================================================

WalkableDecoration::WalkableDecoration()
: Decoration()
{
	// NIX
}

WalkableDecoration::WalkableDecoration(const NodeID& node_top_left, const NodeID& node_bottom_right, const Orientation& orientation)
: Decoration(node_top_left, node_bottom_right, orientation)
{
	// NIX
}

WalkableDecoration::~WalkableDecoration()
{
	// NIX
}

QPixmap* WalkableDecoration::getImage() const
{
	cout << "Warning: WalkableDecoration::getImage() called" << endl;
	return 0;
}

bool WalkableDecoration::isWalkable() const
{
	return true;
}

// =============================================================================

Stairs::Stairs()
: WalkableDecoration()
{
	// NIX
}

Stairs::Stairs(const NodeID& node_top_left, const NodeID& node_bottom_right, const Orientation& orientation)
: WalkableDecoration(node_top_left, node_bottom_right, orientation)
{
	// NIX
}

Stairs::Stairs(const NodeID& node_top_left, const Orientation& orientation)
: WalkableDecoration(
	node_top_left,
	node_top_left + NodeID(1, 1),
	orientation)
{
	// NIX
}

Stairs::~Stairs()
{
	// NIX
}

QPixmap* Stairs::getImage() const
{
	return HeroQuestLevelWindow::_hero_quest->getPlayground()->getStairsImage();
}

// =============================================================================
