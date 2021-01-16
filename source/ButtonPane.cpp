#include "ButtonPane.h"

#include <stdlib.h>

#include <QtWidgets/QLayout>
#include <QtWidgets/QPushButton>

#include "Debug.h"
#include "HeroQuestLevelWindow.h"
#include "Level.h"
#include "Playground.h"
#include "QuestBoard.h"
//#include "Watcher.h"
//#include "SoundManager.h"

using namespace std;

ButtonPane::ButtonPane()
: QLabel(),
  _button_ids(),
  _buttons()
{
    _button_ids["Search Secret Doors / Traps"] = 0;
    _button_ids["Search Treasures"] = 1;
    _button_ids["Delay Turn"] = 2;
    _button_ids["End Turn"] = 3;

	QVBoxLayout* vertical_layout = new QVBoxLayout;
	setLayout(vertical_layout);
	vertical_layout->setAlignment(Qt::AlignTop);
	vertical_layout->setMargin(0);
	vertical_layout->setSpacing(20);

	vector<QString> button_captions_ordered(_button_ids.size());
	for (map<QString, uint>::const_iterator it = _button_ids.begin(); it != _button_ids.end(); ++it)
	{
	    button_captions_ordered[it->second] = it->first;
	}

    for (vector<QString>::const_iterator it = button_captions_ordered.begin(); it != button_captions_ordered.end(); ++it)
    {
        _buttons.push_back(new QPushButton(*it, this));
    }

	for (vector<QPushButton*>::iterator it = _buttons.begin(); it != _buttons.end(); ++it)
	{
	    (*it)->setStyleSheet(HeroQuestLevelWindow::BUTTON_STYLE_INACTIVE);
	    vertical_layout->addWidget(*it);
	}

	setMinimumSize(vertical_layout->sizeHint());
}

ButtonPane::~ButtonPane()
{
    // NIX
}

void ButtonPane::setAllInactive()
{
    for (vector<QPushButton*>::iterator it = _buttons.begin(); it != _buttons.end(); ++it)
    {
        (*it)->setStyleSheet(HeroQuestLevelWindow::BUTTON_STYLE_INACTIVE);
        (*it)->setEnabled(false);
    }
}

void ButtonPane::updateButtons(const Level::HeroActionStates& hero_action_states)
{
    DV(("ButtonPane::updateButtons BEGIN"));
    Hero* current_hero = 0;
    if (HeroQuestLevelWindow::_hero_quest->getLevel() != 0)
        current_hero = dynamic_cast<Hero*>(HeroQuestLevelWindow::_hero_quest->getLevel()->getCurrentlyActingCreature());

    const NodeID* hero_node_id = 0;
    if (HeroQuestLevelWindow::_hero_quest->getPlayground() != 0)
        hero_node_id = HeroQuestLevelWindow::_hero_quest->getPlayground()->getCurrentlyActingCreaturePos();

    uint current_hero_room_id = 0;
    if (hero_node_id != 0)
        current_hero_room_id = HeroQuestLevelWindow::_hero_quest->getPlayground()->getQuestBoard()->getRoomID(*hero_node_id);


    // -----------------------------
    bool current_hero_sees_monster = HeroQuestLevelWindow::_hero_quest->getPlayground()->heroSeesMonster(current_hero);
    bool current_hero_is_caught_in_pit_trap = HeroQuestLevelWindow::_hero_quest->getLevel()->creatureIsCaughtInPitTrap(*current_hero);
    if (!current_hero_sees_monster &&
            HeroQuestLevelWindow::_hero_quest->getLevel()->getCurrentlyActingHeroActionStates() != 0 &&
            !HeroQuestLevelWindow::_hero_quest->getLevel()->getCurrentlyActingHeroActionStates()->_has_searched &&
            !HeroQuestLevelWindow::_hero_quest->getLevel()->getCurrentlyActingHeroActionStates()->_has_cast_spell
            &&
            !current_hero_is_caught_in_pit_trap
            && HeroQuestLevelWindow::_hero_quest->getUserInteractionMode()
                    == HeroQuestLevelWindow::USER_INTERACTION_NORMAL)
    {
        _buttons[_button_ids["Search Secret Doors / Traps"]]->setStyleSheet(HeroQuestLevelWindow::BUTTON_STYLE_ACTIVE);
        _buttons[_button_ids["Search Secret Doors / Traps"]]->setEnabled(true);
    }
    else
    {
        _buttons[_button_ids["Search Secret Doors / Traps"]]->setStyleSheet(HeroQuestLevelWindow::BUTTON_STYLE_INACTIVE);
        _buttons[_button_ids["Search Secret Doors / Traps"]]->setEnabled(false);
    }


    // -----------------------------
    if (!current_hero_sees_monster &&
            (current_hero_room_id != 0) &&
            !HeroQuestLevelWindow::_hero_quest->getLevel()->getCurrentlyActingHeroActionStates()->_has_searched &&
            !HeroQuestLevelWindow::_hero_quest->getLevel()->getCurrentlyActingHeroActionStates()->_has_cast_spell
            &&
            !current_hero_is_caught_in_pit_trap &&
            (HeroQuestLevelWindow::_hero_quest->getLevel()->roomContainsClosedChest(current_hero_room_id) // open chest
            || (!HeroQuestLevelWindow::_hero_quest->getLevel()->roomContainsChest(current_hero_room_id) // draw treasure card
                            && !HeroQuestLevelWindow::_hero_quest->getLevel()->hasRoomBeenSearchedForTreasures(
                                    current_hero_room_id)))
            && HeroQuestLevelWindow::_hero_quest->getUserInteractionMode()
                    == HeroQuestLevelWindow::USER_INTERACTION_NORMAL)
    {
        _buttons[_button_ids["Search Treasures"]]->setStyleSheet(HeroQuestLevelWindow::BUTTON_STYLE_ACTIVE);
        _buttons[_button_ids["Search Treasures"]]->setEnabled(true);
    }
    else
    {
        _buttons[_button_ids["Search Treasures"]]->setStyleSheet(HeroQuestLevelWindow::BUTTON_STYLE_INACTIVE);
        _buttons[_button_ids["Search Treasures"]]->setEnabled(false);
    }


    // -----------------------------
    if (!hero_action_states._has_acted_at_all
            && HeroQuestLevelWindow::_hero_quest->getUserInteractionMode()
                    == HeroQuestLevelWindow::USER_INTERACTION_NORMAL)
    {
        _buttons[_button_ids["Delay Turn"]]->setStyleSheet(HeroQuestLevelWindow::BUTTON_STYLE_ACTIVE);
        _buttons[_button_ids["Delay Turn"]]->setEnabled(true);
    }
    else
    {
        _buttons[_button_ids["Delay Turn"]]->setStyleSheet(HeroQuestLevelWindow::BUTTON_STYLE_INACTIVE);
        _buttons[_button_ids["Delay Turn"]]->setEnabled(false);
    }


    // -----------------------------
    if (HeroQuestLevelWindow::_hero_quest->getUserInteractionMode() == HeroQuestLevelWindow::USER_INTERACTION_NORMAL)
    {
        _buttons[_button_ids["End Turn"]]->setStyleSheet(HeroQuestLevelWindow::BUTTON_STYLE_ACTIVE);
        _buttons[_button_ids["End Turn"]]->setEnabled(true);
    }
    else
    {
        _buttons[_button_ids["End Turn"]]->setStyleSheet(HeroQuestLevelWindow::BUTTON_STYLE_INACTIVE);
        _buttons[_button_ids["End Turn"]]->setEnabled(false);
    }

    DV(("ButtonPane::updateButtons: update"));
    update();

    DV(("ButtonPane::updateButtons END"));
}

QPushButton* ButtonPane::getButton(const QString& str_id)
{
    return _buttons[_button_ids[str_id]];
}
