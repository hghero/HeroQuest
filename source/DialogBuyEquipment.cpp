#include "DialogBuyEquipment.h"

#include <QtWidgets/QLabel>
#include <QtWidgets/QLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QDialogButtonBox>
#include <QtGui/QIcon>
#include "HeroQuestLevelWindow.h"
#include "InventoryPane.h"
#include "EquipmentCardPane.h"
#include "EquipmentCardStorage.h"
#include "EquipmentMarketPlacePane.h"
#include "Inventory.h"
#include "HeroCamp.h"
#include "Hero.h"
#include "Debug.h"
#include "ParameterStorage.h"

using namespace std;

const int DialogBuyEquipment::HERO_ICON_HEIGHT = 100;

DialogBuyEquipment::DialogBuyEquipment(HeroCamp& hero_camp)
        :
        QDialog(), _heroes(), _currently_chosen_hero_index(0), _hero_avatars_color(), _hero_avatars_gray(), //
        _hero_buttons(), _hero_gold_labels(), _inventory_label(0), _inventory_pane(0), _equipment_market_place_pane(0)
{
    setWindowTitle("Buy equipment: choose hero");
    setStyleSheet(HeroQuestLevelWindow::DIALOG_STYLE);

    int screen_width = ParameterStorage::instance->getScreenWidth();
    int screen_height = ParameterStorage::instance->getScreenHeight();

    // Dialog height := 3/4 of screen height
    int dialog_height(screen_height * 3 / 4);

    hero_camp.getHeroes(&_heroes);
    initHeroAvatars();

    _hero_gold_labels.reserve(_heroes.size());

    /*
     *  -------------------------------------------------------------------------------------------------
     * |   -----------    Hero's Inventory                                                               |
     * |  | Barbarian |    ----------------------------------------------------------------------------  |
     * |  | Gold: 340 |   | Equipment                                                               ^  | |
     * |   -----------    |                                                                         |  | |
     * |                  | <equipment back> <equipment image> <equipment title>                 scroll| |
     * |     Dwarf        |                                    <equipment description>             bar | |
     * |    Gold: 100     |                                                      <dispose button>   |  | |
     * |                  |                                                                         v  | |
     * |                   ----------------------------------------------------------------------------  |
     * |      Alb                                                                                        |
     * |    Gold:  50     Equipment Market Place                                                         |
     * |                   ----------------------------------------------------------------------------  |
     * |                  | <equipment back> <equipment image> <equipment title>                    ^  | |
     * |    Magician      |                                    <equipment description>              |  | |
     * |    Gold:   0     |                                                         <buy button> scroll| |
     * |                  |                                                                        bar | |
     * |                  |                                                                         |  | |
     * |                  |                                                                         v  | |
     * |                   ----------------------------------------------------------------------------  |
     *  -------------------------------------------------------------------------------------------------
     * |                                                                              | Proceed to level |
     *  -------------------------------------------------------------------------------------------------
     */

    QBoxLayout* outer_vertical_box_layout_level_1 = new QBoxLayout(QBoxLayout::TopToBottom);
    setLayout(outer_vertical_box_layout_level_1);
    outer_vertical_box_layout_level_1->setSpacing(20);

    // === main upper part ===
    {
        QBoxLayout* outer_horizontal_box_layout_level_2 = new QBoxLayout(QBoxLayout::LeftToRight);
        outer_vertical_box_layout_level_1->addLayout(outer_horizontal_box_layout_level_2);

        // --- left bar: heroes + gold ---
        {
            QBoxLayout* vertical_heroes_layout_level_3 = new QBoxLayout(QBoxLayout::TopToBottom);
            outer_horizontal_box_layout_level_2->addLayout(vertical_heroes_layout_level_3);
            vertical_heroes_layout_level_3->setAlignment(Qt::AlignHCenter);
            vertical_heroes_layout_level_3->setSpacing(10);

            // for each hero, add a button showing the hero's image, and a gold label
            for (size_t i = 0; i < _heroes.size(); ++i)
            {
                Hero* hero = _heroes[i];

                QPushButton* hero_button = new QPushButton(this);
                _hero_buttons.push_back(hero_button);
                hero_button->setStyleSheet(HeroQuestLevelWindow::BUTTON_STYLE_ACTIVE);
                vertical_heroes_layout_level_3->addWidget(hero_button, 0, Qt::AlignHCenter);

                switch (i)
                {
                    case 0:
                    {
                        connect(hero_button, SIGNAL(clicked()), this, SLOT(hero0pressed()));
                        break;
                    }
                    case 1:
                    {
                        connect(hero_button, SIGNAL(clicked()), this, SLOT(hero1pressed()));
                        break;
                    }
                    case 2:
                    {
                        connect(hero_button, SIGNAL(clicked()), this, SLOT(hero2pressed()));
                        break;
                    }
                    case 3:
                    {
                        connect(hero_button, SIGNAL(clicked()), this, SLOT(hero3pressed()));
                        break;
                    }
                    default:
                    {
                        DVX(("Index %d out of bounds", i));
                        break;
                    }
                }

                QLabel* gold_label = new QLabel(QString("Gold: %1").arg(hero->getGold()), this);
                _hero_gold_labels.push_back(gold_label);
                gold_label->setStyleSheet(HeroQuestLevelWindow::DIALOG_TEXT_STYLE);
                gold_label->setAlignment(Qt::AlignHCenter);
                vertical_heroes_layout_level_3->addWidget(gold_label, 0, Qt::AlignHCenter);

                vertical_heroes_layout_level_3->addSpacing(10);
            }
            updateHeroButtonIcons();

            vertical_heroes_layout_level_3->addStretch();
        }

        // --- right bar: inventory of selected hero, and equipment market place ---
        {
            QBoxLayout* vertical_inventory_equipment_layout_level_3 = new QBoxLayout(QBoxLayout::TopToBottom);
            outer_horizontal_box_layout_level_2->addLayout(vertical_inventory_equipment_layout_level_3);
            vertical_inventory_equipment_layout_level_3->setAlignment(Qt::AlignLeft);

            // upper: inventory of selected hero
            {
                QBoxLayout* vertical_inventory_layout_level_4 = new QBoxLayout(QBoxLayout::TopToBottom);
                vertical_inventory_equipment_layout_level_3->addLayout(vertical_inventory_layout_level_4);
                vertical_inventory_layout_level_4->setAlignment(Qt::AlignLeft);
                vertical_inventory_layout_level_4->setSpacing(10);

                // inventory: label
                _inventory_label = new QLabel(
                        QString("%1's Inventory").arg(_heroes[_currently_chosen_hero_index]->getName()), this);
                _inventory_label->setStyleSheet(HeroQuestLevelWindow::DIALOG_HEADER_STYLE);
                vertical_inventory_layout_level_4->addWidget(_inventory_label, 0, Qt::AlignLeft);

                // inventory: InventoryPane
                // Inventory height := 1/3 of dialog height
                _inventory_pane = new InventoryPane(this, &(_heroes[_currently_chosen_hero_index]->getInventory()),
                        InventoryPane::ALL, InventoryPane::USE_BUTTON_DISABLE, InventoryPane::DISCARD_BUTTON_ENABLE,
                        this);
                _inventory_pane->setFixedHeight(dialog_height / 3);
                vertical_inventory_layout_level_4->addWidget(_inventory_pane, 0, Qt::AlignLeft);

                vertical_inventory_layout_level_4->addStretch();
            }

            vertical_inventory_equipment_layout_level_3->addSpacing(20);

            // lower: equipment market place
            {
                QBoxLayout* vertical_equipment_layout_level_4 = new QBoxLayout(QBoxLayout::TopToBottom);
                vertical_inventory_equipment_layout_level_3->addLayout(vertical_equipment_layout_level_4);
                vertical_equipment_layout_level_4->setAlignment(Qt::AlignLeft);

                QLabel* equipment_market_place_label = new QLabel("Equipment Market Place", this);
                equipment_market_place_label->setStyleSheet(HeroQuestLevelWindow::DIALOG_HEADER_STYLE);
                vertical_equipment_layout_level_4->addWidget(equipment_market_place_label);

                // equipment height := 1/2 of dialog height
                _equipment_market_place_pane = new EquipmentMarketPlacePane(this,
                        _heroes[_currently_chosen_hero_index]);
                _equipment_market_place_pane->setFixedHeight(dialog_height / 2);
                DV(
                        ("_equipment_market_place_pane: width %d, height %d", _equipment_market_place_pane->width(), _equipment_market_place_pane->height()));
                vertical_equipment_layout_level_4->addWidget(_equipment_market_place_pane);

                vertical_equipment_layout_level_4->addStretch();
            }

            // --- stretch ---
            vertical_inventory_equipment_layout_level_3->addStretch();
        }

        // --- stretch ---
        outer_horizontal_box_layout_level_2->addStretch();
    }

    // === lower part: "proceed" button ===
    {
        QBoxLayout* horizontal_button_layout_level_2 = new QBoxLayout(QBoxLayout::RightToLeft);
        outer_vertical_box_layout_level_1->addLayout(horizontal_button_layout_level_2);

        QPushButton* button_proceed = new QPushButton("Proceed to level", this);
        button_proceed->setStyleSheet(HeroQuestLevelWindow::BUTTON_STYLE_ACTIVE);
        button_proceed->setFixedHeight(40);
        connect(button_proceed, SIGNAL(clicked()), this, SLOT(accept()));
        horizontal_button_layout_level_2->addWidget(button_proceed);

        horizontal_button_layout_level_2->addStretch();
    }
}

DialogBuyEquipment::~DialogBuyEquipment()
{
    // _hero_buttons is automatically destroyed with this dialog, as this dialog is its parent
}

void DialogBuyEquipment::updateHeroButtonIcons()
{
    for (size_t i = 0; i < _heroes.size(); ++i)
    {
        QPixmap& pixmap = (i == _currently_chosen_hero_index) ? _hero_avatars_color[i] : _hero_avatars_gray[i];
        _hero_buttons[i]->setIcon(pixmap);
        QSize icon_size(HERO_ICON_HEIGHT * pixmap.width() / pixmap.height(), HERO_ICON_HEIGHT);
        _hero_buttons[i]->setIconSize(icon_size);
        _hero_buttons[i]->setFixedSize(icon_size);
    }
}

void DialogBuyEquipment::updateHeroGoldLabelOfCurrentlyChosenHero()
{
    Hero* hero = _heroes[_currently_chosen_hero_index];
    QLabel* gold_label = _hero_gold_labels[_currently_chosen_hero_index];
    gold_label->setText(QString("Gold: %1").arg(hero->getGold()));
}

void DialogBuyEquipment::hero0pressed()
{
    _currently_chosen_hero_index = 0;
    updateContent();
}

void DialogBuyEquipment::hero1pressed()
{
    _currently_chosen_hero_index = 1;
    updateContent();
}

void DialogBuyEquipment::hero2pressed()
{
    _currently_chosen_hero_index = 2;
    updateContent();
}

void DialogBuyEquipment::hero3pressed()
{
    _currently_chosen_hero_index = 3;
    updateContent();
}

void DialogBuyEquipment::updateContent()
{
    updateHeroButtonIcons();
    updateInventory();
    updateEquipmentMarketPlace();
    update();
}

void DialogBuyEquipment::updateInventory()
{
    _inventory_label->setText(QString("%1's Inventory").arg(_heroes[_currently_chosen_hero_index]->getName()));
    _inventory_pane->updateInventory(&(_heroes[_currently_chosen_hero_index]->getInventory()), InventoryPane::ALL,
            false,
            true, this);
}

void DialogBuyEquipment::updateEquipmentMarketPlace()
{
    _equipment_market_place_pane->updateEquipmentMarketPlace(_heroes[_currently_chosen_hero_index]);
}

/*!
 * Clears and rebuilds _hero_avatars_color and _hero_avatars_gray from _heroes' avatars.
 */
void DialogBuyEquipment::initHeroAvatars()
{
    _hero_avatars_gray.clear();

    for (size_t i = 0; i < _heroes.size(); ++i)
    {
        Hero* hero = _heroes[i];

        QPixmap* hero_avatar_orig = hero->getAvatar();
        _hero_avatars_color.push_back(hero_avatar_orig->copy());

        QImage image = hero_avatar_orig->copy().toImage();
        QImage image_grayscale = image.convertToFormat(QImage::Format_Mono);
        _hero_avatars_gray.push_back(QPixmap::fromImage(image_grayscale));
    }
}
