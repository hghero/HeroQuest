#include "HeroStatisticPane.h"

#include <cstdlib>

#include <QtWidgets/QLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSizePolicy>

#include "Hero.h"
#include "HeroQuestLevelWindow.h"
#include "Debug.h"
#include "Level.h"


using namespace std;


const QString HeroStatisticPane::BACKGROUND_YELLOW_FILENAME = ":/graphics/background_yellow.jpg";

HeroStatisticPane::HeroStatisticPane(Hero* hero)
:
QLabel(),
_background_image(0),
_hero(hero),
_hero_name_label(0),
_num_dice_move_label(0),
_num_move_points_remaining_label(0),
_num_dice_attack_label(0),
_num_dice_defend_label(0),
_life_points_label(0),
_intelligence_points_label(0),
_gold_label(0),
_weapons_label(0),
_inventory_button(0)
{
    _background_image = new QPixmap(BACKGROUND_YELLOW_FILENAME.toUtf8().constData());
    if (!_background_image) {
       DVX(("%s not found or failed to load", qPrintable(BACKGROUND_YELLOW_FILENAME)));
    }
    setPixmap(*_background_image);

#if 0
    setFrameStyle(QFrame::Panel | QFrame::Sunken);
    setStyleSheet("color: rgb(255,0,0) ; border: 1px solid #00ff00 ;");
#endif

    //   --------------- ---------------
    //  |               |               |
    //  |               |               |
    //  |               |               |
    //  | left content  | right content |
    //  |               |               |
    //  |               |               |
    //  |               |               |
    //   --------------- ---------------
    //

    QHBoxLayout* hbox_layout = new QHBoxLayout;
    hbox_layout->setContentsMargins(0, 0, 0, 0);
    setLayout(hbox_layout);

    QLabel* left_content = new QLabel();
    QLabel* right_content = new QLabel();
    left_content->setStyleSheet(HeroQuestLevelWindow::STATISTIC_BODY_STYLE_ACTIVE);
    right_content->setStyleSheet(HeroQuestLevelWindow::STATISTIC_BODY_STYLE_ACTIVE);
    hbox_layout->addWidget(left_content);
    hbox_layout->addWidget(right_content);
    hbox_layout->setStretchFactor(left_content, 3);
    hbox_layout->setStretchFactor(right_content, 2);

    // --------------------------------------
    // left content (statistical information)
    // --------------------------------------
    QVBoxLayout* vbox_layout = new QVBoxLayout;
    left_content->setLayout(vbox_layout);

#if 0
    // === is _hero currently active? ===
    list<Hero*>::iterator acting_hero_it = _hero_quest->getLevel()->getRound()._acting_hero_it;
    bool hero_is_active =
        _hero_quest->getLevel()->getRound()._heroes_not_monsters &&
        ((*acting_hero_it) == _hero);
#endif

    // the hero's name
    _hero_name_label = new QLabel;
    _hero_name_label->setText(QString(""));
    _hero_name_label->setStyleSheet(HeroQuestLevelWindow::STATISTIC_HEADER_STYLE_INACTIVE);
    vbox_layout->addWidget(_hero_name_label);

    // num_dice_move
    _num_dice_move_label = new QLabel;
    _num_dice_move_label->setText(QString(""));
    _num_dice_move_label->setStyleSheet(HeroQuestLevelWindow::STATISTIC_BODY_STYLE_INACTIVE);
    vbox_layout->addWidget(_num_dice_move_label);

    // number of remaining movement points (if hero is currently active)
    _num_move_points_remaining_label = new QLabel;
    _num_move_points_remaining_label->setText(QString("12")); // just to get maximum x-extent; will be reset to "" later
    _num_move_points_remaining_label->setStyleSheet(HeroQuestLevelWindow::STATISTIC_BODY_STYLE_INACTIVE);
    vbox_layout->addWidget(_num_move_points_remaining_label);

    // num_dice_attack
    _num_dice_attack_label = new QLabel;
    _num_dice_attack_label->setText(QString(""));
    _num_dice_attack_label->setStyleSheet(HeroQuestLevelWindow::STATISTIC_BODY_STYLE_INACTIVE);
    vbox_layout->addWidget(_num_dice_attack_label);

    // num_dice_defend
    _num_dice_defend_label = new QLabel;
    _num_dice_defend_label->setText(QString(""));
    _num_dice_defend_label->setStyleSheet(HeroQuestLevelWindow::STATISTIC_BODY_STYLE_INACTIVE);
    vbox_layout->addWidget(_num_dice_defend_label);

    // life_points
    _life_points_label = new QLabel;
    _life_points_label->setText(QString(""));
    _life_points_label->setStyleSheet(HeroQuestLevelWindow::STATISTIC_BODY_STYLE_INACTIVE);
    vbox_layout->addWidget(_life_points_label);

    // intelligence_points
    _intelligence_points_label = new QLabel;
    _intelligence_points_label->setText(QString(""));
    _intelligence_points_label->setStyleSheet(HeroQuestLevelWindow::STATISTIC_BODY_STYLE_INACTIVE);
    vbox_layout->addWidget(_intelligence_points_label);

    // gold
    _gold_label = new QLabel;
    _gold_label->setText(QString(""));
    _gold_label->setStyleSheet(HeroQuestLevelWindow::STATISTIC_BODY_STYLE_INACTIVE);
    vbox_layout->addWidget(_gold_label);

    // weapons
    _weapons_label = new QLabel;
    _weapons_label->setText(QString(""));
    _weapons_label->setStyleSheet(HeroQuestLevelWindow::STATISTIC_BODY_STYLE_INACTIVE);
    vbox_layout->addWidget(_weapons_label);

    // --------------------------------------
    // right content (buttons)
    // --------------------------------------
    QVBoxLayout* vbox_layout_right = new QVBoxLayout;
    vbox_layout_right->setAlignment(Qt::AlignBottom | Qt::AlignCenter);
    right_content->setLayout(vbox_layout_right);

    vbox_layout_right->addStretch();

    // inventory button
    _inventory_button = new QPushButton("Inventory");
    _inventory_button->setEnabled(false);
    _inventory_button->setStyleSheet(HeroQuestLevelWindow::INVENTORY_BUTTON_STYLE_INACTIVE);
    vbox_layout_right->addWidget(_inventory_button);

    update(); // assigns texts to labels

    left_content->setFixedSize(vbox_layout->sizeHint());
    _num_move_points_remaining_label->setText(QString(""));

    right_content->setFixedHeight(vbox_layout->sizeHint().height());
    right_content->setFixedWidth(vbox_layout_right->sizeHint().width());

    // adjust widths to remaining free screen size available in main window's info pane
    int info_pane_width = HeroQuestLevelWindow::_hero_quest->getInfoPaneSize().width();
    int free_space = info_pane_width - left_content->width() - right_content->width();
    if (free_space > 0)
    {
        left_content->setFixedWidth(left_content->width() + free_space / 2);
        right_content->setFixedWidth(right_content->width() + free_space / 2);
    }

    setFixedSize(hbox_layout->sizeHint());
}

HeroStatisticPane::~HeroStatisticPane()
{
    // NIX
}

void HeroStatisticPane::update()
{
    // === is _hero currently active? ===
    bool hero_is_active = false;
    if (HeroQuestLevelWindow::_hero_quest->getLevel() != 0 && HeroQuestLevelWindow::_hero_quest->getLevel()->getCurrentlyActingCreature() != 0)
    {
        Creature* creature = HeroQuestLevelWindow::_hero_quest->getLevel()->getCurrentlyActingCreature();
        hero_is_active = creature->isHero() && dynamic_cast<Hero*>(creature) == _hero;
    }

    // the hero's name
    QString hero_name = _hero->getName();
    if (HeroQuestLevelWindow::_hero_quest->getLevel()->getHeroActionStates(_hero)->_sleeping)
        hero_name += " (sleeping)";
    _hero_name_label->setText(hero_name);
    if (hero_is_active)
        _hero_name_label->setStyleSheet(HeroQuestLevelWindow::STATISTIC_HEADER_STYLE_ACTIVE);
    else
        _hero_name_label->setStyleSheet(HeroQuestLevelWindow::STATISTIC_HEADER_STYLE_INACTIVE);

    // num_dice_move
    _num_dice_move_label->setText(
            QString("Movement dice: %1%2").arg(_hero->getNumDiceMove()).arg(
                    _hero->getCanMoveThroughWalls() ? " (through walls)" : ""));
    if (hero_is_active)
        _num_dice_move_label->setStyleSheet(HeroQuestLevelWindow::STATISTIC_BODY_STYLE_ACTIVE);
    else
        _num_dice_move_label->setStyleSheet(HeroQuestLevelWindow::STATISTIC_BODY_STYLE_INACTIVE);

    // number of remaining movement points (if hero is currently active)
    if (hero_is_active &&
            HeroQuestLevelWindow::_hero_quest->getLevel() != 0 &&
            HeroQuestLevelWindow::_hero_quest->getLevel()->getCurrentlyActingCreature() != 0 &&
            HeroQuestLevelWindow::_hero_quest->getLevel()->getCurrentlyActingCreature()->isHero())
    {
        _num_move_points_remaining_label->setText(QString("Current movement points: %1").arg(HeroQuestLevelWindow::_hero_quest->getLevel()->getCurrentlyActingHeroActionStates()->_move_points));
    }
    else
    {
        _num_move_points_remaining_label->setText(QString("Current movement points: 0"));
    }
    if (hero_is_active)
        _num_move_points_remaining_label->setStyleSheet(HeroQuestLevelWindow::STATISTIC_BODY_STYLE_ACTIVE);
    else
        _num_move_points_remaining_label->setStyleSheet(HeroQuestLevelWindow::STATISTIC_BODY_STYLE_INACTIVE);

    // num_dice_attack
    _num_dice_attack_label->setText(QString("Attack dice: %1").arg(_hero->getNumDiceAttack()));
    if (hero_is_active)
        _num_dice_attack_label->setStyleSheet(HeroQuestLevelWindow::STATISTIC_BODY_STYLE_ACTIVE);
    else
        _num_dice_attack_label->setStyleSheet(HeroQuestLevelWindow::STATISTIC_BODY_STYLE_INACTIVE);

    // num_dice_defend
    _num_dice_defend_label->setText(QString("Defend dice: %1").arg(_hero->getNumDiceDefend()));
    if (hero_is_active)
        _num_dice_defend_label->setStyleSheet(HeroQuestLevelWindow::STATISTIC_BODY_STYLE_ACTIVE);
    else
        _num_dice_defend_label->setStyleSheet(HeroQuestLevelWindow::STATISTIC_BODY_STYLE_INACTIVE);

    // life_points
    _life_points_label->setText(QString("Life points: %1").arg(_hero->getLifePoints()));
    DV(
            ("update HeroStatisticsPane for hero %s (0x%x) to %d life points", qPrintable(_hero->getName()), (unsigned int)((Creature*)_hero), _hero->getLifePoints()));
    if (_hero->getLifePoints() <= 2)
        _life_points_label->setStyleSheet(HeroQuestLevelWindow::STATISTIC_BODY_STYLE_WARNING);
    else
    {
        if (hero_is_active)
            _life_points_label->setStyleSheet(HeroQuestLevelWindow::STATISTIC_BODY_STYLE_ACTIVE);
        else
            _life_points_label->setStyleSheet(HeroQuestLevelWindow::STATISTIC_BODY_STYLE_INACTIVE);
    }

    // intelligence_points
    _intelligence_points_label->setText(QString("Intelligence points: %1").arg(_hero->getIntelligencePoints()));
    if (hero_is_active)
        _intelligence_points_label->setStyleSheet(HeroQuestLevelWindow::STATISTIC_BODY_STYLE_ACTIVE);
    else
        _intelligence_points_label->setStyleSheet(HeroQuestLevelWindow::STATISTIC_BODY_STYLE_INACTIVE);

    // gold
    _gold_label->setText(QString("Gold: %1").arg(_hero->getGold()));
    if (hero_is_active)
        _gold_label->setStyleSheet(HeroQuestLevelWindow::STATISTIC_BODY_STYLE_ACTIVE);
    else
        _gold_label->setStyleSheet(HeroQuestLevelWindow::STATISTIC_BODY_STYLE_INACTIVE);

    // weapons
    _weapons_label->setText(QString("Weapons: (NYI)"));
    if (hero_is_active)
        _weapons_label->setStyleSheet(HeroQuestLevelWindow::STATISTIC_BODY_STYLE_ACTIVE);
    else
        _weapons_label->setStyleSheet(HeroQuestLevelWindow::STATISTIC_BODY_STYLE_INACTIVE);

    // inventory button
    if (hero_is_active && !_hero->getInventory().empty()
            && HeroQuestLevelWindow::_hero_quest->getUserInteractionMode()
                    == HeroQuestLevelWindow::USER_INTERACTION_NORMAL)
    {
        _inventory_button->setEnabled(true);
        _inventory_button->setStyleSheet(HeroQuestLevelWindow::INVENTORY_BUTTON_STYLE_ACTIVE);
    }
    else
    {
        _inventory_button->setEnabled(false);
        _inventory_button->setStyleSheet(HeroQuestLevelWindow::INVENTORY_BUTTON_STYLE_INACTIVE);
    }

    QLabel::update();
}

const Hero* HeroStatisticPane::getHero() const
{
    return _hero;
}

QPushButton* HeroStatisticPane::getInventoryButton()
{
    return _inventory_button;
}
