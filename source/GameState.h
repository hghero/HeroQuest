#ifndef GAME_STATE_H
#define GAME_STATE_H

#include <vector>
#include <fstream>
#include <iostream>

#include <QtCore/QString>

#include "SpellCard.h"

class SaveContext;
class LoadContext;

struct GameState
{
    enum CurrentLevelState {
    BRIEFING = 0,
    RUNNING
    };

    enum LevelID
    {
        LEVEL_THE_PROBATION = 0, LEVEL_SAVING_SIR_RAGNAR
    };

	GameState();
	virtual ~GameState();

    virtual bool save(SaveContext& save_context) const;
    virtual bool load(LoadContext& load_context);

    std::vector<QString> _hero_names;
    SpellCard::SpellFamily _alb_spell_family;
    std::vector<QString> _level_names;
    LevelID _current_level;
    CurrentLevelState _current_level_state;
};

#endif
