#ifndef MONSTER_H
#define MONSTER_H

#include "Creature.h"

// ==================================================================

class Monster : public Creature
{
private:
	static const QString ICON_FILENAME;
	typedef Creature Parent;

public:
	Monster(const QString& name, int num_move_points, int num_dice_attack, int num_dice_defend, int life_points, int life_points_max, int intelligence_points);
	virtual ~Monster();

	virtual const QString& getIconFilename() const;

	int getNumMovePoints() const;

	virtual bool isHero() const;
	virtual bool isMonster() const;

	virtual Creature& operator=(const Monster& other);
	virtual Creature* copy() const;

	static QString getName(const Monster* monster);

    virtual bool save(std::ostream& stream) const;
    virtual bool load(std::istream& stream);

protected:
	// movement
	int _num_move_points;
};

// ==================================================================

class Goblin : public Monster
{
private:
	static const QString ICON_FILENAME;

public:
	Goblin();
	virtual ~Goblin();

	virtual const QString& getIconFilename() const;

	virtual Creature* copy() const;
};

// ==================================================================

class Orc : public Monster
{
private:
	static const QString ICON_FILENAME;

public:
	Orc();
	virtual ~Orc();

	virtual const QString& getIconFilename() const;

	virtual Creature* copy() const;
};

// ==================================================================

class Fimir : public Monster
{
private:
	static const QString ICON_FILENAME;

public:
	Fimir();
	virtual ~Fimir();

	virtual const QString& getIconFilename() const;

	virtual Creature* copy() const;
};

// ==================================================================

class Skeleton : public Monster
{
private:
	static const QString ICON_FILENAME;

public:
	Skeleton();
	virtual ~Skeleton();

	virtual const QString& getIconFilename() const;

	virtual Creature* copy() const;
};

// ==================================================================

class Zombie : public Monster
{
private:
	static const QString ICON_FILENAME;

public:
	Zombie();
	virtual ~Zombie();

	virtual const QString& getIconFilename() const;

	virtual Creature* copy() const;
};

// ==================================================================

class Mummy : public Monster
{
private:
	static const QString ICON_FILENAME;

public:
	Mummy();
	virtual ~Mummy();

	virtual const QString& getIconFilename() const;

	virtual Creature* copy() const;
};

// ==================================================================

class ChaosWarrior : public Monster
{
private:
	static const QString ICON_FILENAME;

public:
	ChaosWarrior();
	virtual ~ChaosWarrior();

	virtual const QString& getIconFilename() const;

	virtual Creature* copy() const;
};

// ==================================================================

class Gargoyle : public Monster
{
private:
	static const QString ICON_FILENAME;

public:
	Gargoyle();
	virtual ~Gargoyle();

	virtual const QString& getIconFilename() const;

	virtual Creature* copy() const;
};

// ==================================================================

#endif
