#ifndef LEVEL_01_THE_PROBATION_H
#define LEVEL_01_THE_PROBATION_H

#include "Level.h"


class Level01TheProbation : public Level
{
public:
    Level01TheProbation(size_t num_heroes);
    virtual ~Level01TheProbation();

	virtual QString getName() const;
	static QString prettyClassName();

    virtual QString getLevelBriefing() const;

    protected:
    virtual bool removeMonster(Monster* monster);

private:
	virtual void placeHeroes();
	virtual void createMonsters();
	virtual void createDoors();
	virtual void createTraps();
	virtual void createDecoration();
    virtual void createChestTreasureDescriptions();
	virtual const Monster* getRoamingMonsterType() const;
};

#endif
