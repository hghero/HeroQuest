#include "InventoryItem.h"

//using namespace std;


InventoryItem::InventoryItem()
{
	// NIX
}

InventoryItem::~InventoryItem()
{
	// NIX
}

bool InventoryItem::isTreasureCard() const
{
    return false;
}

bool InventoryItem::isSpellCard() const
{
    return false;
}

bool InventoryItem::save(std::ostream& stream) const
{
    return true;
}

bool InventoryItem::load(std::istream& stream)
{
    return true;
}
