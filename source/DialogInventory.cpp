#include "DialogInventory.h"

#include <set>
#include <iostream>

#include <QtWidgets/QLabel>
#include <QtWidgets/QLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QScrollBar>
#include <QtWidgets/QStyle>

#include "HeroQuestLevelWindow.h"
#include "Hero.h"
#include "Level.h"
#include "InventoryPane.h"
#include "Debug.h"

using namespace std;


DialogInventory::DialogInventory(uint show_content)
        :
        QDialog(HeroQuestLevelWindow::_hero_quest), _inventory_pane(0)
{
    Hero* current_hero = HeroQuestLevelWindow::_hero_quest->getLevel()->getCurrentlyActingHero();
    if (current_hero == 0)
        return;

    setWindowTitle("Inventory");
    setStyleSheet(HeroQuestLevelWindow::DIALOG_STYLE);
    setContentsMargins(0, 0, 0, 0);

    QBoxLayout* layout = new QBoxLayout(QBoxLayout::TopToBottom);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(10);
    setLayout(layout);

    _inventory_pane = new InventoryPane(this, &(current_hero->getInventory()), show_content,
            InventoryPane::USE_BUTTON_ENABLE, InventoryPane::DISREGARD_BUTTON_DISABLE, 0);
    layout->addWidget(_inventory_pane);
    _inventory_pane->setFixedHeight(400);

    {
        // ----------------- close button -----------------
        QBoxLayout* outer_button_layout = new QBoxLayout(QBoxLayout::LeftToRight);
        layout->addLayout(outer_button_layout);

        outer_button_layout->setSpacing(0);
        outer_button_layout->setContentsMargins(0, 0, 0, 0);

        {
            outer_button_layout->addStretch();

            QDialogButtonBox* button_box = new QDialogButtonBox(QDialogButtonBox::Close);
            button_box->setStyleSheet(HeroQuestLevelWindow::DIALOG_STYLE);
            button_box->button(QDialogButtonBox::Close)->setMinimumSize(InventoryPane::MINIMUM_BUTTON_SIZE);
            button_box->button(QDialogButtonBox::Close)->setStyleSheet(HeroQuestLevelWindow::BUTTON_STYLE_ACTIVE);
            outer_button_layout->addWidget(button_box);
            connect(button_box->button(QDialogButtonBox::Close), SIGNAL(clicked()), this, SLOT(accept()));

            //outer_button_layout->addSpacing(scroll_bar_padding + grid_layout_spacing);
        }
    }
}

DialogInventory::~DialogInventory()
{
    // NIX
}

