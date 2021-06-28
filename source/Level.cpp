//#define HG_DEBUG

#include "Level.h"

#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>

#include <QtWidgets/QDialog>
#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtCore/QString>

#include "Preferences.h"
#include "Hero.h"
#include "HeroQuestLevelWindow.h"
#include "Debug.h"
#include "QuestBoard.h"
#include "Playground.h"
#include "DiceRollPane.h"
#include "SecretDoor.h"
#include "NonWalkableDecoration.h"
#include "WalkableDecoration.h"
#include "Command.h"
#include "SoundManager.h"
#include "ThreadUtils.h"
#include "StreamUtils.h"
#include "SpellCardStorage.h"
#include "DialogGameOver.h"
#include "EventBlockingContext.h"
#include "TreasureCardStorage.h"
#include "HeroCamp.h"
#include "SaveContext.h"
#include "LoadContext.h"

using namespace std;

// delays in ms
const unsigned int Level::MOVE_DELAY = 50;
const unsigned int Level::ATTACK_DELAY = 1000;
const unsigned int Level::ATTACK_FINISHED_DELAY = 500;
const unsigned int Level::TRAP_TRIGGER_DELAY = 1000;


// ==================================================================

Level::ActionStates::ActionStates()
:
_has_acted_at_all(false),
_has_moved(false),
_num_attacks_left(1),
_has_cast_spell(false),
_num_spell_casts_left(1),
 _move_points(0), _sleeping(false)
{
}

Level::ActionStates::~ActionStates()
{
    // NIX
}

void Level::ActionStates::reset()
{
    Creature* creature = 0;
    if (HeroQuestLevelWindow::_hero_quest->getLevel() != 0)
        creature = HeroQuestLevelWindow::_hero_quest->getLevel()->getCurrentlyActingCreature();
    _has_acted_at_all = false;
    _has_moved = false;
    _num_attacks_left = 1;
    DV(
            ("_num_attacks_left = 1 (reset), %s", qPrintable(creature == 0 ? "null" : creature->getName())));
    _has_cast_spell = false;
    _num_spell_casts_left = 1;
    _move_points = 0;

    _sleeping = false;
}

void Level::ActionStates::set(const SetType& set_type)
{
    switch (set_type)
    {
        case ALL_DONE:
            _has_acted_at_all = true;
            _has_moved = true;
            _num_attacks_left = 0;
            DV(("_num_attacks_left = 0 (set ALL_DONE)"))
            ;
            _has_cast_spell = true;
            _num_spell_casts_left = 0;
            _move_points = 0;
            break;

        default:
            break;
    }
}

bool Level::ActionStates::save(SaveContext& save_context) const
{
    SaveContext::OpenChapter open_chapter(save_context, "ActionStates");

    save_context.writeBool(_has_acted_at_all, "_has_acted_at_all");
    save_context.writeBool(_has_moved, "_has_moved");
    save_context.writeUInt(_num_attacks_left, "_num_attacks_left");
    save_context.writeBool(_has_cast_spell, "_has_cast_spell");
    save_context.writeUInt(_num_spell_casts_left, "_num_spell_casts_left");
    save_context.writeUInt(_move_points, "_move_points");
    save_context.writeBool(_sleeping, "_sleeping");

    return !save_context.fail();
}

bool Level::ActionStates::load(LoadContext& load_context)
{
    LoadContext::OpenChapter open_chapter(load_context, "Level::ActionStates");

    load_context.readBool(&_has_acted_at_all, "_has_acted_at_all");
    load_context.readBool(&_has_moved, "_has_moved");
    load_context.readUInt(&_num_attacks_left, "_num_attacks_left");
    load_context.readBool(&_has_cast_spell, "_has_cast_spell");
    load_context.readUInt(&_num_spell_casts_left, "_num_spell_casts_left");
    load_context.readUInt(&_move_points, "_move_points");
    load_context.readBool(&_sleeping, "_sleeping");

    return !load_context.fail();
}

// ==================================================================

Level::HeroActionStates::HeroActionStates()
:
ActionStates(),
_has_searched(false),
 _has_thrown_movement_dice(false), _may_cross_monsters_during_movement(
                false), _dice_results()
{
    // NIX
};

void Level::HeroActionStates::initOnStartTurn(const Hero& hero)
{
    reset(true /*start_turn*/);

    _num_spell_casts_left = hero.canCastSpells() ? 1 : 0;
}

void Level::HeroActionStates::reset(bool start_turn)
{
    ActionStates::reset();
    _has_searched = false;
    _has_thrown_movement_dice = false;
    if (!start_turn)
    {
        _may_cross_monsters_during_movement = false;
    }
    _dice_results.clear();
}

void Level::HeroActionStates::set(const SetType& set_type)
{
    switch (set_type)
    {
        case ALL_DONE:
            Parent::set(set_type);
            _has_searched = true;
            _has_thrown_movement_dice = true;
            _may_cross_monsters_during_movement = false;
            break;

        default:
            break;
    }
}

bool Level::HeroActionStates::save(SaveContext& save_context) const
{
    SaveContext::OpenChapter open_chapter(save_context, "HeroActionStates");

    Parent::save(save_context);

    save_context.writeBool(_has_searched, "_has_searched");
    save_context.writeBool(_has_thrown_movement_dice, "_has_thrown_movement_dice");
    save_context.writeBool(_may_cross_monsters_during_movement, "_may_cross_monsters_during_movement");

    save_context.writeUInt(_dice_results.size(), "_dice_results.size()");
    for (uint i = 0; i < _dice_results.size(); ++i)
    {
        QString i_str;
        i_str.setNum(i);
        save_context.writeUInt(_dice_results[i], QString("_dice_results[") + i_str + "]");
    }

    return !save_context.fail();
}

bool Level::HeroActionStates::load(LoadContext& load_context)
{
    LoadContext::OpenChapter open_chapter(load_context, "Level::HeroActionStates");

    Parent::load(load_context);

    load_context.readBool(&_has_searched, "_has_searched");
    load_context.readBool(&_has_thrown_movement_dice, "_has_thrown_movement_dice");
    load_context.readBool(&_may_cross_monsters_during_movement, "_may_cross_monsters_during_movement");

    _dice_results.clear();
    uint num_dice_results;
    load_context.readUInt(&num_dice_results, "_dice_results.size()");
    _dice_results.reserve(num_dice_results);
    for (uint i = 0; i < num_dice_results; ++i)
    {
        uint dice_result;
        load_context.readUInt(&dice_result, "dice_result");
        _dice_results.push_back(dice_result);
    }

    return !load_context.fail();
}

// ==================================================================

Level::MonsterActionStates::MonsterActionStates()
:
ActionStates()
{
    // NIX
}

void Level::MonsterActionStates::init(const Monster& monster)
{
    reset();

    _move_points = monster.getNumMovePoints();
}

void Level::MonsterActionStates::reset()
{
    ActionStates::reset();
}

bool Level::MonsterActionStates::save(SaveContext& save_context) const
{
    SaveContext::OpenChapter open_chapter(save_context, "MonsterActionStates");

    Parent::save(save_context);

    return !save_context.fail();
}

bool Level::MonsterActionStates::load(LoadContext& load_context)
{
    LoadContext::OpenChapter open_chapter(load_context, "Level::MonsterActionStates");

    Parent::load(load_context);

    return !load_context.fail();
}

// ==================================================================

bool Level::LevelState::save(SaveContext& save_context) const
{
    SaveContext::OpenChapter open_chapter(save_context, "LevelState");

    save_context.writeUInt(_round_no, "_round_no");
    save_context.writeUInt(_current_hero_idx, "_current_hero_idx");
    save_context.writeUInt(_current_monster_idx, "_current_monster_idx");

    save_context.writeUInt(_hero_action_states.size(), "_hero_action_states.size()");
    for (vector<HeroActionStates>::const_iterator it = _hero_action_states.begin(); it != _hero_action_states.end(); ++it)
        it->save(save_context);

    save_context.writeUInt(_monster_action_states.size(), "_monster_action_states.size()");
    for (vector<MonsterActionStates>::const_iterator it = _monster_action_states.begin(); it != _monster_action_states.end(); ++it)
        it->save(save_context);

    save_context.writeUInt(_num_turns_left.size(), "_num_turns_left.size()");
    for (vector<uint>::const_iterator it = _num_turns_left.begin(); it != _num_turns_left.end(); ++it)
        save_context.writeUInt(*it, "_num_turns_left[i]");

    save_context.writeBool(_heroes_may_exit_level_via_stairway, "_heroes_may_exit_level_via_stairway");

    return !save_context.fail();
}

bool Level::LevelState::load(LoadContext& load_context)
{
    LoadContext::OpenChapter open_chapter(load_context, "Level");

    load_context.readUInt(&_round_no, "_round_no");
    load_context.readUInt(&_current_hero_idx, "_current_hero_idx");
    load_context.readUInt(&_current_monster_idx, "_current_monster_idx");

    _hero_action_states.clear();
    uint num_hero_action_states;
    load_context.readUInt(&num_hero_action_states, "_hero_action_states.size()");
    for (uint i = 0; i < num_hero_action_states; ++i)
    {
        _hero_action_states.push_back(HeroActionStates());
        _hero_action_states.back().load(load_context);
    }

    _monster_action_states.clear();
    uint num_monster_action_states;
    load_context.readUInt(&num_monster_action_states, "_monster_action_states.size()");
    for (uint i = 0; i < num_monster_action_states; ++i)
    {
        _monster_action_states.push_back(MonsterActionStates());
        _monster_action_states.back().load(load_context);
    }

    _num_turns_left.clear();
    uint num_num_turns_left;
    load_context.readUInt(&num_num_turns_left, "_num_turns_left.size()");
    for (uint i = 0; i < num_num_turns_left; ++i)
    {
        uint num;
        load_context.readUInt(&num, "num for _num_turns_left");
        _num_turns_left.push_back(num);
    }

    load_context.readBool(&_heroes_may_exit_level_via_stairway, "_heroes_may_exit_level_via_stairway");

    return !load_context.fail();
}

// ==================================================================

Level::Level(uint num_heroes)
        :
        _acting_heroes(), // set later via setActingHeroes (after the heroes have been added)
  _level_state(),
  _delayed_heroes(),
  _monsters(),
  _doors(),
  _traps(),
  _traps_mutex(),
  _decoration(),
  _treasure_card_stock(),
  _treasure_card_deposition(),
  _chest_pos_to_treasure_description(),
  _non_chest_room_ids_treasures_searched(),
  _roaming_monster_templates()
{
    // create action states
    _level_state._hero_action_states.resize(num_heroes);

	// store dummy monster instances, just used for the roaming monster information method
	// "getRoamingMonsterType"
	_roaming_monster_templates.insert(make_pair("Goblin", new Goblin));
	_roaming_monster_templates.insert(make_pair("Orc", new Orc));
	_roaming_monster_templates.insert(make_pair("Fimir", new Fimir));
	_roaming_monster_templates.insert(make_pair("Skeleton", new Skeleton));
	_roaming_monster_templates.insert(make_pair("Zombie", new Zombie));
	_roaming_monster_templates.insert(make_pair("Mummy", new Mummy));
	_roaming_monster_templates.insert(make_pair("ChaosWarrior", new ChaosWarrior));
	_roaming_monster_templates.insert(make_pair("Gargoyle", new Gargoyle));
}

Level::~Level()
{
    deleteMonsters();
    deleteDoors();
    deleteTraps();
    deleteDecoration();

	while (!_roaming_monster_templates.empty())
	{
		map<QString, Monster*>::iterator it = _roaming_monster_templates.begin();
		delete it->second;
		_roaming_monster_templates.erase(it);

	}
}

QString Level::getName() const
{
    return "UNKNOWN";
}

Level::LevelState* Level::getLevelState()
{
    return &_level_state;
}

Level::HeroActionStates* Level::getCurrentlyActingHeroActionStates()
{
    if (_level_state._current_hero_idx >= _level_state._hero_action_states.size())
        return 0;

    return &(_level_state._hero_action_states[_level_state._current_hero_idx]);
}

const Level::HeroActionStates* Level::getCurrentlyActingHeroActionStates() const
{
    if (_level_state._current_hero_idx >= _level_state._hero_action_states.size())
        return 0;

    return &(_level_state._hero_action_states[_level_state._current_hero_idx]);
}

Level::HeroActionStates* Level::getHeroActionStates(Hero* hero)
{
    uint hero_index = getHeroIndex(hero);
    return &(_level_state._hero_action_states[hero_index]);
}

Level::MonsterActionStates* Level::getMonsterActionStates(Monster* monster)
{
    uint monster_index = getMonsterIndex(monster);
    return &(_level_state._monster_action_states[monster_index]);
}

Level::ActionStates* Level::getCreatureActionStates(Creature* creature)
{
    if (creature->isHero())
    {
        return getHeroActionStates(dynamic_cast<Hero*>(creature));
    }
    else
    {
        // monster
        return getMonsterActionStates(dynamic_cast<Monster*>(creature));
    }
}

Level::MonsterActionStates* Level::getCurrentlyActingMonsterActionStates()
{
    return &(_level_state._monster_action_states[_level_state._current_monster_idx]);
}

const std::vector<Hero*>& Level::getActingHeroes() const
{
    return _acting_heroes;
}

bool Level::heroesAreMoving() const
{
    /*cout << "Test" << endl;
    DVX(("_level_state._current_hero_idx = %d", _level_state._current_hero_idx));*/
    return _level_state._current_hero_idx < UINT_MAX;
}

bool Level::monstersAreMoving() const
{
    return _level_state._current_hero_idx == UINT_MAX;
}

const Level::LevelState& Level::getLevelState() const
{
    return _level_state;
}

void Level::startRound()
{
    DVX(("Level::startRound"));

    // round number
    ++(_level_state._round_no);

    // each hero may execute 1 turn in this round
    _level_state._num_turns_left = vector<uint>(_level_state._hero_action_states.size(), 1);

    // start with hero 0 in the vector
    _level_state._current_hero_idx = 0;

    _delayed_heroes.clear();

    startHeroTurn();

    autoSave();
}

/*!
 * If Preferences::ENABLE_AUTOSAVE is true, this method auto-saves the game at beginning of each round;
 * the save game file is saved in Preferences::AUTOSAVE_DIR.
 */
void Level::autoSave()
{
    if (!Preferences::ENABLE_AUTOSAVE)
        return;

    QDir savegame_basedir(Preferences::AUTOSAVE_DIR);
    if (!savegame_basedir.exists())
    {
        savegame_basedir.mkdir(Preferences::AUTOSAVE_DIR);
    }

    static uint num_round = 0;
    ++num_round;

    // before saving the first file, move all (non-dir) files in the savegame_basedir to a new subdir
    if (num_round == 1)
    {
        // are there any non-dir files?
        QDir dir(Preferences::AUTOSAVE_DIR);
        dir.setFilter(QDir::Files | QDir::NoSymLinks);
        QFileInfoList filesList = dir.entryInfoList();
        if (!filesList.empty())
        {
            // we have to move all files given by filesList to a tmp subdir
            // => get next free name of tmp subdir
            string tmp_base = "tmp_";
            int new_endnumber = 1;
            dir.setFilter(QDir::Dirs | QDir::NoSymLinks);
            QFileInfoList dirs_list = dir.entryInfoList();
            int i_dirlist;
            for (i_dirlist = 0; i_dirlist < dirs_list.size(); ++i_dirlist)
            {
                QString current_dirname = dirs_list.at(i_dirlist).path();
                //cout << "current_dirname: " << qPrintable(current_dirname) << endl;
                int index_of_endnumber = current_dirname.length();
                while (true)
                {
                    --index_of_endnumber;
                    if (index_of_endnumber < 0 || !current_dirname[index_of_endnumber].isDigit())
                    {
                        ++index_of_endnumber;
                        break;
                    }
                }
                if (index_of_endnumber >= current_dirname.length() || !current_dirname[index_of_endnumber].isDigit())
                {
                    // no digit at end
                    continue;
                }
                // found endnumber; extract it
                QString endnumber_str = current_dirname.right(current_dirname.length() - index_of_endnumber);
                //cout << "endnumber_str: " << qPrintable(endnumber_str) << endl;
                bool ok;
                int endnumber = endnumber_str.toInt(&ok);
                if (ok && endnumber > new_endnumber)
                {
                    new_endnumber = endnumber + 1;
                }
                //cout << "new_endnumber: " << new_endnumber << endl;
            }
            // have the new_endnumber => create new subdir and move existing files there
            QDir tmp_dir(Preferences::AUTOSAVE_DIR);
            QString new_subdir = QString(tmp_base.c_str()) + QString::number(new_endnumber);
            //cout << "creating new_subdir: " << qPrintable(new_subdir) << endl;
            tmp_dir.mkdir(new_subdir);
            // move (i.e. rename) files from filesList to the new subdir
            for (int i = 0; i < filesList.size(); ++i)
            {
                //cout << "dir.rename(" << qPrintable(filesList.at(i).fileName()) << ", " << qPrintable(new_subdir) << "/"
                //        << qPrintable(filesList.at(i).fileName()) << ")" << endl;
                dir.rename(filesList.at(i).fileName(), new_subdir + "/" + filesList.at(i).fileName());
            }
        }
    }

    stringstream str_stream;
    str_stream << qPrintable(Preferences::AUTOSAVE_DIR) << "/round_" << num_round << ".hqsg";
    string savegame_filename;
    str_stream >> savegame_filename;
    QString savegame_filename_qstring(savegame_filename.c_str());

    SaveContext save_context(savegame_filename_qstring,
            Preferences::ENABLE_SAVE_LOAD_LOG ? SaveContext::ENABLE_LOG : SaveContext::DISABLE_LOG);
    if (save_context.fail())
        return;

    HeroQuestLevelWindow::_hero_quest->save(save_context);
}

Creature* Level::getCurrentlyActingCreature()
{
    if (_level_state.isHeroActing() && !_acting_heroes.empty())
    {
        // hero is acting
        return _acting_heroes[_level_state._current_hero_idx];
    }

    if (_level_state.isMonsterActing() && !_monsters.empty())
    {
        // monster is acting
        return _monsters[_level_state._current_monster_idx];
    }

    return 0;
}

Hero* Level::getCurrentlyActingHero()
{
    Creature* current_creature = getCurrentlyActingCreature();
    if (!current_creature->isHero())
        return 0;

    return dynamic_cast<Hero*>(current_creature);
}

Monster* Level::getCurrentlyActingMonster()
{
    Creature* current_creature = getCurrentlyActingCreature();
    if (!current_creature->isMonster())
        return 0;

    return dynamic_cast<Monster*>(current_creature);
}

/*!
 * Initializes the level.
 */
void Level::create()
{
    /*
	_level_ended = false;

	// remove any creatures from previous quests from the playground
	_hero_quest.getPlayground().removeCreatures();

	// ---------------------------------------------------------------------------------------------
    */

    // delete any still existing monsters
    deleteMonsters();

	// prepare treasure cards stock
	prepareTreasureCards();

	// place heroes onto the playground
	placeHeroes();

	// create monsters, and place them on the board
	createMonsters();

	// create doors and secret doors on the board
	createDoors();

	// create traps on the board
	createTraps();

	// create decoration on the board
	createDecoration();
	// build (node to decoration) map; (this also updates node neighborhood info)
	for (vector<Decoration*>::iterator it = _decoration.begin(); it != _decoration.end(); ++it)
	{
		HeroQuestLevelWindow::_hero_quest->getPlayground()->addDecoration(*it);
	}

    createChestTreasureDescriptions();

	// compute the initial field visibility
	HeroQuestLevelWindow::_hero_quest->getPlayground()->updateFieldVisibility();
}

void Level::setActingHeroes(const vector<Hero*>& heroes)
{
    _acting_heroes = heroes;
}

/*!
 * @param referencing_id
 * @return the monster referenced by the referencing_id; null if this monster does not exist anymore
 */
Monster* Level::getMonster(uint referencing_id)
{
    Monster* monster = 0;
    for (vector<Monster*>::iterator it = _monsters.begin(); it != _monsters.end(); ++it)
    {
        if ((*it)->getReferencingID() == referencing_id)
            return *it;
    }

    DV(("Did not find monster with ref ID %d", referencing_id));
    return 0;
}

/*!
 * @param referencing_id
 * @return the monster referenced by the referencing_id (null if this monster does not exist anymore),
 *         paired with the index of this monster in _monsters (UINT_MAX if this monster does not exist anymore)
 */
pair<Monster*, uint> Level::getMonsterWithIndex(uint referencing_id)
{
    Monster* monster = 0;
    for (uint i = 0; i < _monsters.size(); ++i)
    {
        if (_monsters[i]->getReferencingID() == referencing_id)
            return make_pair(_monsters[i], i);
    }

    DV(("Did not find monster with ref ID %d", referencing_id));
    return make_pair<Monster*, uint>(0, UINT_MAX);
}

const vector<Monster*>& Level::getMonsters() const
{
    return _monsters;
}

Decoration* Level::getDecoration(uint referencing_id)
{
    Decoration* decoration = 0;
    for (vector<Decoration*>::iterator it = _decoration.begin(); it != _decoration.end(); ++it)
    {
        if ((*it)->getReferencingID() == referencing_id)
        {
            decoration = *it;
            break;
        }
    }
    if (decoration == 0)
    {
        DVX(("Did not find decoration with ref ID %d", referencing_id));
    }
    return decoration;
}

/*!
 * Builds the stock of treasure cards.
 */
bool Level::prepareTreasureCards()
{
    if (!_treasure_card_deposition.empty())
    {
        ProgError(("Treasure card deposition is not empty!"));
    }
    if (!_treasure_card_stock.empty())
    {
        ProgError(("Treasure card stock is not empty!"));
    }

    // move all treasure cards from TreasureCardStorage to _treasure_card_stock
    for (vector<TreasureCard>::const_iterator it = TreasureCardStorage::instance->getTreasureCards().begin();
            it != TreasureCardStorage::instance->getTreasureCards().end(); ++it)
	{
        _treasure_card_stock.push_back(*it);
	}
    TreasureCardStorage::instance->getTreasureCards().clear();

    // shuffle the cards in the stock
    shuffleTreasureCardStock();

	return true;
}

/*!
 * Shuffles the treasure card stock.
 */
void Level::shuffleTreasureCardStock()
{
    // move to tmp vector (shuffling requires random access)
    vector<TreasureCard> tmp_cards;
    tmp_cards.reserve(_treasure_card_stock.size());
    for (list<TreasureCard>::const_iterator it = _treasure_card_stock.begin(); it != _treasure_card_stock.end(); ++it)
    {
        tmp_cards.push_back(*it);
    }
    _treasure_card_stock.clear();

    // shuffle on random access structure
    random_shuffle(tmp_cards.begin(), tmp_cards.end());

    // move back to originating list
    for (vector<TreasureCard>::const_iterator it = tmp_cards.begin(); it != tmp_cards.end(); ++it)
    {
        _treasure_card_stock.push_back(*it);
    }
    tmp_cards.clear();
}

void Level::openChest(uint room_id)
{
    OpenChestCommand cmd(room_id);
    HeroQuestLevelWindow::_hero_quest->execute(cmd);
}

void Level::drawTreasureCard()
{
    DrawTreasureCardCommand cmd;
    HeroQuestLevelWindow::_hero_quest->execute(cmd);
}

uint Level::getHeroIndex(Hero* hero) const
{
    uint i = 0;
    for (vector<Hero*>::const_iterator it = _acting_heroes.begin(); it != _acting_heroes.end(); ++it)
    {
        if (*it == hero)
            return i;

        ++i;
    }

    return UINT_MAX;
}

uint Level::getMonsterIndex(Monster* monster) const
{
    uint i = 0;
    for (vector<Monster*>::const_iterator it = _monsters.begin(); it != _monsters.end(); ++it)
    {
        if (*it == monster)
            return i;

        ++i;
    }

    return UINT_MAX;
}

void Level::placeHeroes()
{
	// NIX
}

void Level::createMonsters()
{
	// NIX
}

void Level::createDoors()
{
	// NIX
}

void Level::createTraps()
{
	// NIX
}

void Level::createDecoration()
{
	// NIX
}

void Level::createChestTreasureDescriptions()
{
    // NIX
}

QString Level::getLevelBriefing() const
{
    return "";
}

void Level::deleteMonsters()
{
	for (vector<Monster*>::iterator it = _monsters.begin(); it != _monsters.end(); ++it)
	{
		if ((*it) == 0)
			cout << "Level::deleteMonsters: Warning: monster already deleted!" << endl;
		else
		{
		    HeroQuestLevelWindow::_hero_quest->getPlayground()->removeCreatureFromMaps(*it);
		    delete (*it);
		}
	}
	_monsters.clear();
}

void Level::deleteDoors()
{
	for (unsigned int i = 0; i < _doors.size(); ++i)
	{
		delete _doors[i];
	}
	_doors.clear();
}

void Level::deleteTraps()
{
	for (unsigned int i = 0; i < _traps.size(); ++i)
	{
		delete _traps[i];
	}
	_traps.clear();
}

void Level::deleteDecoration()
{
	for (unsigned int i = 0; i < _decoration.size(); ++i)
	{
		delete _decoration[i];
	}
	_decoration.clear();
}

/*!
 * @param monster
 * @return true iff monster was successfully removed
 */
bool Level::removeMonster(Monster* monster)
{
    HG_ASSERT((monster != 0), "removeMonster: monster is 0");
    DV(("Level::removeMonster %s BEGIN", qPrintable(Monster::getName(monster))));
    if (_monsters.size() != _level_state._monster_action_states.size())
    {
        DVX(("_monsters.size() = %d, but _level_state._monster_action_states.size() = %d", _monsters.size(), _level_state._monster_action_states.size()));
        return false;
    }

    // _monsters and action states iterators
    bool found = false;
    vector<Monster*>::iterator it_monsters = _monsters.begin();
    vector<MonsterActionStates>::iterator it_action_states = _level_state._monster_action_states.begin();
    for (uint i = 0; i < _monsters.size(); ++i)
    {
        if (_monsters[i] == monster)
        {
            found = true;
            break;
        }

        ++it_monsters;
        ++it_action_states;
    }

    if (!found)
    {
        DV(("Level::removeMonster: monster %s not found", qPrintable(Monster::getName(monster))));
        return false;
    }

	// remove from playground
    HeroQuestLevelWindow::_hero_quest->getPlayground()->removeCreatureFromMaps(monster);

	// remove from dice roll panel
    HeroQuestLevelWindow::_hero_quest->getDiceRollPane()->clearDefendDice();

    // delete from monsters list and action states
    _level_state._monster_action_states.erase(it_action_states);

    delete *it_monsters;
    _monsters.erase(it_monsters);

    DV(("Level::removeMonster %s END", qPrintable(Monster::getName(monster))));

    return true;
}

/*!
 * Removes hero from the level. If exit_successfully is false, we check if the hero really dies,
 * and possible display a game over dialog if no hero is left anymore.
 * If hero is the currently acting hero, the _level_state._current_hero_idx is invalidated to UINT_MAX.
 *
 * @param hero
 * @param exit_successfully
 * @return true iff hero was successfully removed from the level
 */
bool Level::removeHero(Hero* hero, bool exit_successfully)
{
    if (_acting_heroes.size() != _level_state._hero_action_states.size() ||
            _acting_heroes.size() != _level_state._num_turns_left.size())
    {
        DVX(("_acting_heroes.size() = %d, _level_state._hero_action_states.size() = %d, _level_state._num_turns_left.size() = %d",
                _acting_heroes.size(),
                _level_state._hero_action_states.size(),
                _level_state._num_turns_left.size()));
        return false;
    }

    // There are circumstances in which the hero will not yet die.
    if (!exit_successfully && !hero->checkDeath())
        return false;

    // potentially invalidate _current_hero_idx
    if (hero == getCurrentlyActingHero())
    {
        _level_state._current_hero_idx = UINT_MAX;
    }

    // remove from playground
    HeroQuestLevelWindow::_hero_quest->getPlayground()->removeCreatureFromMaps(hero);

    // remove from dice roll panel
    HeroQuestLevelWindow::_hero_quest->getDiceRollPane()->clearDefendDice();

    // action states, active_heroes, num_turns_left
    vector<Hero*>::iterator it_acting_heroes = _acting_heroes.begin();
    vector<HeroActionStates>::iterator it_hero_action_states = _level_state._hero_action_states.begin();
    vector<uint>::iterator it_num_turns_left = _level_state._num_turns_left.begin();
    for (uint i = 0; i != _acting_heroes.size(); ++i)
    {
        if (hero == (*it_acting_heroes))
        {
            _acting_heroes.erase(it_acting_heroes);
            _delayed_heroes.erase(hero);
            _level_state._hero_action_states.erase(it_hero_action_states);
            _level_state._num_turns_left.erase(it_num_turns_left);

            break;
        }

        ++it_acting_heroes;
        ++it_hero_action_states;
        ++it_num_turns_left;
    }

    HeroQuestLevelWindow::_hero_quest->removeHeroStatisticPane(hero);

    // if no hero is left, we display "game over" and go back to the startup dialog (i.e. kill the HeroQuestLevelWindow)
    if (!exit_successfully && _acting_heroes.empty())
    {
        DialogGameOver dialog_game_over;
        dialog_game_over.exec();

        HeroQuestLevelWindow::_hero_quest->exitLost();
    }

    return true;
}

bool Level::removeCreature(Creature* creature)
{
    bool creature_removed = false;

	if (creature->isHero())
        creature_removed = removeHero(dynamic_cast<Hero*>(creature), false);
	else if (creature->isMonster())
	    creature_removed = removeMonster(dynamic_cast<Monster*>(creature));

	return creature_removed;
}

bool Level::isCreatureAlive(Creature* creature)
{
    for (vector<Hero*>::const_iterator it = _acting_heroes.begin(); it != _acting_heroes.end(); ++it)
    {
        if (((Creature*) *it) == creature)
            return true;
    }

    for (vector<Monster*>::const_iterator it = _monsters.begin(); it != _monsters.end(); ++it)
    {
        if (((Creature*) *it) == creature)
            return true;
    }

    return false;
}

const vector<Door*>& Level::getDoors() const
{
	return _doors;
}

vector<Door*>& Level::getDoors()
{
	return _doors;
}

const vector<Trap*>& Level::getTraps() const
{
	return _traps;
}

vector<Trap*>& Level::getTraps()
{
	return _traps;
}

const vector<Decoration*>& Level::getDecoration() const
{
	return _decoration;
}

vector<Decoration*>& Level::getDecoration()
{
	return _decoration;
}

/*!
 * Clears incident_doors, then adds all doors which are incident to node_id to incident_doors.
 *
 * \return True, if at least one door has been added to incident_doors; false otherwise.
 */
bool Level::getIncidentDoors(const NodeID& node_id, vector<Door*>* incident_doors)
{
	incident_doors->clear();
	bool door_added = false;
	// search _doors for a door being incdent to node_id
	for (unsigned int i = 0; i < _doors.size(); ++i)
	{
		Door* door = _doors[i];
		if (door->isIncidentTo(node_id))
		{
			incident_doors->push_back(door);
			door_added = true;
		}
	}

	return door_added;
}

/*!
 * \return True, if there is at least one door (not the secret doors) incident to node_id.
 */
bool Level::hasIncidentClosedDoors(const NodeID& node_id)
{
    vector<Door*> incident_doors_dummy;
    return getIncidentClosedDoors(node_id, &incident_doors_dummy);
}

/*!
 * Clears incident_closed_doors, then adds all closed doors (not the secret doors) which are incident
 * to node_id to incident_closed_doors.
 *
 * \return True, if at least one door has been added to incident_closed_doors; false otherwise.
 */
bool Level::getIncidentClosedDoors(const NodeID& node_id, vector<Door*>* incident_closed_doors)
{
	incident_closed_doors->clear();

	vector<Door*> incident_doors;
	if (!getIncidentDoors(node_id, &incident_doors))
		return false;

	// are closed doors in incident_doors?
	bool door_added = false;
	for (unsigned int i = 0; i < incident_doors.size(); ++i)
	{
		Door* door = incident_doors[i];

		// no secret doors
		if (typeid(*door) == typeid(SecretDoor))
			continue;

		if (!door->getOpen())
		{
			incident_closed_doors->push_back(door);
			door_added = true;
		}
	}

	return door_added;
}

/*!
 * \return The door which connects the transition's nodes; 0 if there is no
 *         door in the _doors vector that fits.
 */
const Door* Level::getDoor(const pair<NodeID, NodeID>& transition) const
{
	const Door* door = 0;
	for (unsigned int i = 0; i < _doors.size(); ++i)
	{
		const Door* current_door = _doors[i];
		if (current_door->connects(transition.first, transition.second))
		{
			door = current_door;
		}
	}
	return door;
}

void Level::openDoor(const Door* destination_door)
{
	// find the door in the _doors vector
	unsigned int door_num = UINT_MAX;
	for (unsigned int i = 0; i < _doors.size(); ++i)
	{
		if (_doors[i] == destination_door)
		{
			door_num = i;
			break;
		}
	}
	if (door_num == UINT_MAX)
		return;

	// adjust the neighbors in the BoardGraph
	BoardGraph* board_graph = HeroQuestLevelWindow::_hero_quest->getPlayground()->getQuestBoard()->getBoardGraph();
	board_graph->getNode(destination_door->getF1()).addNeighborAbsolute(destination_door->getF2());
	board_graph->getNode(destination_door->getF2()).addNeighborAbsolute(destination_door->getF1());

	// set open state
	_doors[door_num]->setOpen(true);

	// play sound
	if (typeid(*destination_door) == typeid(Door))
	    HeroQuestLevelWindow::_hero_quest->playSoundOnce(SoundManager::SOUND_DOOR_OPENS);
	if (typeid(*destination_door) == typeid(SecretDoor))
	    HeroQuestLevelWindow::_hero_quest->playSoundOnce(SoundManager::SOUND_SECRET_DOOR_OPENS);

	// update field visibility
	HeroQuestLevelWindow::_hero_quest->getPlayground()->updateFieldVisibility();
	// secret doors: set the field which contains the secret door pad
	if (typeid(*destination_door) == typeid(SecretDoor))
	    HeroQuestLevelWindow::_hero_quest->getPlayground()->setNodeVisibility(dynamic_cast<const SecretDoor*>(destination_door)->getDrawPosNodeID(), true);

    // update the reachable area
    HeroQuestLevelWindow::_hero_quest->getPlayground()->getQuestBoard()->computeReachableArea();
	
	// redraw
	HeroQuestLevelWindow::_hero_quest->getPlayground()->update();

    // update button visibility
    HeroQuestLevelWindow::_hero_quest->updateButtons();

    // execute follow-up actions specific to the level
    doorHasBeenOpenedBy(getCurrentlyActingCreature(), destination_door);
}

void Level::startHeroTurn()
{
    DVX(("Level::startHeroTurn"));

    // prepare hero
    Hero* current_hero = dynamic_cast<Hero*>(getCurrentlyActingCreature());
    if (current_hero == 0)
        return;

    DVX(("current hero: %s", qPrintable(current_hero->getName())));

    // is this hero in the delayed state (has he already thrown the movement dice earlier)?
    set<Hero*>::const_iterator it_delayed_heroes = _delayed_heroes.find(current_hero);

    if (it_delayed_heroes == _delayed_heroes.end())
    {
        // normal start of hero turn

        // may this hero act in this round?
        if (current_hero->getSkipRounds() > 0)
        {
            current_hero->decreaseSkipRounds();
            endHeroTurn();
            return;
        }

        HeroActionStates& hero_action_states = getCurrentHeroActionStates();

        // sleeping hero must throw a 6 before waking up
        if (hero_action_states._sleeping)
        {
            if (!HeroQuestLevelWindow::_hero_quest->creatureTriesToWakeUp(*current_hero))
            {
                // still sleeping
                endHeroTurn();
                return;
            }

            // hero woke up
            HeroQuestLevelWindow::_hero_quest->updateHeroStatisticPaneOfCurrentHero();
        }

        // update states
        hero_action_states.initOnStartTurn(*(_acting_heroes[_level_state._current_hero_idx]));
    }
    else
    {
        // hero's turn was delayed; don't init hero states
        // (hero has already thrown the movement dice before in this round)
    }

    // action pane
    DV(("Level::startHeroTurn: initActionPane"));
    HeroQuestLevelWindow::_hero_quest->initActionPane();

    // update field visibility
    DV(("Level::startHeroTurn: updateFieldVisibility"));
    HeroQuestLevelWindow::_hero_quest->getPlayground()->updateFieldVisibility(*current_hero);

    // statistics
    DV(("Level::startHeroTurn: updateHeroStatisticPanes"));
    HeroQuestLevelWindow::_hero_quest->updateHeroStatisticPanes();

    // buttons
    DV(("Level::startHeroTurn: updateButtons"));
    HeroQuestLevelWindow::_hero_quest->updateButtons();

    // re-render the playground, so that the highlight of the currently acting hero is updated
    HeroQuestLevelWindow::_hero_quest->getPlayground()->update();

    if (it_delayed_heroes == _delayed_heroes.end())
    {
        // implicitly "click the move button", i.e. throw movement dice and go to ACTION_STATE_MOVE_OR_ATTACK
        HeroQuestLevelWindow::_hero_quest->initCurrentHeroActionStateToMoveOrAttack();
    }
    else
    {
        // restore the movement dice roll area
        HeroQuestLevelWindow::_hero_quest->getDiceRollPane()->updateMovementDice(*current_hero,
                getCurrentHeroActionStates()._dice_results);

        // restore the playground
        HeroQuestLevelWindow::_hero_quest->getPlayground()->getQuestBoard()->computeReachableArea();
        HeroQuestLevelWindow::_hero_quest->getPlayground()->update();

        // clear this hero from the delayed heroes
        _delayed_heroes.erase(it_delayed_heroes);
    }
}

void Level::endHeroTurn()
{
    Creature* current_creature = getCurrentlyActingCreature();
    if (!current_creature->isHero())
        return;
    Hero* current_hero = dynamic_cast<Hero*>(current_creature);

    // state
    getCurrentlyActingHeroActionStates()->_has_acted_at_all = true;
    if (_level_state._num_turns_left[_level_state._current_hero_idx] > 0)
        --(_level_state._num_turns_left[_level_state._current_hero_idx]);

    // reset the through-wall movability
    current_hero->setCanMoveThroughWalls(false);

	// reset the attack/defend areas
    HeroQuestLevelWindow::_hero_quest->resetActionPane();

    HeroQuestLevelWindow::_hero_quest->updateButtons();
    HeroQuestLevelWindow::_hero_quest->updateHeroStatisticPanes();

    // reset the reachable area (movement)
    HeroQuestLevelWindow::_hero_quest->getPlayground()->getQuestBoard()->clearReachableArea();
    HeroQuestLevelWindow::_hero_quest->getPlayground()->update();

    bool remove_current_hero_from_level = false;
    if (_level_state._heroes_may_exit_level_via_stairway && currentHeroIsOnStairway())
    {
        DVX(
                ("removing current hero from level, because the level task has been done and the hero stands on the stairway"));
        remove_current_hero_from_level = true;
    }

    // switch to next hero (potentially removing the current hero)
    switch (switchToNextCreature(remove_current_hero_from_level))
    {
        case SWITCHED_TO_HERO:
        {
            startHeroTurn();
        }
            break;

        case SWITCHED_TO_MONSTER:
        {
            // if no hero remaining in this round, switch to monsters
            ExecuteMonsterTurnsCommand cmd;
            HeroQuestLevelWindow::_hero_quest->execute(cmd);
        }
            break;

        case LEVEL_END:
            HeroQuestLevelWindow::_hero_quest->exitLevelFinished();
            break;
    }
}

/*!
 * Copies cards back to storages.
 */
void Level::cleanupGameMaterialOnLevelFinish()
{
    // --- treasure cards ---
    for (list<TreasureCard>::iterator it = _treasure_card_stock.begin(); it != _treasure_card_stock.end(); ++it)
    {
        TreasureCardStorage::instance->getTreasureCards().push_back(*it);
    }
    _treasure_card_stock.clear();

    for (list<TreasureCard>::iterator it = _treasure_card_deposition.begin(); it != _treasure_card_deposition.end();
            ++it)
    {
        TreasureCardStorage::instance->getTreasureCards().push_back(*it);
    }
    _treasure_card_deposition.clear();

    // --- spell cards ---
    HeroCamp::instance->redistributeSpellCardsToHeroes();
}

/*!
 * Called when the level was finished successfully. Here, Heroes will obtain their level reward
 * (see reimplementation in each level).
 */
void Level::obtainLevelReward()
{
}

/*!
 * Is called when creature has been moved onto node.
 *
 * @param creature
 * @param node
 */
void Level::fieldHasBeenReachedBy(const Creature* creature, const NodeID& node)
{
}

/*!
 * Is called when creature has opened door.
 *
 * @param creature
 * @param door
 */
void Level::doorHasBeenOpenedBy(const Creature* creature, const Door* door)
{
}

void Level::putSpellCardsBackToStorage(Hero* hero)
{
    const set<SpellCard>& spell_cards = hero->getInventory().getSpellCards();
    for (set<SpellCard>::const_iterator it_const = spell_cards.begin(); it_const != spell_cards.end(); ++it_const)
    {
        SpellCardStorage::instance->getSpellCardStock().push_back(*it_const);
    }
    hero->getInventory().removeSpellCards();
}

/*!
 * A hero is weak, if (life_points - num_attack_dice + num_defend_dice) is minimal.
 * Inserts all heroes who have the minimum weakness (among all heroes) into weakest_heroes.
 *
 * \return The minimum weakness.
 */
int Level::computeWeakestHeroes(vector<Hero*>* weakest_heroes)
{
    return computeWeakestHeroes(HeroQuestLevelWindow::_hero_quest->getLevel()->getActingHeroes(), weakest_heroes);
}

/*!
 * A hero is weak, if (life_points - num_attack_dice + num_defend_dice) is minimal.
 * Inserts all heroes who have the minimum weakness (among considered_heroes) into weakest_heroes.
 *
 * \return The minimum weakness.
 */
int Level::computeWeakestHeroes(const vector<Hero*>& considered_heroes, vector<Hero*>* weakest_heroes)
{
	weakest_heroes->clear();

	// compute all weaknesses, and the minimum weakness
	map<Hero*, int> weaknesses;
	int min_weakness = INT_MAX;
	for (unsigned int i = 0; i < considered_heroes.size(); ++i)
	{
		Hero* hero = considered_heroes[i];
        int weakness = hero->getLifePoints() - hero->getNumDiceAttack() - hero->getNumDiceAttackExtra()
                + hero->getNumDiceDefend();
		weaknesses[hero] = weakness;

		if (weakness < min_weakness)
			min_weakness = weakness;
	}

	// collect all heroes who have minimum weakness
	for (map<Hero*, int>::iterator it = weaknesses.begin(); it != weaknesses.end(); ++it)
	{
		Hero* hero = it->first;
		int weakness = it->second;

		if (weakness == min_weakness)
			weakest_heroes->push_back(hero);
	}

	return min_weakness;
}

void Level::throwMovementDice()
{
	Hero* current_hero = dynamic_cast<Hero*>(getCurrentlyActingCreature());
	if (current_hero == 0)
	    return;

    HeroQuestLevelWindow::_hero_quest->getDiceRollPane()->throwMovementDice(*current_hero,
            &(getCurrentHeroActionStates()._dice_results));
	
	unsigned int movement_dice_sum = 0;
    for (unsigned int i = 0; i < getCurrentHeroActionStates()._dice_results.size(); ++i)
	{
        movement_dice_sum += getCurrentHeroActionStates()._dice_results[i];
	}

	getCurrentHeroActionStates()._move_points = movement_dice_sum;
	getCurrentHeroActionStates()._has_thrown_movement_dice = true;
    HeroQuestLevelWindow::_hero_quest->updateHeroStatisticPanes();

}

Level::HeroActionStates& Level::getCurrentHeroActionStates()
{
    return _level_state._hero_action_states[_level_state._current_hero_idx];
}

Level::ActionStates* Level::getCurrentlyActingCreatureActionStates()
{
    if (_level_state.isHeroActing())
    {
        // hero acting
        return &(_level_state._hero_action_states[_level_state._current_hero_idx]);
    }
    else if (_level_state.isMonsterActing())
    {
        // monster acting
        return &(_level_state._monster_action_states[_level_state._current_monster_idx]);
    }
    else
    {
        return 0;
    }
}

/*!
 * Throws movement dice, computes the reachable area, and sets the current hero's action states
 * to MOVE_OR_ATTACK.
 *
 * Must be called by the GUI thread! Use HeroQuestLevelWindow::initCurrentHeroActionStateToMoveOrAttack
 * to call this from another thread. This is executed on startTurn of each hero.
 */
void Level::initCurrentHeroActionStateToMoveOrAttack()
{
    if (!HeroQuestLevelWindow::currentThreadIsGUIThread())
    {
        cout << "ERROR: Level::initCurrentHeroActionStateToMoveOrAttack was called by a thread != GUI thread!" << endl;
        return;
    }

#if 0
	// DEBUG
	_hero_quest.displayDialog();
#endif

	// is it currently the heroes' turn?
	if (!_level_state.isHeroActing())
		return;

	Hero* current_hero = dynamic_cast<Hero*>(getCurrentlyActingCreature());
	if (current_hero == 0)
	    return;

	// need to throw the movement dice?
	if (!getCurrentlyActingHeroActionStates()->_has_thrown_movement_dice)
	{
		throwMovementDice();
	}

    HeroQuestLevelWindow::_hero_quest->getPlayground()->setActionMode(
            Playground::ACTION_MODE_MOVE_OR_ATTACK_OR_OPEN_DOOR);

	HeroQuestLevelWindow::_hero_quest->updateButtons();

    HeroQuestLevelWindow::_hero_quest->getPlayground()->getQuestBoard()->computeReachableArea();
    HeroQuestLevelWindow::_hero_quest->getPlayground()->update();
}

/*!
 * Shows all secret doors and pit traps which are in line of sight from the current hero's position.
 */
void Level::searchTrapsButtonClicked()
{
    Creature* current_creature = getCurrentlyActingCreature();
    if (!current_creature->isHero())
        return;
    Hero* current_hero = dynamic_cast<Hero*>(current_creature);

    getCurrentlyActingHeroActionStates()->_has_acted_at_all = true;

    const NodeID* pos_current_hero = HeroQuestLevelWindow::_hero_quest->getPlayground()->getCreaturePos(current_hero);
	if (pos_current_hero == 0)
		DVX(("Internal program error: Level::searchTrapsButtonClicked()"));

	// compute the viewable nodes
	set<NodeID> viewable_nodes;
    HeroQuestLevelWindow::_hero_quest->getPlayground()->computeViewableNodes(*pos_current_hero, false, true,
            &viewable_nodes); // false: do not respect target node border (used for finding secret doors and traps); true: view through heroes

	// open all SecretDoors which are viewable from the current hero's position
	vector<SecretDoor*> viewable_secret_doors;
	HeroQuestLevelWindow::_hero_quest->getPlayground()->computeViewableSecretDoors(viewable_nodes, &viewable_secret_doors);
	for (unsigned int i = 0; i < viewable_secret_doors.size(); ++i)
	{
		SecretDoor* secret_door = viewable_secret_doors[i];
		// open only closed/invisible secret doors
		if (!secret_door->getOpen())
		{
			openDoor(secret_door);
		}
	}

	// show all pit traps which are viewable from the current hero's position
	vector<Trap*> viewable_traps;
	HeroQuestLevelWindow::_hero_quest->getPlayground()->computeViewableTraps(viewable_nodes, &viewable_traps);
	for (unsigned int i = 0; i < viewable_traps.size(); ++i)
	{
		// set trap visible
		Trap* trap = viewable_traps[i];
		trap->setVisible(true);
	}

	getCurrentlyActingHeroActionStates()->_has_searched = true;

    HeroQuestLevelWindow::_hero_quest->getPlayground()->getQuestBoard()->computeReachableArea();
	HeroQuestLevelWindow::_hero_quest->getPlayground()->update();

	HeroQuestLevelWindow::_hero_quest->updateButtons();
}

/*!
 * Switches from reachable_area view to searchable_area view.
 */
void Level::searchTrapsButtonPressed()
{
    HeroQuestLevelWindow::_hero_quest->getPlayground()->getQuestBoard()->computeSearchableArea();
    HeroQuestLevelWindow::_hero_quest->getPlayground()->update();
}

/*!
 * Switches from searchable_area view to reachable_area view.
 */
void Level::searchTrapsButtonReleased()
{
    HeroQuestLevelWindow::_hero_quest->getPlayground()->getQuestBoard()->clearSearchableArea();
    HeroQuestLevelWindow::_hero_quest->getPlayground()->update();
}

void Level::searchTreasuresButtonClicked()
{
    // preparation
    Hero* current_hero = dynamic_cast<Hero*>(getCurrentlyActingCreature());

    const NodeID* hero_node_id = 0;
    if (HeroQuestLevelWindow::_hero_quest->getPlayground() != 0)
        hero_node_id = HeroQuestLevelWindow::_hero_quest->getPlayground()->getCurrentlyActingCreaturePos();

    uint current_hero_room_id = 0;
    if (hero_node_id != 0)
        current_hero_room_id = HeroQuestLevelWindow::_hero_quest->getPlayground()->getQuestBoard()->getRoomID(*hero_node_id);

    if (HeroQuestLevelWindow::_hero_quest->getLevel()->roomContainsClosedChest(current_hero_room_id))
    {
        // action: open chest
        openChest(current_hero_room_id);
    }
    else
    {
        // action: draw treasure card
        drawTreasureCard();
        setNonChestRoomHasBeenSearchedForTreasures(current_hero_room_id);
    }

	getCurrentlyActingHeroActionStates()->_has_acted_at_all = true;
	getCurrentlyActingHeroActionStates()->_has_searched = true;
    // after searching, attacking is not possible anymore
    getCurrentlyActingHeroActionStates()->_num_attacks_left = 0;

    HeroQuestLevelWindow::_hero_quest->getPlayground()->getQuestBoard()->computeReachableArea();
    HeroQuestLevelWindow::_hero_quest->getPlayground()->update();

	HeroQuestLevelWindow::_hero_quest->updateButtons();
}
#if 0
void Level::castSpellButtonClicked()
{
	cout << "Level::castSpellButtonClicked()" << endl;

	_round._hero_action_states._has_acted_at_all = true;
	--(_round._hero_action_states._num_spell_casts_left);
	_round._hero_action_states._num_attacks_left = 0;
	_hero_quest.getTurnActivityPanel().updateButtonVisibility();
}
 #endif
/*!
 * Removes the current hero from the acting heroes list, and appends it at the back
 * of that list. Then starts the turn of the next hero.
 */
void Level::delayTurnButtonClicked()
{
    // Store a pointer to the delayed hero in _delayed_heroes, so that we can come back
    // to this hero later. Note that the hero's action states stay untouched.
    Hero* current_hero = getCurrentlyActingHero();
    if (current_hero == 0)
        return;

    _delayed_heroes.insert(current_hero);

    switchToNextCreature(false);

	startHeroTurn();
}

void Level::endTurnButtonClicked()
{
    endHeroTurn();
}

void Level::handlePitTrap(
	const NodeID& orig_hero_pos,
	const NodeID& dest_node,
	bool dest_node_is_occupied,
	PitTrap* pit_trap,
	bool* hero_may_continue_moving)
{
	Hero* current_hero = getCurrentlyActingHero();
	if (current_hero == 0)
	    return;

	bool trigger_trap = true;
	bool try_to_jump_over_trap = false;
	NodeID field_behind_trap = pit_trap->getNodeID() + (pit_trap->getNodeID() - orig_hero_pos);

	// visible?
	if (pit_trap->getVisible())
	{
		// pit trap is visible => hero might want to try to jump over the trap

		// are the trap and the field behind the trap free?
		if (!dest_node_is_occupied &&
			HeroQuestLevelWindow::_hero_quest->getPlayground()->contains(field_behind_trap) &&
			!HeroQuestLevelWindow::_hero_quest->getPlayground()->isFieldOccupiedByCreature(field_behind_trap))
		{
			vector<NodeID> movement_path;
			// problem: in einem raum sind alle nodes viewable!
            HeroQuestLevelWindow::_hero_quest->getPlayground()->getQuestBoard()->getBoardGraph()->getShortestPath(
                    orig_hero_pos, field_behind_trap, false, false, current_hero->getCanMoveThroughWalls(), 2,
                    &movement_path);
			if (movement_path.size() == 2)
			{
				// field_behind_trap is accessible and free

				// does the hero have enough movement points to jump over the trap?
				if (getCurrentlyActingHeroActionStates()->_move_points > 0)
				{
					// yes, everything OK => ask if the hero shall try to jump over the trap
                    try_to_jump_over_trap = HeroQuestLevelWindow::_hero_quest->execDialogOkCancel("Pit trap",
                            "Do you want to try to jump over the pit trap?") == QDialog::Accepted;
				}
			}
		}
	}

	if (try_to_jump_over_trap)
	{
		// throw attack dice... skull = fall down, otherwise = jump over
		vector<DiceRollPane::AttackDiceResult> attack_dice_results;
		HeroQuestLevelWindow::_hero_quest->throwAttackDice(*current_hero, 1, &attack_dice_results);
		if (attack_dice_results.empty())
		{
		    DVX(("Problem in Level::handlePitTrap()"));
		    return;
		}
		if (attack_dice_results[0] != DiceRollPane::ATTACK_DICE_RESULT_SKULL)
			trigger_trap = false;
	}
			
	if (trigger_trap)
	{
		// pit trap is triggered => trap causes damage to hero, and hero may not perform further actions
		pit_trap->setVisible(true);

		getCurrentlyActingHeroActionStates()->set(HeroActionStates::ALL_DONE);
		*hero_may_continue_moving = false;

		int life_points = current_hero->getLifePoints() - 1;
		current_hero->setLifePoints(life_points);
		if (life_points == 0 && removeCreature(current_hero))
		{
		    HeroQuestLevelWindow::_hero_quest->playSoundOnce(SoundManager::SOUND_HERO_DIES);
		}
		else
		{
			HeroQuestLevelWindow::_hero_quest->playSoundOnce(SoundManager::SOUND_PIT_TRAP_TRIGGERS);
		}
		HeroQuestLevelWindow::_hero_quest->delay(TRAP_TRIGGER_DELAY);
	}

	if (try_to_jump_over_trap && !trigger_trap)
	{
		// successfully jumped over pit trap => move hero to the field behind the trap
		HeroQuestLevelWindow::_hero_quest->getPlayground()->moveCreature(current_hero, field_behind_trap);

		// update action states
		getCurrentlyActingHeroActionStates()->_move_points -= 1;

		// update the node visibility
		HeroQuestLevelWindow::_hero_quest->getPlayground()->updateFieldVisibility(*current_hero);
	}
}

void Level::handleSpearTrap(SpearTrap* spear_trap)
{
	spear_trap->setVisible(true);
	HeroQuestLevelWindow::_hero_quest->getPlayground()->update();

	Hero* current_hero = getCurrentlyActingHero();
	if (current_hero == 0)
	    return;

	// throw attack dice... skull = trap triggers, otherwise = no damage
	bool trigger_trap = true;
	vector<DiceRollPane::AttackDiceResult> attack_dice_results;
	HeroQuestLevelWindow::_hero_quest->throwAttackDice(*current_hero, 1, &attack_dice_results);
	if (attack_dice_results.empty())
	{
	    DVX(("Problem in Level::handleSpearTrap()"));
	    return;
	}
	if (attack_dice_results[0] != DiceRollPane::ATTACK_DICE_RESULT_SKULL)
		trigger_trap = false;

	if (trigger_trap)
	{
		// spear trap is triggered => trap causes damage to hero
		int life_points = current_hero->getLifePoints() - 1;
		current_hero->setLifePoints(life_points);
		if (life_points == 0 && removeCreature(current_hero))
		{
		    HeroQuestLevelWindow::_hero_quest->playSoundOnce(SoundManager::SOUND_HERO_DIES);
		}
		else
		{
			HeroQuestLevelWindow::_hero_quest->playSoundOnce(SoundManager::SOUND_SPEAR_TRAP_TRIGGERS);
		}
	}

	HeroQuestLevelWindow::_hero_quest->delay(TRAP_TRIGGER_DELAY);

	// spear trap is removed immediately
	removeTrap(spear_trap);
}

/*!
 * @return true iff the currently acting creature is hero and this hero is standing on the stairway
 */
bool Level::currentHeroIsOnStairway()
{
    Hero* hero = getCurrentlyActingHero();
    if (hero == 0)
        return false;

    const NodeID* hero_pos = HeroQuestLevelWindow::_hero_quest->getPlayground()->getCreaturePos(hero);
    if (hero_pos == 0)
        return false;

    return HeroQuestLevelWindow::_hero_quest->getPlayground()->nodeIsOnStairway(*hero_pos);
}

/*!
 * Removes the current hero from the level (hero successfully exits the level),
 * if remove_current_hero_from_level is true.
 * Switches _level_state._current_hero_idx to the next hero who still has a turn left.
 * If there is no hero having a turn left, set _level_state._current_hero_idx to
 * UINT_MAX (error).
 *
 * \param remove_current_hero_from_level
 * \return SWICHTED_TO_HERO, if there is a hero who still has a turn left;
 *         SWITCHED_TO_MONSTER, if the level has not yet ended, but no hero can act anymore in this round;
 *         LEVEL_END, otherwise.
 */
Level::SwitchToNextCreatureResult Level::switchToNextCreature(bool remove_current_hero_from_level)
{
    if (HeroQuestLevelWindow::_debug_create_attacks_creature_command_running)
        DVX(("BREAKPOINT"));
    DVX(("Level::switchToNextHero"));

    if (remove_current_hero_from_level)
    {
        DVX(
                ("remove_current_hero_from_level == true => removing %s from the level", qPrintable(getCurrentlyActingHero()->getName())));
        uint previous_hero_index = _level_state._current_hero_idx;

        removeHero(getCurrentlyActingHero(), true /*exit_successfully*/);

        // check for level end
        if (_level_state._heroes_may_exit_level_via_stairway && _acting_heroes.empty())
            return LEVEL_END;

        // set current_hero_idx to the hero who acted before the just deleted one,
        // so that the potential index increase (see below) targets the correct
        // hero in order
        if (previous_hero_index == 0)
            _level_state._current_hero_idx = _acting_heroes.size() - 1;
        else
            _level_state._current_hero_idx = previous_hero_index - 1;
    }

    // check if there is a hero who still has a turn left
    bool hero_exists_who_has_turn_left = false;
    for (uint i = 0; i < _level_state._num_turns_left.size(); ++i)
    {
        if (_level_state._num_turns_left[i] > 0)
        {
            hero_exists_who_has_turn_left = true;
            break;
        }
    }

    if (!hero_exists_who_has_turn_left)
    {
        _level_state._current_hero_idx = UINT_MAX;
        DVX(("switch to monsters"));
        return SWITCHED_TO_MONSTER;
    }

    // hero with at least 1 turn left exists => set _current_hero_idx accordingly
    uint count_search = 0;
    while (count_search < _level_state._num_turns_left.size()) // check each hero only once
    {
        ++count_search;

        ++(_level_state._current_hero_idx);
        if (_level_state._current_hero_idx == _level_state._num_turns_left.size())
            _level_state._current_hero_idx = 0;

        if (_level_state._num_turns_left[_level_state._current_hero_idx] > 0)
        {
            // found the next hero
            break;
            DVX(("Level::switchToNextHero: %s", qPrintable(getCurrentlyActingHero()->getName())));
        }
    }

    return SWITCHED_TO_HERO;
}

void Level::handleTraps(
	const NodeID& orig_hero_pos,
	const NodeID& dest_node,
	bool dest_node_is_occupied,
	bool* hero_may_continue_moving)
{
	Trap* trap = HeroQuestLevelWindow::_hero_quest->getPlayground()->getTrap(dest_node);

	PitTrap* pit_trap = dynamic_cast<PitTrap*>(trap);
	SpearTrap* spear_trap = dynamic_cast<SpearTrap*>(trap);

	// PitTrap?
	if (pit_trap != 0)
	{
		handlePitTrap(orig_hero_pos, dest_node, dest_node_is_occupied, pit_trap, hero_may_continue_moving);
	}
	// SpearTrap?
	else if (spear_trap != 0)
	{
		handleSpearTrap(spear_trap);
	}
}

/*!
 * Moves the current hero to dest_node. This requires 1 movement point + additional_movement_cost.
 * False is returned if an immediate action happens after the hero move, which has the effect that
 * the hero may not continue moving.
 * 
 * \return True, if the hero may continue acting after the move; false otherwise.
 */
bool Level::moveHero(Hero& hero, const NodeID& dest_node, unsigned int additional_movement_cost)
{
	bool hero_may_continue_moving = true;
	
	// save original hero position
	NodeID orig_hero_pos = *(HeroQuestLevelWindow::_hero_quest->getPlayground()->getCreaturePos(hero));

	// save the information if dest_node is currently occupied
	bool dest_node_is_occupied = HeroQuestLevelWindow::_hero_quest->getPlayground()->isFieldOccupiedByCreature(dest_node);

	// move current hero to dest_node
	HeroQuestLevelWindow::_hero_quest->getPlayground()->moveCreature(&hero, dest_node);
	HeroQuestLevelWindow::_hero_quest->playSoundOnce(SoundManager::SOUND_CREATURE_MOVES);

	// update action states
	if (getCurrentlyActingHeroActionStates() == 0)
	{
	    cout << "getCurrentlyActingHeroActionStates() == 0" << endl;
	    return false;
	}
	getCurrentlyActingHeroActionStates()->_move_points -= (1 + additional_movement_cost);
	getCurrentlyActingHeroActionStates()->_has_moved = true;
    getCurrentlyActingHeroActionStates()->_may_cross_monsters_during_movement = false;

	// update the node visibility
	HeroQuestLevelWindow::_hero_quest->getPlayground()->updateFieldVisibility(hero);

	// trap?
	if (HeroQuestLevelWindow::_hero_quest->getPlayground()->containsTrap(dest_node))
	{
		handleTraps(orig_hero_pos, dest_node, dest_node_is_occupied, &hero_may_continue_moving);
	}

	// redraw
	HeroQuestLevelWindow::_hero_quest->getPlayground()->update();

	// update statistics
	HeroQuestLevelWindow::_hero_quest->updateHeroStatisticPaneOfCurrentHero();

    // update buttons (finally done by GUI thread)
    HeroQuestLevelWindow::_hero_quest->updateButtons();

	HeroQuestLevelWindow::_hero_quest->delay(MOVE_DELAY);

    fieldHasBeenReachedBy(&hero, dest_node);

	return hero_may_continue_moving;
}

// entered only by GUI thread
void Level::moveHeroMovementPath(Hero& hero, const vector<NodeID>& movement_path)
{
    getCurrentlyActingHeroActionStates()->_has_acted_at_all = true;

    MoveHeroMovementPathCommand cmd(hero, movement_path);
    HeroQuestLevelWindow::_hero_quest->execute(cmd);
}

/*!
 * Moves the current monster to dest_node. This requires 1 movement point + additional_movement_cost.
 * False is returned if an immediate action happens after the hero move, which has the effect that
 * the monster may not continue moving.
 * 
 * \return True, if the monster may continue acting after the move; false otherwise.
 */
bool Level::moveCurrentMonster(const NodeID& dest_node, unsigned int additional_movement_cost)
{
	// move current monster to dest_node
	Monster* current_monster = getCurrentlyActingMonster();
	if (current_monster == 0)
	    return false;

	HeroQuestLevelWindow::_hero_quest->getPlayground()->moveCreature(current_monster, dest_node);
	HeroQuestLevelWindow::_hero_quest->getPlayground()->update();
	HeroQuestLevelWindow::_hero_quest->playSoundOnce(SoundManager::SOUND_CREATURE_MOVES);

	// update action states
	getCurrentlyActingMonsterActionStates()->_move_points -= (1 + additional_movement_cost);
	getCurrentlyActingMonsterActionStates()->_has_moved = true;

	HeroQuestLevelWindow::_hero_quest->delay(MOVE_DELAY);

	return true;
}
#if 0
/*!
 * Moves monster to dest_node, without affecting any move point counter.
 */
void Level::moveMonsterWithoutMovePoints(Monster* monster, const NodeID& dest_node)
{
	DisableRedrawWatcher disable_redraw_watcher(_hero_quest);

	// move monster to dest_node
	_hero_quest.getPlayground().moveCreature(monster, dest_node);
	_hero_quest.getSoundManager()->playSoundOnce(SoundManager::SOUND_CREATURE_MOVES);

	_hero_quest.delay(MOVE_DELAY);
}
#endif
/*!
 * Move the current monster along movement_path.
 */
void Level::moveCurrentMonster(const vector<NodeID>& movement_path)
{
	if (movement_path.empty())
		return;

	unsigned int jump_count = 0;
	for (unsigned int next_field_index = 0; next_field_index < movement_path.size(); /*increment in loop body*/ )
	{
		if (getCurrentlyActingMonsterActionStates()->_move_points > 0)
		{
			const NodeID& target_field = movement_path[next_field_index];
			++next_field_index; // update for next iteration

			// if next_field is not occupied, perform a single step to that field
			if (!HeroQuestLevelWindow::_hero_quest->getPlayground()->isFieldOccupiedByCreature(target_field))
			{
				if (!moveCurrentMonster(target_field, jump_count))
					break;
				jump_count = 0;
			}
			else
			{
				// next_field is occupied; jump over it
				++jump_count;
			}
		}
		else
			break;
	}
}
#if 0
/*!
 * Move monster along movement_path, without affecting any move point counter.
 */
void Level::moveMonsterWithoutMovePoints(Monster* monster, const vector<NodeID>& movement_path)
{
	if (movement_path.empty())
		return;

	for (unsigned int next_field_index = 0; next_field_index < movement_path.size(); /*increment in loop body*/ )
	{
		const NodeID& target_field = movement_path[next_field_index];
		++next_field_index; // update for next iteration

		// if next_field is not occupied, perform a single step to that field
		if (!_hero_quest.getPlayground().isFieldOccupiedByCreature(target_field))
		{
			moveMonsterWithoutMovePoints(monster, target_field);
		}
	}
}
#endif
/*!
 * \return A reference to a dummy instance of the monster which is the roaming monster in this level.
 */
const Monster* Level::getRoamingMonsterType() const
{
	// here: just any monster, as this parent level class isn't used as playable level anyway
	return _roaming_monster_templates.find("Goblin")->second;
}

void Level::roamingMonsterAttacksCurrentHero()
{
	Hero* hero = getCurrentlyActingHero();
	if (hero == 0)
    {
        cout << "ERROR: Level::roamingMonsterAttacksCurrentHero: there is no current hero!" << endl;
	    return;
    }
	
	// determine a place near the hero which is most near to him
	const NodeID* node_hero = HeroQuestLevelWindow::_hero_quest->getPlayground()->getCreaturePos(*hero);
	NodeID node_near_hero;
	if (!HeroQuestLevelWindow::_hero_quest->getPlayground()->computeRoamingMonsterPos(*node_hero, &node_near_hero))
	{
		cout << "A roaming monster could not be summoned, because no adequate position on the playground could be determined!" << endl;
		return;
	}

    // the hero may not act anymore
    getCurrentHeroActionStates().set(HeroActionStates::ALL_DONE);

	// create the roaming monster
	const Monster* roaming_monster_type = getRoamingMonsterType();
	Monster* roaming_monster = dynamic_cast<Monster*>(roaming_monster_type->copy());
	roaming_monster->create();
	_monsters.push_back(roaming_monster);
	_level_state._monster_action_states.push_back(Level::MonsterActionStates());
	
	// place the monster near the hero
	HeroQuestLevelWindow::_hero_quest->getPlayground()->addCreature(roaming_monster, node_near_hero);

    // clear the reachable area, as the hero won't be able to move in consequence any more
    HeroQuestLevelWindow::_hero_quest->getPlayground()->getQuestBoard()->clearReachableArea();

	// redraw the playground, so that we actually see the roaming monster
    HeroQuestLevelWindow::_hero_quest->getPlayground()->update();

	// if the monster stands directly near the hero, attack the hero;
	// otherwise do nothing
	Hero* hero_to_attack = hero;
	const vector<NodeID>& hero_neighbors = HeroQuestLevelWindow::_hero_quest->getPlayground()->getQuestBoard()->getBoardGraph()->getNode(*node_hero).getNeighbors();
	for (vector<NodeID>::const_iterator it_neighbors = hero_neighbors.begin(); it_neighbors != hero_neighbors.end(); ++it_neighbors)
	{
		if (*it_neighbors == node_near_hero)
		{
			creatureAttacksCreature(roaming_monster, hero);
			return;
		}
	}

	// hero is not attackable right now => the roaming monster does nothing further
}

/*!
 * If attack is confirmed (only when hero tries to attack other hero), the attack is executed.
 *
 * @param attacker
 * @param defender
 */
bool Level::creatureAttacksCreature(Creature* attacker, Creature* defender)
{
    DVX(
            ("Level::creatureAttacksCreature: attacker = %s, defender = %s", qPrintable(attacker->getName()), qPrintable(defender->getName())));
    // monsters are attacked without confirmation; attacking a hero needs a confirmation
    if (attacker->isHero() && defender->isHero())
    {
        bool hero_attacks_hero_confirmed = HeroQuestLevelWindow::_hero_quest->execDialogOkCancel("Really attack hero?",
                "Do you really want to attack another hero?") == QDialog::Accepted;
        if (!hero_attacks_hero_confirmed)
        {
            DV(("Level::creatureAttacksCreature: attack cancelled, as hero->hero attack was not confirmed."));
            return false;
        }
    }

    if (attacker->isHero())
    {
        getCurrentlyActingHeroActionStates()->_has_acted_at_all = true;
    }

    CreatureAttacksCreatureCommand cmd(*attacker, *defender);
    HeroQuestLevelWindow::_hero_quest->execute(cmd);

    return true;
}

/*!
 * A genie (cast by spell_caster) attacks the defender with 5 dice.
 *
 * @param spell_caster
 * @param defender
 */
bool Level::genieAttacksCreature(Creature* spell_caster, Creature* defender)
{
    DVX(
            ("Level::genieAttacksCreature: spell_caster = %s, defender = %s", qPrintable(spell_caster->getName()), qPrintable(defender->getName())));
    // monsters are attacked without confirmation; attacking a hero needs a confirmation
    if (spell_caster->isHero() && defender->isHero())
    {
        bool genie_attacks_hero_confirmed = HeroQuestLevelWindow::_hero_quest->execDialogOkCancel("Really attack hero?",
                "Do you really want to attack another hero?") == QDialog::Accepted;
        if (!genie_attacks_hero_confirmed)
        {
            DV(("Level::genieAttacksCreature: attack cancelled, as hero->hero attack was not confirmed."));
            return false;
        }
    }

    GenieAttacksCreatureCommand cmd(*spell_caster, *defender);
    HeroQuestLevelWindow::_hero_quest->execute(cmd);
    return true;
}

/*!
 * Defender is attacked by a spell (cast by spell_caster) with num_skulls skulls.
 *
 * @param spell_caster
 * @param num_skulls
 * @param defender
 */
bool Level::attackCreatureWithSkulls(Creature* spell_caster, uint num_skulls, Creature* defender)
{
    DVX(
            ("Level::attackCreatureWithSkulls: spell_caster = %s, num_skulls = %d, defender = %s", qPrintable(spell_caster->getName()), num_skulls, qPrintable(defender->getName())));
    // monsters are attacked without confirmation; attacking a hero needs a confirmation
    if (spell_caster->isHero() && defender->isHero())
    {
        bool attack_hero_confirmed = HeroQuestLevelWindow::_hero_quest->execDialogOkCancel("Really attack hero?",
                "Do you really want to attack another hero?") == QDialog::Accepted;
        if (!attack_hero_confirmed)
        {
            DV(("Level::attackCreatureWithSkulls: attack cancelled, as hero->hero attack was not confirmed."));
            return false;
        }
    }

    AttackCreatureWithSkullsCommand cmd(*spell_caster, num_skulls, *defender);
    HeroQuestLevelWindow::_hero_quest->execute(cmd);

    return true;
}

/*!
 * Defender is attacked by a spell (cast by spell_caster) with num_skulls skulls.
 * Defender may defend the attack with num_defend_dice dice.
 * @param spell_caster
 * @param num_skulls
 * @param defender
 * @param num_defend_dice
 */
bool Level::attackCreatureWithSkulls(Creature* spell_caster, uint num_skulls, Creature* defender, uint num_defend_dice)
{
    DVX(
            ("Level::attackCreatureWithSkulls: spell_caster = %s, num_skulls = %d, defender = %s, num_defend_dice = %d", qPrintable(spell_caster->getName()), num_skulls, qPrintable(defender->getName()), num_defend_dice));
    // monsters are attacked without confirmation; attacking a hero needs a confirmation
    if (spell_caster->isHero() && defender->isHero())
    {
        bool attack_hero_confirmed = HeroQuestLevelWindow::_hero_quest->execDialogOkCancel("Really attack hero?",
                "Do you really want to attack another hero?") == QDialog::Accepted;
        if (!attack_hero_confirmed)
        {
            DV(("Level::attackCreatureWithSkulls: attack cancelled, as hero->hero attack was not confirmed."));
            return false;
        }
    }

    AttackCreatureWithSkullsCommand cmd(*spell_caster, num_skulls, *defender, num_defend_dice);
    HeroQuestLevelWindow::_hero_quest->execute(cmd);

    return true;
}

void Level::currentHeroAttacks(const NodeID& dest_node)
{
	Hero* attacker = getCurrentlyActingHero();
	if (attacker == 0)
	    return;
	Creature* defender = HeroQuestLevelWindow::_hero_quest->getPlayground()->getCreature(dest_node);
	if (defender == 0)
	{
		cout << "Internal program error, Level::currentHeroAttacks" << endl;
		return;
	}
	creatureAttacksCreature(attacker, defender);
}

void Level::disarmPitTrapUsingToolbox(Hero* hero, PitTrap* pit_trap)
{
    DVX(("Level::disarmPitTrapUsingToolbox: hero = %s", qPrintable(hero->getName())));
    DisarmPitTrapUsingToolboxCommand cmd(*hero, *pit_trap);
    HeroQuestLevelWindow::_hero_quest->execute(cmd);
}

/*!
 * \returns True if creature is currently caught in a pit trap.
 */
bool Level::creatureIsCaughtInPitTrap(const Creature& creature) const
{
	const NodeID* creature_node_id = HeroQuestLevelWindow::_hero_quest->getPlayground()->getCreaturePos(creature);
	for (vector<Trap*>::const_iterator it_trap = _traps.begin(); it_trap != _traps.end(); ++it_trap)
	{
		const Trap* trap = *it_trap;
		if (trap->getNodeID() == *creature_node_id)
			return true;
	}

	return false;
}

void Level::removeTrap(Trap* trap)
{
	for (vector<Trap*>::iterator it = _traps.begin(); it != _traps.end(); ++it)
	{
		if (*it == trap)
		{
            AutoMutex am(HeroQuestLevelWindow::_hero_quest->getLevel()->getTrapsMutex());

			_traps.erase(it);
			delete trap;
			break;
		}
	}
}

Monster* Level::createMonster(const QString& name)
{
    Monster* monster = 0;

    if (name == "Goblin")
        monster = new Goblin;
    if (name == "Orc")
        monster = new Orc;
    if (name == "Fimir")
        monster = new Fimir;
    if (name == "Skeleton")
        monster = new Skeleton;
    if (name == "Zombie")
        monster = new Zombie;
    if (name == "Mummy")
        monster = new Mummy;
    if (name == "ChaosWarrior")
        monster = new ChaosWarrior;
    if (name == "Gargoyle")
        monster = new Gargoyle;

    if (monster != 0)
        monster->create();

    return monster;
}

Door* Level::createDoor(const QString& name)
{
    Door* door = 0;

    if (name == "Door")
        door = new Door;
    else if (name == "SecretDoor")
        door = new SecretDoor;

    return door;
}

Trap* Level::createTrap(const QString& name)
{
    Trap* trap = 0;

    if (name == "Trap")
        trap = new Trap;
    else if (name == "PitTrap")
        trap = new PitTrap;
    else if (name == "SpearTrap")
        trap = new SpearTrap;

    return trap;
}

Decoration* Level::createDecoration(const QString& name)
{
    Decoration* decoration = 0;

    // NonWalkable
    if (name == "Chest")
        decoration = new Chest;
    else if (name == "AlchemistTable")
        decoration = new AlchemistTable;
    else if (name == "Armory")
        decoration = new Armory;
    else if (name == "Cupboard")
        decoration = new Cupboard;
    else if (name == "Fireplace")
        decoration = new Fireplace;
    else if (name == "Sarcophagus")
        decoration = new Sarcophagus;
    else if (name == "Table")
        decoration = new Table;
    else if (name == "Throne")
        decoration = new Throne;
    else if (name == "TortureRack")
        decoration = new TortureRack;
    else if (name == "WarlockTable")
        decoration = new WarlockTable;
    else if (name == "Debris")
        decoration = new Debris;

    // Walkable
    else if (name == "Stairs")
        decoration = new Stairs;

    return decoration;
}

QMutex& Level::getTrapsMutex()
{
	return _traps_mutex;
}

/*!
 * @param room_id
 * @return True, if room with id room_id does not contain any chest, and the room has already been searched for treasures; false otherwise.
 */
bool Level::hasRoomBeenSearchedForTreasures(uint room_id) const
{
    return _non_chest_room_ids_treasures_searched.find(room_id) != _non_chest_room_ids_treasures_searched.end();
}

void Level::setNonChestRoomHasBeenSearchedForTreasures(uint room_id)
{
    _non_chest_room_ids_treasures_searched.insert(room_id);
}

list<TreasureCard>& Level::getTreasureCardDeposition()
{
    return _treasure_card_deposition;
}

const map<NodeID, TreasureDescription>& Level::getChestPosToTreasureDescription() const
{
    return _chest_pos_to_treasure_description;
}

bool Level::roomContainsChest(uint room_id) const
{
    Chest* chest = const_cast<Level*>(this)->getNextChestOfRoom(room_id, CHEST_BOTH);
    return chest != 0;
}

bool Level::roomContainsClosedChest(uint room_id) const
{
    Chest* chest = const_cast<Level*>(this)->getNextChestOfRoom(room_id, CHEST_CLOSED);
    return chest != 0;
}

Chest* Level::getNextChestOfRoom(uint room_id, ChestOpenState open_state)
{
    set<Chest*> chests;
    getDecoration(&chests); // gets all chests

    // check if any still closed chest is in room with id "room_id"
    for (set<Chest*>::iterator it = chests.begin(); it != chests.end(); ++it)
    {
        Chest* chest = *it;
        if ((open_state == CHEST_CLOSED) && chest->isOpen())
            continue;
        if ((open_state == CHEST_OPEN) && !chest->isOpen())
            continue;

        // chest is closed

        if (HeroQuestLevelWindow::_hero_quest->getPlayground()->getQuestBoard()->getRoomID(chest->getNodeTopLeft())
                == room_id)
            return chest;
    }

    return 0;
}

bool Level::save(SaveContext& save_context) const
{
    SaveContext::OpenChapter open_chapter(save_context, "Level");

    save_context.writeUInt(_acting_heroes.size(), "_acting_heroes.size()");
    for (uint i = 0; i < _acting_heroes.size(); ++i)
    {
        QString i_str;
        i_str.setNum(i);
        save_context.writeString(_acting_heroes[i]->getName(),
                QString("_acting_heroes[") + i_str + "]->getName()");
    }

    _level_state.save(save_context);

    save_context.writeUInt(_delayed_heroes.size(), "_delayed_heroes.size()");
    for (set<Hero*>::const_iterator it_delayed_heroes = _delayed_heroes.begin();
            it_delayed_heroes != _delayed_heroes.end(); ++it_delayed_heroes)
    {
        save_context.writeUInt((*it_delayed_heroes)->getReferencingID(), "delayed_hero->getReferencingID()");
    }

    save_context.writeUInt(_monsters.size(), "_monsters.size()");
    for (uint i = 0; i < _monsters.size(); ++i)
    {
        save_context.writeString(Monster::getName(_monsters[i]), "Monster::getName(_monsters[i])");
        _monsters[i]->save(save_context);
    }

    save_context.writeUInt(_doors.size(), "_doors.size()");
    for (uint i = 0; i < _doors.size(); ++i)
    {
        save_context.writeString(Door::getName(_doors[i]), "Door::getName(_doors[i])");
        _doors[i]->save(save_context);
    }

    save_context.writeUInt(_traps.size(), "_traps.size()");
    for (uint i = 0; i < _traps.size(); ++i)
    {
        save_context.writeString(Trap::getName(_traps[i]), "Trap::getName(_traps[i])");
        _traps[i]->save(save_context);
    }

    save_context.writeUInt(_decoration.size(), "_decoration.size()");
    for (uint i = 0; i < _decoration.size(); ++i)
    {
        save_context.writeString(Decoration::getName(_decoration[i]), "Decoration::getName(_decoration[i])");
        _decoration[i]->save(save_context);
    }

    save_context.writeUInt(_treasure_card_stock.size(), "_treasure_card_stock.size()");
    for (list<TreasureCard>::const_iterator it = _treasure_card_stock.begin(); it != _treasure_card_stock.end(); ++it)
    {
        it->save(save_context);
    }

    save_context.writeUInt(_treasure_card_deposition.size(), "_treasure_card_deposition.size()");
    for (list<TreasureCard>::const_iterator it = _treasure_card_deposition.begin(); it != _treasure_card_deposition.end(); ++it)
    {
        it->save(save_context);
    }

    save_context.writeUInt(_chest_pos_to_treasure_description.size(), "_chest_pos_to_treasure_description.size()");
    for (map<NodeID, TreasureDescription>::const_iterator it = _chest_pos_to_treasure_description.begin();
            it != _chest_pos_to_treasure_description.end(); ++it)
    {
        it->first.save(save_context);
        it->second.save(save_context);
    }

    save_context.writeUInt(_non_chest_room_ids_treasures_searched.size(),
            "_non_chest_room_ids_treasures_searched.size()");
    for (set<uint>::const_iterator it = _non_chest_room_ids_treasures_searched.begin(); it != _non_chest_room_ids_treasures_searched.end(); ++it)
    {
        save_context.writeUInt(*it, "_non_chest_room_ids_treasures_searched[i]");
    }

    return !save_context.fail();
}

bool Level::load(LoadContext& load_context)
{
    LoadContext::OpenChapter open_chapter(load_context, "Level");

    // _acting_heroes
    DV(("Level::load: loading heroes..."));
    _acting_heroes.clear();
    vector<Hero*>& heroes = HeroQuestLevelWindow::_hero_quest->getHeroes();
    uint num_acting_heroes;
    load_context.readUInt(&num_acting_heroes, "_acting_heroes.size()");
    for (uint i = 0; i < num_acting_heroes; ++i)
    {
        QString hero_name;
        load_context.readString(&hero_name, "hero_name");

        bool found = false;
        for (uint j = 0; j < heroes.size(); ++j)
        {
            if (hero_name == heroes[j]->getName())
            {
                found = true;
                _acting_heroes.push_back(heroes[j]);
                break;
            }
        }
        if (!found)
        {
            DVX(("Hero name \"%s\" not found!", qPrintable(hero_name)));
            return false;
        }
    }

    DV(("Level::load: loading level_state..."));
    _level_state.load(load_context);

    _delayed_heroes.clear();
    uint num_delayed_heroes;
    load_context.readUInt(&num_delayed_heroes, "_delayed_heroes.size()");
    for (uint i = 0; i < num_delayed_heroes; ++i)
    {
        uint hero_referencing_id;
        QString i_str;
        i_str.setNum(i);
        load_context.readUInt(&hero_referencing_id, QString("delayed hero ") + i_str + " referencing id");

        // find hero in vector
        Hero* target_hero = 0;
        for (uint j = 0; j < heroes.size(); ++j)
        {
            if (heroes[j]->getReferencingID() == hero_referencing_id)
            {
                target_hero = heroes[j];
                break;
            }
        }
        if (target_hero == 0)
        {
            cout << "ERROR: Level::load: could not find hero for referencing id " << hero_referencing_id << "!" << endl;
            return false;
        }
    }

    // _monsters
    deleteMonsters();
    uint num_monsters;
    load_context.readUInt(&num_monsters, "_monsters.size()");
    for (uint i = 0; i < num_monsters; ++i)
    {
        QString monster_name;
        load_context.readString(&monster_name, "monster_name");
        //DVX(("read monster name: %s", qPrintable(monster_name)));

        Monster* monster = createMonster(monster_name);
        if (monster == 0)
        {
            DVX(("Could not create monster \"%s\".", qPrintable(monster_name)));
            return false;
        }
        monster->load(load_context);
        _monsters.push_back(monster);
    }

    // doors
    deleteDoors();
    uint num_doors;
    load_context.readUInt(&num_doors, "_doors.size()");
    for (uint i = 0; i < num_doors; ++i)
    {
        QString door_name;
        load_context.readString(&door_name, "door_name");
        Door* door = createDoor(door_name);
        door->load(load_context);
        _doors.push_back(door);
    }

    // traps
    deleteTraps();
    uint num_traps;
    load_context.readUInt(&num_traps, "_traps.size()");
    for (uint i = 0; i < num_traps; ++i)
    {
        QString trap_name;
        load_context.readString(&trap_name, "trap_name");
        Trap* trap = createTrap(trap_name);
        trap->load(load_context);
        _traps.push_back(trap);
    }

    // decoration:
    deleteDecoration();
    uint num_decoration;
    load_context.readUInt(&num_decoration, "_decoration.size()");
    for (uint i = 0; i < num_decoration; ++i)
    {
        QString decoration_name;
        load_context.readString(&decoration_name, "decoration_name");
        Decoration* decoration = createDecoration(decoration_name);
        decoration->load(load_context);
        _decoration.push_back(decoration);
    }

    // treasure cards
    _treasure_card_stock.clear();
    uint num_treasure_cards_stock;
    load_context.readUInt(&num_treasure_cards_stock, "_treasure_card_stock.size()");
    for (uint i = 0; i < num_treasure_cards_stock; ++i)
    {
        TreasureCard treasure_card;
        treasure_card.load(load_context);
        _treasure_card_stock.push_back(treasure_card);
    }

    _treasure_card_deposition.clear();
    uint num_treasure_cards_deposition;
    load_context.readUInt(&num_treasure_cards_deposition, "_treasure_card_deposition.size()");
    for (uint i = 0; i < num_treasure_cards_deposition; ++i)
    {
        TreasureCard treasure_card;
        treasure_card.load(load_context);
        _treasure_card_deposition.push_back(treasure_card);
    }

    _chest_pos_to_treasure_description.clear();
    uint num_chests;
    load_context.readUInt(&num_chests, "_chest_pos_to_treasure_description.size()");
    for (uint i = 0; i < num_chests; ++i)
    {
        NodeID node_id;
        node_id.load(load_context);

        TreasureDescription treasure_description;
        treasure_description.load(load_context);
        _chest_pos_to_treasure_description[node_id] = treasure_description;
    }

    // _room_ids_treasures_searched
    _non_chest_room_ids_treasures_searched.clear();
    uint num_room_ids_treasures_searched;
    load_context.readUInt(&num_room_ids_treasures_searched, "_non_chest_room_ids_treasures_searched.size()");
    for (uint i = 0; i < num_room_ids_treasures_searched; ++i)
    {
        uint room_id;
        QString i_str;
        i_str.setNum(i);
        load_context.readUInt(&room_id, "room_id [" + i_str + "] for _non_chest_room_ids_treasures_searched");
        _non_chest_room_ids_treasures_searched.insert(room_id);
    }

    // _roaming_monster_templates: already created in Level constructor

    return !load_context.fail();
}

/*!
 * Called after save game loading: updates treasure card images in
 * std::list<TreasureCard> _treasure_card_stock and
 * std::list<TreasureCard> _treasure_card_deposition.
 */
void Level::updateTreasureImages()
{
    for (list<TreasureCard>::iterator it = _treasure_card_stock.begin(); it != _treasure_card_stock.end(); ++it)
    {
        it->updateTreasureImages();
    }

    for (list<TreasureCard>::iterator it = _treasure_card_deposition.begin(); it != _treasure_card_deposition.end(); ++it)
    {
        it->updateTreasureImages();
    }
}

/*!
 * @return True, if the current hero may attack; false otherwise.
 */
bool Level::currentHeroMayAttack() const
{
    const HeroActionStates* current_hero_action_states = getCurrentlyActingHeroActionStates();
    if (current_hero_action_states == 0)
        return false;
    if ((*current_hero_action_states)._num_attacks_left == 0)
        return false;
    if ((*current_hero_action_states)._has_cast_spell)
        return false;

    return true;
}
