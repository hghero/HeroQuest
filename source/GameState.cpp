#include "GameState.h"
#include "StreamUtils.h"
#include "Debug.h"

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

bool GameState::save(std::ostream& stream) const
{
    //DVX(("number of hero names: %d", uint(_hero_names.size())));

    // _hero_names
    StreamUtils::writeUInt(stream, _hero_names.size());
    for (uint i = 0; i < _hero_names.size(); ++i)
    {
        StreamUtils::write(stream, _hero_names[i]);
    }

    // _alb_spell_family
    uint alb_spell_family_uint = uint(_alb_spell_family);
    StreamUtils::writeUInt(stream, alb_spell_family_uint);

    // _level_names
    StreamUtils::writeUInt(stream, _level_names.size());
    for (uint i = 0; i < _level_names.size(); ++i)
    {
        StreamUtils::write(stream, _level_names[i]);
    }

    // _current_level
    StreamUtils::writeUInt(stream, (uint) _current_level);

    // _current_level_state
    StreamUtils::writeUInt(stream, uint(_current_level_state));

    return !stream.fail();
}

/*!
 * @param stream Stream from which to load the game
 * @return true if load was successful, false otherwise
 */
bool GameState::load(std::istream& stream)
{
    // _hero_names
    _hero_names.clear();
    uint num_hero_names = 0;
    StreamUtils::readUInt(stream, &num_hero_names);
    if (stream.fail())
    {
        DVX(("Number of heroes is not a number."));
        return false;
    }
    //DVX(("num_hero_names = %d", num_hero_names));
    _hero_names.reserve(num_hero_names);
    for (uint i = 0; i < num_hero_names; ++i)
    {
        QString hero_name;
        StreamUtils::read(stream, &hero_name);

        if (hero_name != "Barbarian" && hero_name != "Dwarf" && hero_name != "Alb" && hero_name != "Magician")
        {
            DVX(("unknown hero name \"%s\"", qPrintable(hero_name)));
            return false;
        }

        _hero_names.push_back(hero_name);
        //DVX(("hero_names: %s", qPrintable(hero_name)));
    }

    // _alb_spell_family
    uint alb_spell_family_uint = 0;
    StreamUtils::readUInt(stream, &alb_spell_family_uint);
    if (stream.fail())
    {
        DVX(("Could not read Alb spell family"));
        return false;
    }
    _alb_spell_family = SpellCard::SpellFamily(alb_spell_family_uint);

    // _level_names
    _level_names.clear();
    uint num_level_names = 0;
    StreamUtils::readUInt(stream, &num_level_names);
    if (stream.fail())
    {
        DVX(("Number of levels is not a number."));
        return false;
    }
    _level_names.reserve(num_level_names);
    for (uint i = 0; i < num_level_names; ++i)
    {
        QString level_name;
        StreamUtils::read(stream, &level_name);

        _level_names.push_back(level_name);
    }

    // _current_level
    uint current_level_uint;
    StreamUtils::readUInt(stream, &current_level_uint);
    _current_level = (LevelID) current_level_uint;

    // _current_level_state
    uint current_level_state = 0;
    StreamUtils::readUInt(stream, &current_level_state);
    if (current_level_state != BRIEFING && current_level_state != RUNNING)
    {
        DVX(("Unknown current level state (%d)!", current_level_state));
        return false;
    }
    _current_level_state = (CurrentLevelState)current_level_state;

    return !stream.fail();
}
