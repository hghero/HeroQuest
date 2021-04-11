//#define DEBUG

#include "SoundManager.h"

#include <iostream>

#include <QtCore/QString>
#include <QtMultimedia/QSound>

using namespace std;


SoundManager::SoundManager()
: _sounds()
{
	init();
}

SoundManager::~SoundManager()
{
    for (map<SoundID, QSound*>::iterator it = _sounds.begin(); it != _sounds.end(); ++it)
    {
        delete it->second;
    }
}

void SoundManager::init()
{
	loadSoundsFromFiles();
}

void SoundManager::playSoundOnce(const SoundID& sound_id) const
{
	map<SoundID, QSound*>::const_iterator it = _sounds.find(sound_id);
	if (it == _sounds.end())
	{
		cout << "Error: no sound with ID " << int(sound_id) << " found" << endl;
		return;
	}

	QSound* sound = it->second;
	if (sound == 0)
	{
		cout << "Error: sound with ID " << int(sound_id) << " is 0!" << endl;
		return;
	}

	sound->play();
}

bool SoundManager::loadSoundsFromFiles()
{
    if (!loadSoundFromFile(SOUND_CHEST_OPENS, ":/sounds/chest_opens.wav"))
        return false;

	if (!loadSoundFromFile(SOUND_CREATURE_MOVES, ":/sounds/creature_moves.wav"))
		return false;

	if (!loadSoundFromFile(SOUND_DOOR_OPENS, ":/sounds/door_opens.wav"))
		return false;

	if (!loadSoundFromFile(SOUND_SECRET_DOOR_OPENS, ":/sounds/secret_door_opens.wav"))
		return false;

	if (!loadSoundFromFile(SOUND_PIT_TRAP_TRIGGERS, ":/sounds/pit_trap_triggers.wav"))
		return false;

	if (!loadSoundFromFile(SOUND_SPEAR_TRAP_TRIGGERS, ":/sounds/spear_trap_triggers.wav"))
		return false;

	if (!loadSoundFromFile(SOUND_HERO_DIES, ":/sounds/hero_dies.wav"))
		return false;

	if (!loadSoundFromFile(SOUND_MONSTER_DIES, ":/sounds/monster_dies.wav"))
		return false;

	if (!loadSoundFromFile(SOUND_ATTACK_HERO_SMALL_DECREASE_LIFE_POINTS, ":/sounds/attack_hero_small_decrease_life_points.wav"))
		return false;

	if (!loadSoundFromFile(SOUND_ATTACK_MONSTER_SMALL_DECREASE_LIFE_POINTS, ":/sounds/attack_monster_small_decrease_life_points.wav"))
		return false;

	if (!loadSoundFromFile(SOUND_ATTACK_HERO_BIG_DECREASE_LIFE_POINTS, ":/sounds/attack_hero_big_decrease_life_points.wav"))
		return false;

	if (!loadSoundFromFile(SOUND_ATTACK_MONSTER_BIG_DECREASE_LIFE_POINTS, ":/sounds/attack_monster_big_decrease_life_points.wav"))
		return false;

	if (!loadSoundFromFile(SOUND_ATTACK_NO_SKULLS, ":/sounds/attack_no_skulls.wav"))
		return false;

	if (!loadSoundFromFile(SOUND_ATTACK_DEFENDED, ":/sounds/attack_defended.wav"))
		return false;

	if (!loadSoundFromFile(SOUND_THROW_DICE, ":/sounds/throw_dice.wav"))
		return false;

	if (!loadSoundFromFile(DRAW_TREASURE_CARD_GOOD, ":/sounds/draw_treasure_card_good.wav"))
		return false;

    if (!loadSoundFromFile(DRAW_TREASURE_CARD_BAD, ":/sounds/draw_treasure_card_bad.wav"))
        return false;

    if (!loadSoundFromFile(CAST_SPELL, ":/sounds/cast_spell.wav"))
        return false;

    if (!loadSoundFromFile(SOUND_DISARM_PIT_TRAP, ":/sounds/disarm_pit_trap.wav"))
        return false;

	return true;
}

bool SoundManager::loadSoundFromFile(const SoundID& sound_id, const QString& filename)
{
    QSound* tmp_sound = new QSound(filename);
    if (tmp_sound == 0)
	{
	  cout << "Error loading sound from file \"" << qPrintable(filename) << "\"" << endl;
	  return false;
	}

	_sounds[sound_id] = tmp_sound;
	return true;
}
