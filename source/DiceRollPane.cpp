#include "DiceRollPane.h"

#include <stdlib.h>

#include <QtWidgets/QLayout>

#include "Debug.h"
#include "Hero.h"
#include "HeroQuestLevelWindow.h"
#include "DiceRollArea.h"
#include "Level.h"
//#include "Watcher.h"
//#include "SoundManager.h"

using namespace std;

const QString DiceRollPane::DICE_ONE_FILENAME = ":/graphics/dice_one.jpg";
const QString DiceRollPane::DICE_TWO_FILENAME = ":/graphics/dice_two.jpg";
const QString DiceRollPane::DICE_THREE_FILENAME = ":/graphics/dice_three.jpg";
const QString DiceRollPane::DICE_FOUR_FILENAME = ":/graphics/dice_four.jpg";
const QString DiceRollPane::DICE_FIVE_FILENAME = ":/graphics/dice_five.jpg";
const QString DiceRollPane::DICE_SIX_FILENAME = ":/graphics/dice_six.jpg";
const QString DiceRollPane::DICE_SKULL_FILENAME = ":/graphics/dice_skull.jpg";
const QString DiceRollPane::DICE_HUMAN_SHIELD_FILENAME = ":/graphics/dice_human_shield.jpg";
const QString DiceRollPane::DICE_MONSTER_SHIELD_FILENAME = ":/graphics/dice_monster_shield.jpg";

DiceRollPane::DiceRollPane(int width, int field_size)
: QLabel(),
  _dice_images(9), // 6 movement + 3 combat
  _dice_orig_sizes(9), // 6 movement + 3 combat
  _width(width),
  _field_size(field_size),
  _movement_dice_roll_area(0),
  _attack_dice_roll_area(0),
  _defend_dice_roll_area(0)
{
	// load the dice images
    _dice_images[0] = new QPixmap(DICE_ONE_FILENAME.toUtf8().constData());
    if (!_dice_images[0]) {
        DVX(("%s not found or failed to load", qPrintable(DICE_ONE_FILENAME)));
    }

    _dice_images[1] = new QPixmap(DICE_TWO_FILENAME.toUtf8().constData());
    if (!_dice_images[1]) {
        DVX(("%s not found or failed to load", qPrintable(DICE_TWO_FILENAME)));
    }

    _dice_images[2] = new QPixmap(DICE_THREE_FILENAME.toUtf8().constData());
    if (!_dice_images[2]) {
        DVX(("%s not found or failed to load", qPrintable(DICE_THREE_FILENAME)));
    }

    _dice_images[3] = new QPixmap(DICE_FOUR_FILENAME.toUtf8().constData());
    if (!_dice_images[3]) {
        DVX(("%s not found or failed to load", qPrintable(DICE_FOUR_FILENAME)));
    }

	_dice_images[4] = new QPixmap(DICE_FIVE_FILENAME.toUtf8().constData());
    if (!_dice_images[4]) {
        DVX(("%s not found or failed to load", qPrintable(DICE_FIVE_FILENAME)));
    }

	_dice_images[5] = new QPixmap(DICE_SIX_FILENAME.toUtf8().constData());
    if (!_dice_images[5]) {
        DVX(("%s not found or failed to load", qPrintable(DICE_SIX_FILENAME)));
    }

	_dice_images[6] = new QPixmap(DICE_SKULL_FILENAME.toUtf8().constData());
    if (!_dice_images[6]) {
       DVX(("%s not found or failed to load", qPrintable(DICE_SKULL_FILENAME)));
    }

	_dice_images[7] = new QPixmap(DICE_HUMAN_SHIELD_FILENAME.toUtf8().constData());
    if (!_dice_images[7]) {
       DVX(("%s not found or failed to load", qPrintable(DICE_HUMAN_SHIELD_FILENAME)));
    }

	_dice_images[8] = new QPixmap(DICE_MONSTER_SHIELD_FILENAME.toUtf8().constData());
    if (!_dice_images[8]) {
       DVX(("%s not found or failed to load", qPrintable(DICE_MONSTER_SHIELD_FILENAME)));
    }

	// gather orig sizes
	for (size_t i = 0; i < _dice_orig_sizes.size(); ++i)
	{
		_dice_orig_sizes[i].x = _dice_images[i]->width();
		_dice_orig_sizes[i].y = _dice_images[i]->height();
	}

	// the sizes of avatars and dice are adjusted to the quest board field size
	Vec2i avatar_size(_field_size, _field_size);
	int dice_size = int(_field_size * 0.7);

    setStyleSheet(HeroQuestLevelWindow::DICE_ROLL_PANE_STYLE);

	/*
	 *  --------------------------------------------------------------------
	 * |                                                                    |
	 * |                      _movement_dice_roll_area                      |
	 * |                                                                    |
	 *  --------------------------------------------------------------------
	 * |                                 |                                  |
	 * |   _attack_dice_roll_area        |    _defend_dice_roll_area        |
	 * |                                 |                                  |
	 *  --------------------------------------------------------------------
	 */

	QVBoxLayout* vertical_layout = new QVBoxLayout;
	setLayout(vertical_layout);
	vertical_layout->setAlignment(Qt::AlignTop);
    vertical_layout->setContentsMargins(0, 0, 0, 0);
	vertical_layout->setSpacing(_field_size / 2);

	// top: _movement_dice_roll_area
	{
	    _movement_dice_roll_area = new DiceRollArea(_width, avatar_size, dice_size);
	    vertical_layout->addWidget(_movement_dice_roll_area);
	    _movement_dice_roll_area->setFixedHeight(_field_size);
	}

	// bottom: _attack_dice_roll_area, _defend_dice_roll_area
	{
	    QLabel* bottom_label = new QLabel;
	    vertical_layout->addWidget(bottom_label);
        vertical_layout->setContentsMargins(0, 0, 0, 0);
	    bottom_label->setFixedHeight(3 * _field_size);

	    QHBoxLayout* horizontal_layout = new QHBoxLayout;
	    bottom_label->setLayout(horizontal_layout);
        horizontal_layout->setContentsMargins(0, 0, 0, 0);
	    horizontal_layout->setSpacing(_field_size);

	    const int dice_roll_area_width = (_width - horizontal_layout->spacing()) / 2;

	    // bottom left
	    _attack_dice_roll_area = new DiceRollArea(dice_roll_area_width, avatar_size, dice_size);
	    horizontal_layout->addWidget(_attack_dice_roll_area);

	    // bottom right
	    _defend_dice_roll_area = new DiceRollArea(dice_roll_area_width, avatar_size, dice_size);
	    horizontal_layout->addWidget(_defend_dice_roll_area);
	}

	setMinimumSize(vertical_layout->sizeHint());
}

DiceRollPane::~DiceRollPane()
{
    for (size_t i = 0; i < _dice_images.size(); ++i)
        delete _dice_images[i];
}

DiceRollArea* DiceRollPane::getMovementDiceRollArea()
{
	return _movement_dice_roll_area;
}

DiceRollArea* DiceRollPane::getAttackDiceRollArea()
{
	return _attack_dice_roll_area;
}

DiceRollArea* DiceRollPane::getDefendDiceRollArea()
{
	return _defend_dice_roll_area;
}

#if 0
void DiceRollPane::handleMouseEvent(const ALLEGRO_EVENT& event)
{
	if (HeroQuestLevelWindow::blocksEvents())
		return;

	/*
	if (event.type != ALLEGRO_EVENT_MOUSE_AXES)
	    cout << "DiceRollPanel::handleMouseEvent" << endl;
	*/
}
#endif

// debug
void DiceRollPane::dummyShowSomething(Hero* hero)
{
    vector<QPixmap*> movement_dice_images;
    movement_dice_images.reserve(2);
    movement_dice_images.push_back(_dice_images[4]);
    movement_dice_images.push_back(_dice_images[2]);
    _movement_dice_roll_area->setContent(hero->getAvatar(), movement_dice_images);

    vector<QPixmap*> attack_dice_images;
    attack_dice_images.reserve(3);
    attack_dice_images.push_back(_dice_images[6]);
    attack_dice_images.push_back(_dice_images[7]);
    attack_dice_images.push_back(_dice_images[6]);
    _attack_dice_roll_area->setContent(hero->getAvatar(), attack_dice_images);
}

/*!
 * Uses _movement_dice_roll_area to roll the dice. Respects _num_dice_move_extra.
 */
void DiceRollPane::throwMovementDice(Hero& hero, vector<unsigned int>* dice_results)
{
	HeroQuestLevelWindow::_hero_quest->playSoundOnce(SoundManager::SOUND_THROW_DICE);

	_movement_dice_roll_area->clearContent();
	
	dice_results->clear();
	dice_results->reserve(hero.getNumDiceMove());

	vector<QPixmap*> dice_images;
	dice_images.reserve(hero.getNumDiceMove());

	for (int i = 0; i < hero.getNumDiceMove(); ++i)
	{
		// generate random number
		unsigned int dice_result = rand() % 6 + 1;

		dice_results->push_back(dice_result);
		dice_images.push_back(_dice_images[dice_result-1]);
	}

	_movement_dice_roll_area->setContent(hero.getAvatar(), dice_images);

	// reset extra movement dice
	hero.setNumDiceMoveExtra(0);
}

/*!
 * Uses _movement_dice_roll_area to roll the dice. Does _not_ respect _num_dice_move_extra.
 */
void DiceRollPane::throwMovementDice(Creature& creature, uint num_dice, vector<uint>* dice_results)
{
    HeroQuestLevelWindow::_hero_quest->playSoundOnce(SoundManager::SOUND_THROW_DICE);

    _movement_dice_roll_area->clearContent();

    dice_results->clear();
    dice_results->reserve(num_dice);

    vector<QPixmap*> dice_images;
    dice_images.reserve(num_dice);

    for (uint i = 0; i < num_dice; ++i)
    {
        // generate random number
        uint dice_result = rand() % 6 + 1;

        dice_results->push_back(dice_result);
        dice_images.push_back(_dice_images[dice_result - 1]);
    }

    _movement_dice_roll_area->setContent(creature.getAvatar(), dice_images);
}

/*!
 * Sets the avatar and dice in the movement dice roll area to represent the params given to this method.
 *
 * @param hero
 * @param dice_results
 */
void DiceRollPane::updateMovementDice(Hero& hero, const std::vector<unsigned int>& dice_results)
{
    _movement_dice_roll_area->clearContent();

    vector<QPixmap*> dice_images;
    dice_images.reserve(dice_results.size());

    for (uint i = 0; i < dice_results.size(); ++i)
    {
        dice_images.push_back(_dice_images[dice_results[i] - 1]);
    }

    _movement_dice_roll_area->setContent(hero.getAvatar(), dice_images);
}

void DiceRollPane::throwAttackDice(Creature& creature, vector<DiceRollPane::AttackDiceResult>* dice_results)
{
    unsigned int num_dice_attack = creature.getNumDiceAttack();

	// if a hero attacker sits in a pit trap, he attacks with 1 dice less
	if (creature.isHero() && HeroQuestLevelWindow::_hero_quest->getLevel()->creatureIsCaughtInPitTrap(creature) && num_dice_attack > 0)
		num_dice_attack -= 1;

	throwAttackDice(creature, num_dice_attack, dice_results);
}

void DiceRollPane::throwAttackDice(Creature& creature, unsigned int num_dice, vector<DiceRollPane::AttackDiceResult>* dice_results)
{
	HeroQuestLevelWindow::_hero_quest->playSoundOnce(SoundManager::SOUND_THROW_DICE);

	_attack_dice_roll_area->clearContent();

	dice_results->clear();
	dice_results->reserve(num_dice);

    vector<QPixmap*> dice_images;
    dice_images.reserve(num_dice);
	for (uint i = 0; i < num_dice; ++i)
	{
		// generate random number
		uint dice_result_num = rand() % 6 + 1;

		// 1, 2, 3: skull
		// 4, 5: human shield
		// 6: monster shield
		DiceRollPane::AttackDiceResult dice_result;
		if (dice_result_num == 1 || dice_result_num == 2 || dice_result_num == 3)
			dice_result = ATTACK_DICE_RESULT_SKULL;
		else if (dice_result_num == 4 || dice_result_num == 5)
			dice_result = ATTACK_DICE_RESULT_HUMAN_SHIELD;
		else
			dice_result = ATTACK_DICE_RESULT_MONSTER_SHIELD;

		dice_results->push_back(dice_result);
		dice_images.push_back(_dice_images[6 + int(dice_result)]);
	}

	_attack_dice_roll_area->setContent(creature.getAvatar(), dice_images);
}

/*!
 * Sets the attack area to num_skulls skulls. Shows creature's avatar if creature != 0.
 *
 * @param creature
 * @param num_skulls
 */
void DiceRollPane::setAttackSkulls(Creature* creature, unsigned int num_skulls)
{
    _attack_dice_roll_area->clearContent();

    vector<QPixmap*> dice_images(num_skulls, _dice_images[6]); // 6, 7, 8 = SKULL

    _attack_dice_roll_area->setContent(creature != 0 ? creature->getAvatar() : 0, dice_images);
}

void DiceRollPane::throwDefendDice(Creature& creature, vector<DiceRollPane::AttackDiceResult>* dice_results)
{
    uint num_dice_defend = creature.getHighestNumDiceDefend();

	// if a hero defender sits in a pit trap, he defends with 1 dice less
	if (creature.isHero() && HeroQuestLevelWindow::_hero_quest->getLevel()->creatureIsCaughtInPitTrap(creature) && num_dice_defend > 0)
		num_dice_defend -= 1;

	throwDefendDice(creature, num_dice_defend, dice_results);

    // Hero: reset extra attack dice
    Hero* hero = dynamic_cast<Hero*>(&creature);
    if (hero != 0)
        hero->setNumDiceDefendExtra(0);
}

void DiceRollPane::throwDefendDice(Creature& creature, unsigned int num_dice, vector<DiceRollPane::AttackDiceResult>* dice_results)
{
	HeroQuestLevelWindow::_hero_quest->playSoundOnce(SoundManager::SOUND_THROW_DICE);

	_defend_dice_roll_area->clearContent();

	dice_results->clear();

    vector<QPixmap*> dice_images;
    dice_images.reserve(num_dice);
	for (uint i = 0; i < num_dice; ++i)
	{
		// generate random number
		uint dice_result_num = rand() % 6 + 1;

		// 1, 2, 3: skull
		// 4, 5: human shield
		// 6: monster shield
		DiceRollPane::AttackDiceResult dice_result;
		if (dice_result_num == 1 || dice_result_num == 2 || dice_result_num == 3)
			dice_result = ATTACK_DICE_RESULT_SKULL;
		else if (dice_result_num == 4 || dice_result_num == 5)
			dice_result = ATTACK_DICE_RESULT_HUMAN_SHIELD;
		else
			dice_result = ATTACK_DICE_RESULT_MONSTER_SHIELD;

		dice_results->push_back(dice_result);
		dice_images.push_back(_dice_images[6 + int(dice_result)]);
	}

	_defend_dice_roll_area->setContent(creature.getAvatar(), dice_images);
}

void DiceRollPane::clearMovementDice()
{
    _movement_dice_roll_area->clearContent();
}

void DiceRollPane::clearAttackDice()
{
    _attack_dice_roll_area->clearContent();
}

void DiceRollPane::clearDefendDice()
{
    _defend_dice_roll_area->clearContent();
}
