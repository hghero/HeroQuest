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
#include "TreasureCard.h"
#include "TreasureCardPane.h"
#include "SpellCardPane.h"
#include "UseInventoryItemButton.h"

using namespace std;

const QSize DialogInventory::MINIMUM_BUTTON_SIZE(100, 25);

/*
 * DialogInventory, outer_vertical_box_layout
 *   scroll_area(scroll_label)
 *
 *     treasure_cards_label
 *     grid_label, grid_layout
 *       treasure_card_pane  |  button
 *       space_label
 *       treasure_card_pane  |  button
 *       ...
 *     space_label
 *
 *     spell_cards_label
 *     grid_label, grid_layout
 *       spell_card_pane  |  button
 *       space_label
 *       spell_card_pane  |  button
 *       ...
 *     space_label
 *
 *   space_label
 *
 *   button_bar_label
 *     button_box
 */
DialogInventory::DialogInventory(uint mode_flag)
: QDialog(HeroQuestLevelWindow::_hero_quest)
{
    bool show_treasure_cards = mode_flag & TREASURE_CARDS;
    bool show_spell_cards = mode_flag & SPELL_CARDS;
    bool show_equipment_cards = mode_flag & EQUIPMENT_CARDS;
    bool show_booty_cards = mode_flag & BOOTY_CARDS;

    Hero* current_hero = HeroQuestLevelWindow::_hero_quest->getLevel()->getCurrentlyActingHero();
    if (current_hero == 0)
        return;

    setWindowTitle("Inventory");
    setStyleSheet(HeroQuestLevelWindow::DIALOG_STYLE);

    QBoxLayout* outer_vertical_box_layout = new QBoxLayout(QBoxLayout::TopToBottom);
    outer_vertical_box_layout->setSpacing(0);
    setLayout(outer_vertical_box_layout);

    int min_width = 0;
    int max_width = 0;
    int min_height = 0;
    int max_height = 0;
    int header_height = 0;
    const int scroll_bar_padding = qApp->style()->pixelMetric(QStyle::PM_ScrollBarExtent);
    int grid_layout_spacing = 0;

    {
        QScrollArea* scroll_area = new QScrollArea(this);
        scroll_area->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        scroll_area->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        scroll_area->setContentsMargins(0, 0, 0, 0);
        scroll_area->setStyleSheet("border: none ;");
        outer_vertical_box_layout->addWidget(scroll_area);

        QLabel* scroll_label = new QLabel();
        scroll_area->setWidget(scroll_label);

        QBoxLayout* vertical_box_layout = new QBoxLayout(QBoxLayout::TopToBottom);
        vertical_box_layout->setContentsMargins(0, 0, 0, 0);
        scroll_label->setLayout(vertical_box_layout);

        if (show_treasure_cards && !current_hero->getInventory().getTreasureCards().empty())
        {
            QLabel* treasure_cards_label = new QLabel("Treasure Cards", scroll_area);
            //debug_treasure_cards_label = treasure_cards_label;
            treasure_cards_label->setStyleSheet(HeroQuestLevelWindow::DIALOG_HEADER_STYLE);
            vertical_box_layout->addWidget(treasure_cards_label);
            header_height = treasure_cards_label->height();
            max_height += header_height;

            QLabel* grid_label = new QLabel(scroll_area);
            grid_label->setContentsMargins(0, 0, 0, 0);
            vertical_box_layout->addWidget(grid_label);

#if 0
            grid_label->setFrameStyle(QFrame::Panel | QFrame::Sunken);
            grid_label->setStyleSheet("color: rgb(0,255,0) ; border: 1px solid #00ff00 ;");
#endif

            QGridLayout* grid_layout = new QGridLayout();
            grid_label->setLayout(grid_layout);
            grid_layout->setContentsMargins(0, 0, 0, 0);

            uint row = 0;
            for (set<TreasureCard>::const_iterator it = current_hero->getInventory().getTreasureCards().begin(); it != current_hero->getInventory().getTreasureCards().end(); ++it)
            {
                const TreasureCard& treasure_card = *it;

                if (row != 0)
                {
                    QLabel* space_label = new QLabel(grid_label);
                    space_label->setFixedHeight(10);
                    grid_layout->addWidget(space_label, row, 0, 2, 1);
                    max_height += space_label->height() + grid_layout->spacing();
                    ++row;
                }

                TreasureCardPane* treasure_card_pane = new TreasureCardPane(grid_label, treasure_card);
                //if (row == 0)
                //debug_treasure_card_pane = treasure_card_pane;
                treasure_card_pane->setContentsMargins(0, 0, 0, 0);
                min_height = max(min_height, treasure_card_pane->sizeHint().height());
                grid_layout->addWidget(treasure_card_pane, row, 0, 1, 1, Qt::AlignTop);
                grid_layout->setRowMinimumHeight(row, min_height);
                max_height += treasure_card_pane->height() + grid_layout->spacing();

                if (row == 0)
                {
                    cout << "setColumnMinimumWidth(0): " << treasure_card_pane->width() << endl;
                    grid_layout->setColumnMinimumWidth(0, treasure_card_pane->width());
                }

                UseInventoryItemButton* button = new UseInventoryItemButton(this, grid_label, treasure_card, "Use");
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

                QLabel* right_space_label = new QLabel();
                right_space_label->setFixedWidth(qApp->style()->pixelMetric(QStyle::PM_ScrollBarExtent));
                grid_layout->addWidget(right_space_label, row, 2);

                ++row;
            }

            grid_label->setMinimumSize(grid_layout->sizeHint());

            QLabel* space_label = new QLabel(scroll_area);
            space_label->setFixedHeight(10);
            vertical_box_layout->addWidget(space_label);
            max_height += space_label->height();

            min_width = grid_label->sizeHint().width() + vertical_box_layout->contentsMargins().left() + vertical_box_layout->contentsMargins().right();
            grid_layout_spacing = max(grid_layout_spacing, grid_layout->spacing());

            vertical_box_layout->addStretch();
        }

        if (show_spell_cards && !current_hero->getInventory().getSpellCards().empty())
        {
            QLabel* spell_cards_label = new QLabel("Spell Cards", scroll_area);
            spell_cards_label->setStyleSheet(HeroQuestLevelWindow::DIALOG_HEADER_STYLE);
            vertical_box_layout->addWidget(spell_cards_label);
            header_height = spell_cards_label->height();
            max_height += header_height;

            QLabel* grid_label = new QLabel(scroll_area);
            grid_label->setContentsMargins(0, 0, 0, 0);
            vertical_box_layout->addWidget(grid_label);

#if 0
            grid_label->setFrameStyle(QFrame::Panel | QFrame::Sunken);
            grid_label->setStyleSheet("color: rgb(0,255,0) ; border: 1px solid #00ff00 ;");
#endif

            QGridLayout* grid_layout = new QGridLayout();
            grid_label->setLayout(grid_layout);
            grid_layout->setContentsMargins(0, 0, 0, 0);

            uint row = 0;
            for (set<SpellCard>::const_iterator it = current_hero->getInventory().getSpellCards().begin(); it != current_hero->getInventory().getSpellCards().end(); ++it)
            {
                const SpellCard& spell_card = *it;

                if (row != 0)
                {
                    QLabel* space_label = new QLabel(grid_label);
                    space_label->setFixedHeight(10);
                    grid_layout->addWidget(space_label, row, 0, 2, 1);
                    max_height += space_label->height() + grid_layout->spacing();
                    ++row;
                }

                SpellCardPane* spell_card_pane = new SpellCardPane(grid_label, spell_card);
                spell_card_pane->setContentsMargins(0, 0, 0, 0);
                min_height = max(min_height, spell_card_pane->sizeHint().height());
                grid_layout->addWidget(spell_card_pane, row, 0, 1, 1, Qt::AlignTop);
                grid_layout->setRowMinimumHeight(row, min_height);
                max_height += spell_card_pane->height() + grid_layout->spacing();

                UseInventoryItemButton* button = new UseInventoryItemButton(this, grid_label, spell_card, "Use");
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

                QLabel* right_space_label = new QLabel();
                right_space_label->setFixedWidth(qApp->style()->pixelMetric(QStyle::PM_ScrollBarExtent));
                grid_layout->addWidget(right_space_label, row, 2);

                ++row;
            }

            grid_label->setMinimumSize(grid_layout->sizeHint());

            QLabel* space_label = new QLabel(scroll_area);
            space_label->setFixedHeight(10);
            vertical_box_layout->addWidget(space_label);
            max_height += space_label->height();

            min_width = grid_label->sizeHint().width() + vertical_box_layout->contentsMargins().left() + vertical_box_layout->contentsMargins().right();
            grid_layout_spacing = max(grid_layout_spacing, grid_layout->spacing());

            vertical_box_layout->addStretch();
        }

        scroll_area->setMinimumSize(QSize(min_width, min_height + header_height + grid_layout_spacing));

        scroll_label->setFixedWidth(scroll_area->width());
        scroll_label->setFixedHeight(max_height);
    }

    QLabel* space_label = new QLabel();
    space_label->setFixedHeight(10);
    outer_vertical_box_layout->addWidget(space_label);

    outer_vertical_box_layout->addStretch();

    {
        QLabel* button_bar_label = new QLabel();
        outer_vertical_box_layout->addWidget(button_bar_label);

#if 0
        button_bar_label->setFrameStyle(QFrame::Panel | QFrame::Sunken);
        button_bar_label->setStyleSheet("color: rgb(255,0,0) ; border: 1px solid #ff0000 ;");
#endif

        QBoxLayout* outer_button_layout = new QBoxLayout(QBoxLayout::LeftToRight);
        outer_button_layout->setSpacing(0);
        outer_button_layout->setContentsMargins(0, 0, 0, 0);
        button_bar_label->setLayout(outer_button_layout);

        QSize internal_size_hint(0, 0);
        {
            outer_button_layout->addStretch();

            QDialogButtonBox* button_box = new QDialogButtonBox(QDialogButtonBox::Close);
            button_box->setStyleSheet(HeroQuestLevelWindow::DIALOG_STYLE);
            button_box->button(QDialogButtonBox::Close)->setMinimumSize(MINIMUM_BUTTON_SIZE);
            button_box->button(QDialogButtonBox::Close)->setStyleSheet(HeroQuestLevelWindow::BUTTON_STYLE_ACTIVE);
            outer_button_layout->addWidget(button_box);
            connect(button_box->button(QDialogButtonBox::Close), SIGNAL(clicked()), this, SLOT(accept()));

            QLabel* space_label = new QLabel();
            space_label->setFixedWidth(scroll_bar_padding + grid_layout_spacing);
            outer_button_layout->addWidget(space_label);

            internal_size_hint = button_box->sizeHint();
        }

        button_bar_label->setMinimumSize(internal_size_hint);
    }

#if 0
    if (debug_treasure_cards_label != 0)
    {
        QSize size_hint_tcl = debug_treasure_cards_label->sizeHint();
        cout << "sizeHint of debug_treasure_cards_label: " << size_hint_tcl.width() << " x " << size_hint_tcl.height()
                << endl;
        QSize size_tcl = debug_treasure_cards_label->size();
        cout << "size of debug_treasure_cards_label: " << size_tcl.width() << " x " << size_tcl.height() << endl;
    }
    if (debug_treasure_card_pane != 0)
    {
        QSize size_hint_tcl = debug_treasure_card_pane->sizeHint();
        cout << "sizeHint of debug_treasure_card_pane: " << size_hint_tcl.width() << " x " << size_hint_tcl.height()
                << endl;
        QSize size_tcl = debug_treasure_card_pane->size();
        cout << "size of debug_treasure_card_pane: " << size_tcl.width() << " x " << size_tcl.height() << endl;
    }
#endif
}

DialogInventory::~DialogInventory()
{
    // NIX
}

