#ifndef HERO_H
#define HERO_H

#include <vector>

#include "Creature.h"
#include "Inventory.h"
#include "SpellCard.h"

// ==================================================================

class Hero : public Creature
{
private:
	static const QString ICON_FILENAME;
	typedef Creature Parent;

public:
	//Hero();
	Hero(const QString& name, int num_dice_move, int num_dice_attack, int num_dice_defend, int life_points, int life_points_max, int intelligence_points, bool add_to_level = true);
	virtual ~Hero();

	virtual const QString& getIconFilename() const;

	int getNumDiceMove() const;
	void setNumDiceMoveExtra(int num_dice_move_extra);
	
	bool getCanMoveThroughWalls() const;
    void setCanMoveThroughWalls(bool value);
	
	virtual int getNumDiceAttack() const;
	void setNumDiceAttackExtra(int num_dice_attack_extra);

    virtual int getNumDiceDefend() const;
    void setNumDiceDefendExtra(int num_dice_defend_extra);

	int getGold() const; // deprecated
	void setGold(int gold); // deprecated

	Inventory& getInventory();
	const Inventory& getInventory() const;

	virtual bool isHero() const;
	virtual bool isMonster() const;

	virtual bool canCastSpells() const;

	bool checkApplySpell();
	bool checkDefend();
	bool checkDeath();

	virtual bool save(std::ostream& stream) const;
	virtual bool load(std::istream& stream);

	void updateTreasureImages();

protected:
	// movement
	int _num_dice_move;
	int _num_dice_move_extra;
    bool _can_move_through_walls;

	// attack
	int _num_dice_attack_extra;

	// defend
	int _num_dice_defend_extra;

	// equipment and valuables
	Inventory _inventory;

private:
	virtual Creature& operator=(const Hero& other);
	virtual Creature* copy() const;
};

// ==================================================================

class Barbarian : public Hero
{
private:
	static const QString ICON_FILENAME;

public:
	Barbarian(bool add_to_level = true);
	virtual ~Barbarian();

	virtual const QString& getIconFilename() const;

	static QString className();
};

// ==================================================================

class Dwarf : public Hero
{
private:
	static const QString ICON_FILENAME;

public:
	Dwarf(bool add_to_level = true);
	virtual ~Dwarf();

	virtual const QString& getIconFilename() const;

	static QString className();
};

// ==================================================================

/*!
 * The Alb has 1 pile of spell cards.
 */
class Alb : public Hero
{
private:
	static const QString ICON_FILENAME;

public:
	Alb(bool add_to_level = true);
	virtual ~Alb();

	virtual const QString& getIconFilename() const;
	virtual bool canCastSpells() const;
	void setSpellFamily(SpellCard::SpellFamily& family);
	const SpellCard::SpellFamily& getSpellFamily() const;

	static QString className();

private:
	SpellCard::SpellFamily _spell_family;
};

// ==================================================================

/*!
 * The Magician has the other 3 piles of spell cards.
 */
class Magician : public Hero
{
private:
	static const QString ICON_FILENAME;

public:
	Magician(bool add_to_level = true);
	virtual ~Magician();

	virtual const QString& getIconFilename() const;
	virtual bool canCastSpells() const;

	static QString className();

private:
    SpellCard::SpellFamily _spell_family;
};

// ==================================================================

#endif
