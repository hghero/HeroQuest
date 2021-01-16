#ifndef INVENTORY_ITEM_H
#define INVENTORY_ITEM_H

#include <iostream>

/*!
 * An item storable in a hero's inventory.
 */
class InventoryItem
{
    public:
    InventoryItem();
    virtual ~InventoryItem();

    virtual bool isTreasureCard() const;
    virtual bool isSpellCard() const;

    virtual bool save(std::ostream& stream) const;
    virtual bool load(std::istream& stream);
};

#endif
