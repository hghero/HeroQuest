#ifndef DICE_ROLL_PANE_H
#define DICE_ROLL_PANE_H

#include <vector>

#include <QtWidgets/QLabel>

#include "Math.h"

class Hero;
class Creature;
class DiceRollArea;

/*!
 * A DiceRollPanel consists of 3 DiceRollArea objects: one for movement and
 * two for combat.
 */
class DiceRollPane : public QLabel
{
    Q_OBJECT
private:
	static const QString DICE_ONE_FILENAME;
	static const QString DICE_TWO_FILENAME;
	static const QString DICE_THREE_FILENAME;
	static const QString DICE_FOUR_FILENAME;
	static const QString DICE_FIVE_FILENAME;
	static const QString DICE_SIX_FILENAME;
	static const QString DICE_SKULL_FILENAME;
	static const QString DICE_HUMAN_SHIELD_FILENAME;
	static const QString DICE_MONSTER_SHIELD_FILENAME;

public:
	enum AttackDiceResult {
		ATTACK_DICE_RESULT_SKULL = 0,
		ATTACK_DICE_RESULT_HUMAN_SHIELD,
		ATTACK_DICE_RESULT_MONSTER_SHIELD
	};

	DiceRollPane(int width, int field_size);
	virtual ~DiceRollPane();

	DiceRollArea* getMovementDiceRollArea();
	DiceRollArea* getAttackDiceRollArea();
	DiceRollArea* getDefendDiceRollArea();

	void dummyShowSomething(Hero* hero);

	void throwMovementDice(Hero& hero, std::vector<unsigned int>* dice_results);
    void throwMovementDice(Creature& creature, uint num_dice, std::vector<uint>* dice_results);
    void updateMovementDice(Hero& hero, const std::vector<unsigned int>& dice_results);
	void throwAttackDice(Creature& creature, std::vector<DiceRollPane::AttackDiceResult>* dice_results);
	void throwAttackDice(Creature& creature, unsigned int num_dice, std::vector<DiceRollPane::AttackDiceResult>* dice_results);
    void setAttackSkulls(Creature* creature, unsigned int num_skulls);
	void throwDefendDice(Creature& creature, std::vector<DiceRollPane::AttackDiceResult>* dice_results);
	void throwDefendDice(Creature& creature, unsigned int num_dice, std::vector<DiceRollPane::AttackDiceResult>* dice_results);

	void clearMovementDice();
	void clearAttackDice();
	void clearDefendDice();

private:
	//! movement and combat dice images: 0..5: movement (1..6); 6..8: combat (skull, human shield, monster_shield)
	std::vector<QPixmap*> _dice_images;
	std::vector<Vec2i> _dice_orig_sizes;

	//! width of this pane
	int _width;

	//! size (width and height) of the quest board fields
	int _field_size;

	//! movement and combat dice roll areas
	DiceRollArea* _movement_dice_roll_area;
	DiceRollArea* _attack_dice_roll_area;
	DiceRollArea* _defend_dice_roll_area;
};

#endif
