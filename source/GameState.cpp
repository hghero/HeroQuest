#include "GameState.h"
#include "StreamUtils.h"
#include "Debug.h"
#include "SaveContext.h"
#include "LoadContext.h"

using namespace std;

GameState::GameState()
: _hero_names(),
  _alb_spell_family(SpellCard::AIR),
  _level_names(),
  _current_level(LEVEL_THE_PROBATION),
  _current_level_state(BRIEFING)
{
    // NIX
}

GameState::~GameState()
{
    // NIX
}

bool GameState::save(SaveContext& save_context) const
{
    SaveContext::OpenChapter open_chapter(save_context, "GameState");

    // _hero_names
    {
        SaveContext::OpenChapter open_chapter_hero_names(save_context, "_hero_names");
        save_context.writeUInt(uint(_hero_names.size()), "size");
        for (size_t i = 0; i < _hero_names.size(); ++i)
        {
            save_context.writeString(_hero_names[i], "hero name");
        }
    }

    // _alb_spell_family
    uint alb_spell_family_uint = uint(_alb_spell_family);
    save_context.writeUInt(alb_spell_family_uint, "_alb_spell_family");

    // _level_names
    {
        SaveContext::OpenChapter open_chapter_level_names(save_context, "_level_names");
        save_context.writeUInt(uint(_level_names.size()), "_level_names.size()");
        for (size_t i = 0; i < _level_names.size(); ++i)
        {
            save_context.writeString(_level_names[i], "level name");
        }
    }

    // _current_level
    save_context.writeUInt((uint) _current_level, "_current_level");

    // _current_level_state
    save_context.writeUInt((uint) _current_level_state, "_current_level_state");

    return !save_context.fail();
}

/*!
 * @param stream Stream from which to load the game
 * @return true if load was successful, false otherwise
 */
bool GameState::load(LoadContext& load_context)
{
    LoadContext::OpenChapter open_chapter(load_context, "GameState");

    // _hero_names
    _hero_names.clear();
    uint num_hero_names = 0;
    load_context.readUInt(&num_hero_names, "_hero_names.size()");
    if (load_context.fail())
    {
        DVX(("Number of heroes is not a number."));
        return false;
    }
    _hero_names.reserve(num_hero_names);
    for (uint i = 0; i < num_hero_names; ++i)
    {
        QString hero_name;
        QString i_str;
        i_str.setNum(i);
        load_context.readString(&hero_name, QString("hero_names[") + i_str + "]");

        _hero_names.push_back(hero_name);
    }

    // _alb_spell_family
    uint alb_spell_family_uint = 0;
    load_context.readUInt(&alb_spell_family_uint, "_alb_spell_family");
    if (load_context.fail())
    {
        DVX(("Could not read Alb spell family"));
        return false;
    }
    _alb_spell_family = SpellCard::SpellFamily(alb_spell_family_uint);

    // _level_names
    _level_names.clear();
    uint num_level_names = 0;
    load_context.readUInt(&num_level_names, "_level_names.size()");
    if (load_context.fail())
    {
        DVX(("Number of levels is not a number."));
        return false;
    }
    _level_names.reserve(num_level_names);
    for (uint i = 0; i < num_level_names; ++i)
    {
        QString level_name;
        QString i_str;
        i_str.setNum(i);
        load_context.readString(&level_name, QString("level[") + i_str + "]");

        _level_names.push_back(level_name);
    }

    // _current_level
    uint current_level_uint;
    load_context.readUInt(&current_level_uint, "_current_level");
    _current_level = (LevelID) current_level_uint;

    // _current_level_state
    uint current_level_state = 0;
    load_context.readUInt(&current_level_state, "_current_level_state");
    if (current_level_state != BRIEFING && current_level_state != RUNNING)
    {
        DVX(("Unknown current level state (%d)!", current_level_state));
        return false;
    }
    _current_level_state = (CurrentLevelState)current_level_state;

    return !load_context.fail();
}
