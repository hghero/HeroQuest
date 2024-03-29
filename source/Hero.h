#ifndef HERO_H
#define HERO_H

#include <vector>

#include "Creature.h"
#include "Inventory.h"
#include "SpellCard.h"

class SaveContext;
class LoadContext;

// ==================================================================

class Hero : public Creature
{
private:
	static const QString ICON_FILENAME;
	typedef Creature Parent;

public:
    Hero(const QString& name, int num_dice_move, int num_dice_attack, int num_dice_defend, int life_points,
            int life_points_max, int intelligence_points);
	virtual ~Hero();

	virtual const QString& getIconFilename() const;

	int getNumDiceMove() const;
	void setNumDiceMoveExtra(int num_dice_move_extra);
	
	bool getCanMoveThroughWalls() const;
    void setCanMoveThroughWalls(bool value);
	
    virtual int getHighestNumDiceRegularAttack() const;
    virtual int getHighestNumDiceAttack(const Creature* defender) const;
	void setNumDiceAttackExtra(int num_dice_attack_extra);
    virtual int getNumDiceAttackExtra() const;

    virtual int getNumDiceDefend() const;
    virtual int getHighestNumDiceDefend() const;
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

    virtual bool save(SaveContext& save_context) const;
    virtual bool load(LoadContext& load_context);

	void updateTreasureImages();

    void addSpellFamily(SpellCard::SpellFamily family);

    void obtainSpellCardsFromStorage();

    int getNumAttackDiceDiagonallyAdjacent() const;
    int getNumAttackDiceDistance() const;

protected:
    int computeHighestNumDiceForRegularAttackFromEquipment() const;
    int computeAdditionalNumDiceForDefenceFromEquipment() const;

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

    // spells
    std::vector<SpellCard::SpellFamily> _spell_families;

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
    Barbarian();
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
    Dwarf();
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
    Alb();
	virtual ~Alb();

	virtual const QString& getIconFilename() const;
	virtual bool canCastSpells() const;

	static QString className();
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
    Magician();
	virtual ~Magician();

	virtual const QString& getIconFilename() const;
	virtual bool canCastSpells() const;

	static QString className();
};

// ==================================================================

/*!
 * Sir Ragnar appears only in the 2nd level "Saving Sir Ragnar".
 */
class SirRagnar : public Hero
{
    private:
    static const QString ICON_FILENAME;

    public:
    SirRagnar();
    virtual ~SirRagnar();

    virtual const QString& getIconFilename() const;

    static QString className();
};

// ==================================================================

#endif
