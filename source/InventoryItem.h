#ifndef INVENTORY_ITEM_H
#define INVENTORY_ITEM_H

#include <iostream>

class SaveContext;
class LoadContext;

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
    virtual bool isEquipmentCard() const;

    virtual bool save(SaveContext& save_context) const;
    virtual bool load(LoadContext& load_context);
};

#endif
