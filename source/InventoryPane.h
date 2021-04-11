#ifndef INVENTORYPANE_H
#define INVENTORYPANE_H

#include <QtWidgets/QLabel>

class QDialog;
class QScrollArea;
class QBoxLayout;
class Inventory;
class DialogBuyEquipment;

class InventoryPane : public QLabel
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
    // Loot Cards
    //
    //  -------------------------
    // |                         |
    // | LootCardPane            |
    // |                         |
    //  -------------------------
    // |                         |
    // | LootCardPane            |
    // |                         |
    //  -------------------------

    static const QSize MINIMUM_BUTTON_SIZE;

    enum ShowContent {
        TREASURE_CARDS = 1,
        SPELL_CARDS = 2,
        EQUIPMENT_CARDS = 4,
        LOOT_CARDS = 8, ALL = 0xFFFFFFFF
    };

    // TODO: add sort order config

    enum EnableUseButton
    {
        USE_BUTTON_ENABLE = true, USE_BUTTON_DISABLE = false
    };

    enum EnableDisregardButton
    {
        DISCARD_BUTTON_ENABLE = true, DISREGARD_BUTTON_DISABLE = false
    };

    InventoryPane(QDialog* parent, Inventory* inventory, uint show_content, bool enable_use_button,
            bool enable_discard_button, DialogBuyEquipment* dialog_buy_equipment);
    virtual ~InventoryPane();

    void updateInventory(Inventory* inventory, uint show_content, bool enable_use_button,
            bool enable_discard_button, DialogBuyEquipment* dialog_buy_equipment);

    private:
    void addTreasureCards(const Inventory* inventory, bool enable_use_button,
            QScrollArea* scroll_area,
            QBoxLayout* vertical_box_layout,
            int* grid_layout_spacing,
            int* min_width,
            int* min_height,
            int* max_height);
    void addSpellCards(int header_height, const Inventory* inventory, bool enable_use_button, QScrollArea* scroll_area,
            QBoxLayout* vertical_box_layout, int& grid_layout_spacing, int* min_width, int min_height,
            int* max_height);
    void addEquipmentCards(Inventory* inventory, bool enable_use_button, bool enable_discard_button,
            DialogBuyEquipment* dialog_buy_equipment,
            QScrollArea* scroll_area, QBoxLayout* vertical_box_layout, int& grid_layout_spacing, int* min_width,
            int min_height,
            int* max_height);

    QDialog* _dialog;
    QScrollArea* _scroll_area;
};

#endif
