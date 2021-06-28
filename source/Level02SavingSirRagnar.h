#ifndef LEVEL_02_SAVING_SIR_RAGNAR_H
#define LEVEL_02_SAVING_SIR_RAGNAR_H

#include "Level.h"

class SaveContext;
class LoadContext;

class Level02SavingSirRagnar : public Level
{
public:
    Level02SavingSirRagnar(uint num_heroes);
    virtual ~Level02SavingSirRagnar();

	virtual QString getName() const;
	static QString prettyClassName();

    virtual QString getLevelBriefing() const;

    virtual bool save(SaveContext& save_context) const;
    virtual bool load(LoadContext& load_context);

    protected:
    virtual void fieldHasBeenReachedBy(const Creature* creature, const NodeID& node);
    virtual void doorHasBeenOpenedBy(const Creature* creature, const Door* door);

    virtual void searchTrapsButtonClicked();

    virtual void obtainLevelReward();

private:
    enum LevelCompletionState
    {
        LEVEL_COMPLETION_STATE_SEARCHING_SIR_RAGNAR = 0, LEVEL_COMPLETION_STATE_FOUND_SIR_RAGNAR
    };

    void handleSirRagnarFound();

	virtual void placeHeroes();
	virtual void createMonsters();
	virtual void createDoors();
	virtual void createTraps();
	virtual void createDecoration();
    virtual void createChestTreasureDescriptions();
	virtual const Monster* getRoamingMonsterType() const;

    LevelCompletionState _level_completion_state;
};

#endif
