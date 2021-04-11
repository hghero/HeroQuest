#include "InventoryPane.h"

//#define DEBUG_INVENTORY_PANE

#include <set>
#include <iostream>

#include <QtWidgets/QDialog>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QScrollBar>
#include <QtWidgets/QStyle>

#include "HeroQuestLevelWindow.h"
#include "Inventory.h"
#include "Level.h"
#include "TreasureCard.h"
#include "TreasureCardPane.h"
#include "SpellCardPane.h"
#include "EquipmentCardPane.h"
#include "UseInventoryItemButton.h"
#include "Debug.h"
#include "DialogBuyEquipment.h"

using namespace std;

const QSize InventoryPane::MINIMUM_BUTTON_SIZE(100, 25);

/*
 * InventoryPane
 *   scroll_label
 *
 *     treasure_cards_label
 *     grid_label, grid_layout
 *       treasure_card_pane  |  button
 *       treasure_card_pane  |  button
 *       ...
 *
 *     spell_cards_label
 *     grid_label, grid_layout
 *       spell_card_pane  |  button
 *       spell_card_pane  |  button
 *       ...
 *
 *     equipment_cards_label
 *     grid_label, grid_layout
 *       equipment_card_pane  |  button
 *       equipment_card_pane  |  button
 *       ...
 */
InventoryPane::InventoryPane(QDialog* parent, Inventory* inventory, uint show_content, bool enable_use_button,
        bool enable_discard_button, DialogBuyEquipment* dialog_buy_equipment)
        :
        QLabel(parent), _dialog(parent), _scroll_area(0)
{
    setStyleSheet(HeroQuestLevelWindow::DIALOG_STYLE);
    setContentsMargins(0, 0, 0, 0);

    QBoxLayout* outer_vertical_box_layout = new QBoxLayout(QBoxLayout::TopToBottom);
    outer_vertical_box_layout->setSpacing(10);
    setLayout(outer_vertical_box_layout);

    _scroll_area = new QScrollArea(this);
    _scroll_area->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    _scroll_area->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    _scroll_area->setContentsMargins(0, 0, 0, 0);
    _scroll_area->setStyleSheet("border: none ;");
    outer_vertical_box_layout->addWidget(_scroll_area);

    updateInventory(inventory, show_content, enable_use_button, enable_discard_button, dialog_buy_equipment);
#ifdef DEBUG_INVENTORY_PANE
    DVX(("_scroll_area: %d, %d", _scroll_area->width(), _scroll_area->height()));
#endif
}

InventoryPane::~InventoryPane()
{
    // NIX
}

/*!
 * Removes any existing widget from the _scroll_area, and adds a scroll_label to it, showing
 * the inventory items.
 *
 * @param inventory
 * @param show_content
 * @param enable_use_button
 */
void InventoryPane::updateInventory(Inventory* inventory, uint show_content, bool enable_use_button,
        bool enable_discard_button, DialogBuyEquipment* dialog_buy_equipment)
{
    // --- delete the scroll_label, if existing ---
    QWidget* old_scroll_label = _scroll_area->widget();
    _scroll_area->setWidget(0);
    delete old_scroll_label;

    // --- rebuild the scroll_label and everything contained by it ---
    bool show_treasure_cards = show_content & TREASURE_CARDS;
    bool show_spell_cards = show_content & SPELL_CARDS;
    bool show_equipment_cards = show_content & EQUIPMENT_CARDS;
    bool show_loot_cards = show_content & LOOT_CARDS;

    int min_width = 0;
    int max_width = 0;
    int min_height = 0;
    int max_height = 0;
    int header_height = 0;
    int grid_layout_spacing = 0;
    const int ADDITIONAL_SPACE_FOR_PANE = 10; // without this additional space, the pane would be some pixels to narrow; I didn't yet understand why...

    QLabel* scroll_label = new QLabel();
    _scroll_area->setWidget(scroll_label);

    QBoxLayout* vertical_box_layout = new QBoxLayout(QBoxLayout::TopToBottom);
    vertical_box_layout->setContentsMargins(0, 0, 0, 0);
    scroll_label->setLayout(vertical_box_layout);

    if (show_treasure_cards && !inventory->getTreasureCards().empty())
    {
        addTreasureCards(inventory, enable_use_button, _scroll_area, vertical_box_layout,
                &grid_layout_spacing,
                &min_width, &min_height, &max_height);
    }

    if (show_spell_cards && !inventory->getSpellCards().empty())
    {
        addSpellCards(header_height, inventory, enable_use_button, _scroll_area, vertical_box_layout,
                grid_layout_spacing, &min_width, min_height, &max_height);
    }

    if (show_equipment_cards && !inventory->getEquipmentCards().empty())
    {
        addEquipmentCards(inventory, enable_use_button, enable_discard_button, dialog_buy_equipment, _scroll_area,
                vertical_box_layout, grid_layout_spacing,
                &min_width, min_height, &max_height);
    }

    _scroll_area->setMinimumSize(QSize(min_width, min_height + header_height + grid_layout_spacing));

    scroll_label->setFixedWidth(_scroll_area->width());
    scroll_label->setFixedHeight(max_height);

    setMinimumWidth(min_width + ADDITIONAL_SPACE_FOR_PANE);

#ifdef DEBUG_INVENTORY_PANE
    DVX(("scroll_label: %d, %d", scroll_label->width(), scroll_label->height()));
#endif
}

void InventoryPane::addTreasureCards(const Inventory* inventory,
        bool enable_use_button, QScrollArea* scroll_area,
        QBoxLayout* vertical_box_layout,
        int* grid_layout_spacing, int* min_width,
        int* min_height,
        int* max_height)
{
    QLabel* treasure_cards_label = new QLabel("Treasure Cards", scroll_area);
    //debug_treasure_cards_label = treasure_cards_label;
    treasure_cards_label->setStyleSheet(HeroQuestLevelWindow::DIALOG_HEADER_STYLE);

    vertical_box_layout->addWidget(treasure_cards_label);
    
    int header_height = treasure_cards_label->height();
    *max_height += header_height;

    QLabel* grid_label = new QLabel(scroll_area);
    grid_label->setContentsMargins(0, 0, 0, 0);
    vertical_box_layout->addWidget(grid_label);

    QGridLayout* grid_layout = new QGridLayout();
    grid_label->setLayout(grid_layout);
    grid_layout->setContentsMargins(0, 0, 0, 0);
    uint row = 0;
    for (set<TreasureCard>::const_iterator it = inventory->getTreasureCards().begin();
            it != inventory->getTreasureCards().end(); ++it)
    {
        const TreasureCard& treasure_card = *it;
        if (row != 0)
        {
            QLabel* space_label = new QLabel(grid_label);
            space_label->setFixedHeight(10);
            grid_layout->addWidget(space_label, row, 0, 2, 1);
            *max_height += space_label->height() + grid_layout->spacing();
            ++row;
        }
        TreasureCardPane* treasure_card_pane = new TreasureCardPane(grid_label, treasure_card);
        //if (row == 0)
        //debug_treasure_card_pane = treasure_card_pane;
        treasure_card_pane->setContentsMargins(0, 0, 0, 0);
        *min_height = max(*min_height, treasure_card_pane->sizeHint().height());
        grid_layout->addWidget(treasure_card_pane, row, 0, 1, 1, Qt::AlignTop);
        grid_layout->setRowMinimumHeight(row, *min_height);
        *max_height += treasure_card_pane->height() + grid_layout->spacing();
        if (row == 0)
        {
            cout << "setColumnMinimumWidth(0): " << treasure_card_pane->width() << endl;
            grid_layout->setColumnMinimumWidth(0, treasure_card_pane->width());
        }
        if (enable_use_button == USE_BUTTON_ENABLE)
        {
            UseInventoryItemButton* button = new UseInventoryItemButton(_dialog, grid_label, treasure_card, 0, "Use");
            button->setMinimumSize(MINIMUM_BUTTON_SIZE);
            // disable the "Use" button, if the treasure card cannot be used directly from the inventory
            if (treasure_card.isApplyableFromInventory())
            {
                button->setStyleSheet(HeroQuestLevelWindow::BUTTON_STYLE_ACTIVE);
            }
            else
            {
                button->setEnabled(false);
                button->setStyleSheet(HeroQuestLevelWindow::BUTTON_STYLE_INACTIVE);
            }
            grid_layout->addWidget(button, row, 1, 1, 1, Qt::AlignBottom);
            connect(button, SIGNAL(clicked()), button, SLOT(use()));
            if (row == 0)
                grid_layout->setColumnMinimumWidth(1, button->width());
        }
        QLabel* right_space_label = new QLabel();
        right_space_label->setFixedWidth(qApp->style()->pixelMetric(QStyle::PM_ScrollBarExtent));
        grid_layout->addWidget(right_space_label, row, 2);
        ++row;
    }
    grid_label->setMinimumSize(grid_layout->sizeHint());
    vertical_box_layout->addSpacing(10);
    *max_height += 10;
    *min_width = max(*min_width,
            grid_label->sizeHint().width() + vertical_box_layout->contentsMargins().left()
                    + vertical_box_layout->contentsMargins().right());
    *grid_layout_spacing = max(*grid_layout_spacing, grid_layout->spacing());
    vertical_box_layout->addStretch();
#if 0
            grid_label->setFrameStyle(QFrame::Panel | QFrame::Sunken);
            grid_label->setStyleSheet("color: rgb(0,255,0) ; border: 1px solid #00ff00 ;");
#endif
}

void InventoryPane::addSpellCards(int header_height, const Inventory* inventory, bool enable_use_button,
        QScrollArea* scroll_area, QBoxLayout* vertical_box_layout, int& grid_layout_spacing,
        int* min_width, int min_height, int* max_height)
{
    QLabel* spell_cards_label = new QLabel("Spell Cards", scroll_area);
    spell_cards_label->setStyleSheet(HeroQuestLevelWindow::DIALOG_HEADER_STYLE);
    vertical_box_layout->addWidget(spell_cards_label);
    header_height = spell_cards_label->height();
    *max_height += header_height;

    QLabel* grid_label = new QLabel(scroll_area);
#if 0
            grid_label->setFrameStyle(QFrame::Panel | QFrame::Sunken);
            grid_label->setStyleSheet("color: rgb(0,255,0) ; border: 1px solid #00ff00 ;");
#endif
    grid_label->setContentsMargins(0, 0, 0, 0);
    vertical_box_layout->addWidget(grid_label);
    QGridLayout* grid_layout = new QGridLayout();
    grid_label->setLayout(grid_layout);
    grid_layout->setContentsMargins(0, 0, 0, 0);
    uint row = 0;
    for (set<SpellCard>::const_iterator it = inventory->getSpellCards().begin(); it != inventory->getSpellCards().end();
            ++it)
    {
        const SpellCard& spell_card = *it;
        if (row != 0)
        {
            QLabel* space_label = new QLabel(grid_label);
            space_label->setFixedHeight(10);
            grid_layout->addWidget(space_label, row, 0, 2, 1);
            *max_height += space_label->height() + grid_layout->spacing();
            ++row;
        }
        SpellCardPane* spell_card_pane = new SpellCardPane(grid_label, spell_card);
        spell_card_pane->setContentsMargins(0, 0, 0, 0);
        min_height = max(min_height, spell_card_pane->sizeHint().height());
        grid_layout->addWidget(spell_card_pane, row, 0, 1, 1, Qt::AlignTop);
        grid_layout->setRowMinimumHeight(row, min_height);
        *max_height += spell_card_pane->height() + grid_layout->spacing();
        if (enable_use_button == USE_BUTTON_ENABLE)
        {
            UseInventoryItemButton* button = new UseInventoryItemButton(_dialog, grid_label, spell_card, 0, "Use");
            button->setMinimumSize(MINIMUM_BUTTON_SIZE);
            if (!HeroQuestLevelWindow::_hero_quest->getLevel()->getCurrentlyActingHeroActionStates()->_has_cast_spell)
            {
                button->setStyleSheet(HeroQuestLevelWindow::BUTTON_STYLE_ACTIVE);
            }
            else
            {
                button->setEnabled(false);
                button->setStyleSheet(HeroQuestLevelWindow::BUTTON_STYLE_INACTIVE);
            }
            grid_layout->addWidget(button, row, 1, 1, 1, Qt::AlignBottom);
            connect(button, SIGNAL(clicked()), button, SLOT(use()));
        }
        QLabel* right_space_label = new QLabel();
        right_space_label->setFixedWidth(qApp->style()->pixelMetric(QStyle::PM_ScrollBarExtent));
        grid_layout->addWidget(right_space_label, row, 2);
        ++row;
    }
    grid_label->setMinimumSize(grid_layout->sizeHint());
    vertical_box_layout->addSpacing(10);
    *max_height += 10;
    *min_width = max(*min_width,
            grid_label->sizeHint().width() + vertical_box_layout->contentsMargins().left()
                    + vertical_box_layout->contentsMargins().right());
    grid_layout_spacing = max(grid_layout_spacing, grid_layout->spacing());
    vertical_box_layout->addStretch();
}

void InventoryPane::addEquipmentCards(Inventory* inventory, bool enable_use_button, bool enable_discard_button,
        DialogBuyEquipment* dialog_buy_equipment,
        QScrollArea* scroll_area, QBoxLayout* vertical_box_layout, int& grid_layout_spacing, int* min_width,
        int min_height, int* max_height)
{
    QLabel* equipment_cards_label = new QLabel("Equipment Cards", scroll_area);
    equipment_cards_label->setStyleSheet(HeroQuestLevelWindow::DIALOG_HEADER_STYLE);
    vertical_box_layout->addWidget(equipment_cards_label);
    int header_height = equipment_cards_label->height();
    *max_height += header_height;

    QLabel* grid_label = new QLabel(scroll_area);
#if 0
            grid_label->setFrameStyle(QFrame::Panel | QFrame::Sunken);
            grid_label->setStyleSheet("color: rgb(0,255,0) ; border: 1px solid #00ff00 ;");
#endif
    grid_label->setContentsMargins(0, 0, 0, 0);
    vertical_box_layout->addWidget(grid_label);
    QGridLayout* grid_layout = new QGridLayout();
    grid_label->setLayout(grid_layout);
    grid_layout->setContentsMargins(0, 0, 0, 0);
    uint row = 0;
    for (set<EquipmentCard>::const_iterator it = inventory->getEquipmentCards().begin();
            it != inventory->getEquipmentCards().end(); ++it)
    {
        const EquipmentCard& equipment_card = *it;
        if (row != 0)
        {
            QLabel* space_label = new QLabel(grid_label);
            space_label->setFixedHeight(10);
            grid_layout->addWidget(space_label, row, 0, 2, 1);
            *max_height += space_label->height() + grid_layout->spacing();
            ++row;
        }
        EquipmentCardPane* equipment_card_pane = new EquipmentCardPane(grid_label, equipment_card);
        equipment_card_pane->setContentsMargins(0, 0, 0, 0);
        min_height = max(min_height, equipment_card_pane->sizeHint().height());
        grid_layout->addWidget(equipment_card_pane, row, 0, 1, 1, Qt::AlignTop);
        grid_layout->setRowMinimumHeight(row, min_height);
        *max_height += equipment_card_pane->height() + grid_layout->spacing();
        if (enable_use_button == USE_BUTTON_ENABLE && equipment_card.canBeUsed())
        {
            UseInventoryItemButton* button = new UseInventoryItemButton(_dialog, grid_label, equipment_card, inventory,
                    "Use");
            button->setMinimumSize(MINIMUM_BUTTON_SIZE);
            if (HeroQuestLevelWindow::_hero_quest->getLevel()->getCurrentlyActingHeroActionStates()->_num_attacks_left
                    > 0)
            {
                button->setStyleSheet(HeroQuestLevelWindow::BUTTON_STYLE_ACTIVE);
            }
            else
            {
                button->setEnabled(false);
                button->setStyleSheet(HeroQuestLevelWindow::BUTTON_STYLE_INACTIVE);
            }
            grid_layout->addWidget(button, row, 1, 1, 1, Qt::AlignBottom);
            connect(button, SIGNAL(clicked()), button, SLOT(use()));
        }
        else if (enable_discard_button == DISCARD_BUTTON_ENABLE)
        {
            UseInventoryItemButton* button = new UseInventoryItemButton(_dialog, grid_label, equipment_card, inventory,
                    "Discard");
            button->setMinimumSize(MINIMUM_BUTTON_SIZE);
            button->setStyleSheet(HeroQuestLevelWindow::BUTTON_STYLE_ACTIVE);
            grid_layout->addWidget(button, row, 1, 1, 1, Qt::AlignBottom);
            connect(button, SIGNAL(clicked()), button, SLOT(discard()));
        }
        QLabel* right_space_label = new QLabel();
        right_space_label->setFixedWidth(qApp->style()->pixelMetric(QStyle::PM_ScrollBarExtent));
        grid_layout->addWidget(right_space_label, row, 2);
        ++row;
    }
    grid_label->setMinimumSize(grid_layout->sizeHint());
    vertical_box_layout->addSpacing(10);
    *max_height += 10;
    *min_width = max(*min_width,
            grid_label->sizeHint().width() + vertical_box_layout->contentsMargins().left()
                    + vertical_box_layout->contentsMargins().right());
    grid_layout_spacing = max(grid_layout_spacing, grid_layout->spacing());
    vertical_box_layout->addStretch();
}
