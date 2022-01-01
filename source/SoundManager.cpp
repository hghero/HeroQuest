//#define DEBUG

#include "SoundManager.h"

#include <iostream>

#include <QtCore/QString>
#include <QtMultimedia/QMediaPlayer>
#include <QtMultimedia/QAudioOutput>

using namespace std;


SoundManager::SoundManager()
: _sounds(),
  _player(new QMediaPlayer)
{
    QAudioOutput* audio_output = new QAudioOutput;
    audio_output->setVolume(50);
    _player->setAudioOutput(audio_output);

    init();
}

SoundManager::~SoundManager()
{
}

void SoundManager::init()
{
	assignSoundFilenames();
}

void SoundManager::playSoundOnce(const SoundID& sound_id) const
{
	map<SoundID, QString>::const_iterator it = _sounds.find(sound_id);
	if (it == _sounds.end())
	{
		cout << "Error: no sound with ID " << int(sound_id) << " found" << endl;
		return;
	}

	_player->setSource(QUrl(it->second));
	_player->play();
}

bool SoundManager::assignSoundFilenames()
{
    if (!loadSoundFromFile(SOUND_CHEST_OPENS, "qrc:/sounds/chest_opens.wav"))
        return false;

	if (!loadSoundFromFile(SOUND_CREATURE_MOVES, "qrc:/sounds/creature_moves.wav"))
		return false;

	if (!loadSoundFromFile(SOUND_DOOR_OPENS, "qrc:/sounds/door_opens.wav"))
		return false;

	if (!loadSoundFromFile(SOUND_SECRET_DOOR_OPENS, "qrc:/sounds/secret_door_opens.wav"))
		return false;

	if (!loadSoundFromFile(SOUND_PIT_TRAP_TRIGGERS, "qrc:/sounds/pit_trap_triggers.wav"))
		return false;

	if (!loadSoundFromFile(SOUND_SPEAR_TRAP_TRIGGERS, "qrc:/sounds/spear_trap_triggers.wav"))
		return false;

	if (!loadSoundFromFile(SOUND_HERO_DIES, "qrc:/sounds/hero_dies.wav"))
		return false;

	if (!loadSoundFromFile(SOUND_MONSTER_DIES, "qrc:/sounds/monster_dies.wav"))
		return false;

	if (!loadSoundFromFile(SOUND_ATTACK_HERO_SMALL_DECREASE_LIFE_POINTS, "qrc:/sounds/attack_hero_small_decrease_life_points.wav"))
		return false;

	if (!loadSoundFromFile(SOUND_ATTACK_MONSTER_SMALL_DECREASE_LIFE_POINTS, "qrc:/sounds/attack_monster_small_decrease_life_points.wav"))
		return false;

	if (!loadSoundFromFile(SOUND_ATTACK_HERO_BIG_DECREASE_LIFE_POINTS, "qrc:/sounds/attack_hero_big_decrease_life_points.wav"))
		return false;

	if (!loadSoundFromFile(SOUND_ATTACK_MONSTER_BIG_DECREASE_LIFE_POINTS, "qrc:/sounds/attack_monster_big_decrease_life_points.wav"))
		return false;

	if (!loadSoundFromFile(SOUND_ATTACK_NO_SKULLS, "qrc:/sounds/attack_no_skulls.wav"))
		return false;

	if (!loadSoundFromFile(SOUND_ATTACK_DEFENDED, "qrc:/sounds/attack_defended.wav"))
		return false;

	if (!loadSoundFromFile(SOUND_THROW_DICE, "qrc:/sounds/throw_dice.wav"))
		return false;

	if (!loadSoundFromFile(SOUND_DRAW_TREASURE_CARD_GOOD, "qrc:/sounds/draw_treasure_card_good.wav"))
		return false;

    if (!loadSoundFromFile(SOUND_DRAW_TREASURE_CARD_BAD, "qrc:/sounds/draw_treasure_card_bad.wav"))
        return false;

    if (!loadSoundFromFile(SOUND_CAST_SPELL, "qrc:/sounds/cast_spell.wav"))
        return false;

    if (!loadSoundFromFile(SOUND_DISARM_PIT_TRAP, "qrc:/sounds/disarm_pit_trap.wav"))
        return false;

    if (!loadSoundFromFile(SOUND_SIREN, "qrc:/sounds/siren.wav"))
        return false;

	return true;
}

bool SoundManager::loadSoundFromFile(const SoundID& sound_id, const QString& filename)
{
 	_sounds[sound_id] = filename;
	return true;
}
