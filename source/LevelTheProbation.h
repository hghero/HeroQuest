#ifndef LEVEL_THE_PROBATION_H
#define LEVEL_THE_PROBATION_H

#include "Level.h"


class LevelTheProbation : public Level
{
public:
    LevelTheProbation(uint num_heroes);
	virtual ~LevelTheProbation();

	virtual QString getName() const;
	static QString prettyClassName();

    virtual QString getLevelBriefing() const;

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
