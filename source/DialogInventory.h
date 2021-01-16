#ifndef DIALOG_INVENTORY_H
#define DIALOG_INVENTORY_H

#include <QtWidgets/QDialog>

#include "TreasureCardPane.h"
#include "SpellCardPane.h"
#include "Inventory.h"


class DialogInventory : public QDialog
{
    Q_OBJECT

public:
	//
    // Treasure Cards
    //
    //  ------------------------- ---------
    // |                         |         |
    // | TreasureCardPane        |         |
    // |                         |   Use   |
	//  ------------------------- ---------
    // |                         |         |
    // | TreasureCardPane        |         |
    // |                         |   Use   |
    //  ------------------------- ---------
    //
    // Spell Cards
    //
    //  ------------------------- ---------
    // |                         |         |
    // | SpellCardPane           |         |
    // |                         |   Use   |
    //  ------------------------- ---------
    // |                         |         |
    // | SpellCardPane           |         |
    // |                         |   Use   |
    //  ------------------------- ---------
    //
    // Equipment Cards
    //
    //  ------------------------- ---------
    // |                         |         |
    // | EquipmentCardPane       |         |
    // |                         |  (Use)  |
    //  ------------------------- ---------
    // |                         |         |
    // | EquipmentCardPane       |         |
    // |                         |  (Use)  |
    //  ------------------------- ---------
    //
    // Booty Cards
    //
    //  -------------------------
    // |                         |
    // | BootyCardPane           |
    // |                         |
    //  -------------------------
    // |                         |
    // | BootyCardPane           |
    // |                         |
    //  -------------------------

    static const QSize MINIMUM_BUTTON_SIZE;

    enum ModeFlag {
        TREASURE_CARDS = 1,
        SPELL_CARDS = 2,
        EQUIPMENT_CARDS = 4,
        BOOTY_CARDS = 8
    };

	DialogInventory(uint mode_flag = 0xFFFFFFFF);

	virtual ~DialogInventory();

	Inventory::ConstIterator getChosenInventoryItem();

private:
	//const Hero* _hero;
	Inventory::ConstIterator _inventory_it;

	//TreasureCardPane* _inventory_panel;
};

#endif
