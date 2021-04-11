#include "UseInventoryItemButton.h"

#include <QtCore/QString>
#include <QtWidgets/QDialog>

#include "InventoryItem.h"
#include "HeroQuestLevelWindow.h"
#include "EquipmentCardStorage.h"
#include "Inventory.h"
#include "DialogBuyEquipment.h"

UseInventoryItemButton::UseInventoryItemButton(QDialog* parent_dialog, QWidget* parent,
        const InventoryItem& inventory_item, Inventory* inventory, const QString& text)
: QPushButton(text, parent),
  _parent_dialog(parent_dialog),
  _inventory_item(inventory_item), _inventory(inventory)
{
    // NIX
}

UseInventoryItemButton::~UseInventoryItemButton()
{
    // NIX
}

const InventoryItem& UseInventoryItemButton::getInventoryItem() const
{
    return _inventory_item;
}

/*!
 * Called for using spell cards during a level in order to cast a spell.
 * Called for equipment cards during a level in order to e.g. throw a piece of equipment.
 */
void UseInventoryItemButton::use() const
{
    // TreasureCard / SpellCard / EquipmentCard use behaviour
    const InventoryItem& item_copy(_inventory_item); // copy the reference before deleting the dialog
    _parent_dialog->accept();
    HeroQuestLevelWindow::_hero_quest->useInventoryItem(item_copy);
    // update the statistic pane, so that the inventory button is deactivated if no inventory item remains inside
    HeroQuestLevelWindow::_hero_quest->updateHeroStatisticPaneOfCurrentHero();
}

/*!
 * Called from the buy equipment dialog in between two levels.
 * A discarded equipment card will be put back from the selected hero's inventory to the
 * EquipmentCardStorage.
 */
void UseInventoryItemButton::discard()
{
    // EquipmentCard discard behaviour
    if (_inventory_item.isEquipmentCard())
    {
        const EquipmentCard* equipment_card = dynamic_cast<const EquipmentCard*>(&_inventory_item);
        EquipmentCard equipment_card_copy(*equipment_card); // copy before remove

        // remove from selected hero's inventory
        _inventory->removeEquipmentCard(*equipment_card);

        // put it back to the equipment card stock
        EquipmentCardStorage::instance->addEquipmentCardToStock(equipment_card_copy);

        // in DialogBuyEquipment, update the inventory and market place panes
        DialogBuyEquipment* dialog_buy_equipment = dynamic_cast<DialogBuyEquipment*>(_parent_dialog);
        if (dialog_buy_equipment != 0)
        {
            dialog_buy_equipment->updateInventory();
            dialog_buy_equipment->updateEquipmentMarketPlace();
        }
    }
}
