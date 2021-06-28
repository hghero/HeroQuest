#include "InventoryItem.h"

#include "SaveContext.h"
#include "LoadContext.h"

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

bool InventoryItem::isEquipmentCard() const
{
    return false;
}

bool InventoryItem::save(SaveContext& save_context) const
{
    return true;
}

bool InventoryItem::load(LoadContext& load_context)
{
    return true;
}
