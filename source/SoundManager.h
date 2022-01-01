#ifndef SOUND_MANAGER_H
#define SOUND_MANAGER_H

#include <map>
#include <memory>


class QMediaPlayer;
class QString;


class SoundManager
{
public:
	enum SoundID {
        SOUND_CHEST_OPENS = 0,
        SOUND_CREATURE_MOVES,
		SOUND_DOOR_OPENS,
		SOUND_SECRET_DOOR_OPENS,
		SOUND_PIT_TRAP_TRIGGERS,
		SOUND_SPEAR_TRAP_TRIGGERS,
		SOUND_HERO_DIES,
		SOUND_MONSTER_DIES,
		SOUND_ATTACK_HERO_SMALL_DECREASE_LIFE_POINTS,
		SOUND_ATTACK_HERO_BIG_DECREASE_LIFE_POINTS,
		SOUND_ATTACK_MONSTER_SMALL_DECREASE_LIFE_POINTS,
		SOUND_ATTACK_MONSTER_BIG_DECREASE_LIFE_POINTS,
		SOUND_ATTACK_NO_SKULLS,
		SOUND_ATTACK_DEFENDED,
		SOUND_THROW_DICE,
		SOUND_DRAW_TREASURE_CARD_GOOD,
        SOUND_DRAW_TREASURE_CARD_BAD,
        SOUND_CAST_SPELL,
        SOUND_DISARM_PIT_TRAP,
        SOUND_SIREN
	};

	SoundManager();
	~SoundManager();

	void init();

	void playSoundOnce(const SoundID& sound_id) const;

private:
	bool assignSoundFilenames();
	bool loadSoundFromFile(const SoundID& sound_id, const QString& filename);

	std::map<SoundID, QString> _sounds; // <ID, filename>
	std::unique_ptr<QMediaPlayer> _player;
};

#endif
