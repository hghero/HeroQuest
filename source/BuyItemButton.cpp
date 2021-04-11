#include "BuyItemButton.h"

#include <QtCore/QString>
#include <QtWidgets/QDialog>

#include "EquipmentCard.h"
#include "HeroQuestLevelWindow.h"
#include "EquipmentCardStorage.h"
#include "Inventory.h"
#include "DialogBuyEquipment.h"

BuyItemButton::BuyItemButton(DialogBuyEquipment* dialog_buy_equipment, QWidget* parent,
        const EquipmentCard& equipment_card, Inventory* inventory, const QString& text)
: QPushButton(text, parent),
  _dialog_buy_equipment(dialog_buy_equipment), _equipment_card(equipment_card), _inventory(
                inventory)
{
}

BuyItemButton::~BuyItemButton()
{
}

/*!
 * Called for equipment cards in between two levels in order to buy the card and assign it to a hero's inventory.
 */
void BuyItemButton::buy()
{
    EquipmentCard equipment_card(_equipment_card); // copy before remove

    // remove from EquipmentCardStorage
    EquipmentCardStorage::instance->removeEquipmentCardFromStock(equipment_card);

    // add to hero's inventory
    _inventory->addEquipmentCard(equipment_card);

    // pay for the card
    _inventory->subtractGold(equipment_card.getCost());

    // update:
    // - the inventory pane
    _dialog_buy_equipment->updateInventory();
    // - the market place
    _dialog_buy_equipment->updateEquipmentMarketPlace();
    // - the hero's textlabel showing the amount of gold pieces
    _dialog_buy_equipment->updateHeroGoldLabelOfCurrentlyChosenHero();
}
