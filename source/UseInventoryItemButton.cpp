#include "UseInventoryItemButton.h"

#include <QtCore/QString>

#include "InventoryItem.h"
#include "DialogInventory.h"
#include "HeroQuestLevelWindow.h"

UseInventoryItemButton::UseInventoryItemButton(DialogInventory* dialog_inventory, QWidget* parent, const InventoryItem& inventory_item, const QString& text)
: QPushButton(text, parent),
  _dialog_inventory(dialog_inventory),
  _inventory_item(inventory_item)
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

void UseInventoryItemButton::use() const
{
    const InventoryItem& item_copy(_inventory_item); // copy the reference before deleting the dialog
    _dialog_inventory->accept();
    HeroQuestLevelWindow::_hero_quest->useInventoryItem(item_copy);
    // update the statistic pane, so that the inventory button is deactivated if no inventory item remains inside
    HeroQuestLevelWindow::_hero_quest->updateHeroStatisticPaneOfCurrentHero();
}
