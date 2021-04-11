#include "EquipmentMarketPlacePane.h"

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

#include "EquipmentCardPane.h"
#include "BuyItemButton.h"
#include "Debug.h"
#include "DialogBuyEquipment.h"
#include "EquipmentCardStorage.h"
#include "HeroQuestLevelWindow.h"
#include "Hero.h"

using namespace std;

const QSize EquipmentMarketPlacePane::MINIMUM_BUTTON_SIZE(100, 25);

/*
 * EquipmentMarketPlacePane
 *   scroll_label
 *
 *     equipment_cards_label
 *     grid_label, grid_layout
 *       equipment_card_pane  |  button
 *       equipment_card_pane  |  button
 *       ...
 */
EquipmentMarketPlacePane::EquipmentMarketPlacePane(DialogBuyEquipment* dialog_buy_equipment, Hero* hero)
        :
        QLabel(dialog_buy_equipment), _dialog_buy_equipment(dialog_buy_equipment), _scroll_area(0)
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

    updateEquipmentMarketPlace(hero);
}

EquipmentMarketPlacePane::~EquipmentMarketPlacePane()
{
}

/*!
 * Removes any existing widget from the _scroll_area, and adds a scroll_label to it, showing
 * the equipment items.
 */
void EquipmentMarketPlacePane::updateEquipmentMarketPlace(Hero* hero)
{
    // --- delete the scroll_label, if existing ---
    QWidget* old_scroll_label = _scroll_area->widget();
    _scroll_area->setWidget(0);
    delete old_scroll_label;

    // --- rebuild the scroll_label and everything contained by it ---
    int min_width = 0;
    int max_width = 0;
    int min_height = 0;
    int max_height = 0;
    int grid_layout_spacing = 0;
    const int ADDITIONAL_SPACE_FOR_PANE = 10; // without this additional space, the pane would be some pixels to narrow; I didn't yet understand why...

    QLabel* scroll_label = new QLabel();
    _scroll_area->setWidget(scroll_label);

    QBoxLayout* vertical_box_layout = new QBoxLayout(QBoxLayout::TopToBottom);
    vertical_box_layout->setContentsMargins(0, 0, 0, 0);
    scroll_label->setLayout(vertical_box_layout);

    if (!EquipmentCardStorage::instance->getEquipmentCardStock().empty())
    {
        addEquipmentCards(hero, _scroll_area,
                vertical_box_layout, grid_layout_spacing,
                &min_width, min_height, &max_height);
    }

    _scroll_area->setMinimumSize(QSize(min_width, min_height + grid_layout_spacing));

    scroll_label->setFixedWidth(_scroll_area->width());
    scroll_label->setFixedHeight(max_height);

    setMinimumWidth(min_width + ADDITIONAL_SPACE_FOR_PANE);

    DV(("_scroll_area: %d x %d", _scroll_area->width(), _scroll_area->height()));
    DV(("scroll_label: %d x %d", scroll_label->width(), scroll_label->height()));
}

void EquipmentMarketPlacePane::addEquipmentCards(Hero* hero, QScrollArea* scroll_area,
        QBoxLayout* vertical_box_layout,
        int& grid_layout_spacing, int* min_width,
        int min_height,
        int* max_height)
{
    vector<EquipmentCard>& equipment_cards = EquipmentCardStorage::instance->getEquipmentCardStock();

    QLabel* grid_label = new QLabel(scroll_area);
    grid_label->setContentsMargins(0, 0, 0, 0);
    vertical_box_layout->addWidget(grid_label);

    QGridLayout* grid_layout = new QGridLayout();
    grid_label->setLayout(grid_layout);
    grid_layout->setContentsMargins(0, 0, 0, 0);
    uint row = 0;
    for (vector<EquipmentCard>::const_iterator it = equipment_cards.begin(); it != equipment_cards.end(); ++it)
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
        if (row == 0)
        {
            cout << "setColumnMinimumWidth(0): " << equipment_card_pane->width() << endl;
            grid_layout->setColumnMinimumWidth(0, equipment_card_pane->width());
        }

        {
            BuyItemButton* button = new BuyItemButton(_dialog_buy_equipment, grid_label, equipment_card,
                    &(hero->getInventory()),
                    QString("%1 gold").arg(equipment_card.getCost()));
            button->setMinimumSize(MINIMUM_BUTTON_SIZE);
            // disable the "buy" button, if the hero doesn't have enough money for the equipment card,
            // or the hero is not allowed to own the equipment,
            // or the hero already has an identical piece of equipment
            if (equipment_card.getCost() <= hero->getInventory().getGold() && equipment_card.canBeUsedByHero(hero)
                    && !hero->getInventory().containsEquipmentCard(equipment_card))
            {
                button->setStyleSheet(HeroQuestLevelWindow::BUTTON_STYLE_ACTIVE);
            }
            else
            {
                button->setEnabled(false);
                button->setStyleSheet(HeroQuestLevelWindow::BUTTON_STYLE_INACTIVE);
            }
            grid_layout->addWidget(button, row, 1, 1, 1, Qt::AlignBottom);
            connect(button, SIGNAL(clicked()), button, SLOT(buy()));
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
    grid_layout_spacing = max(grid_layout_spacing, grid_layout->spacing());
    vertical_box_layout->addStretch();
}
