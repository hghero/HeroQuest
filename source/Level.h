#ifndef LEVEL_H
#define LEVEL_H

#include <list>
#include <vector>
#include <string>
#include <iostream>
#include <set>

#include "HeroQuestLevelWindow.h"
#include "Monster.h"
#include "GraphBase.h"
#include "Trap.h"
#include "Door.h"
#include "SecretDoor.h"
#include "Command.h"
#include "TreasureCard.h"
#include "SpellCard.h"

#define DEBUG_MODE_SAVE_GAMES
#define DEBUG_MODE_SAVE_GAMES_DIR "C:/Users/Hagen/Desktop/hqsave"

class Decoration;
class Chest;

class Level
{
    friend class MoveHeroMovementPathCommand;
    friend class CreatureAttacksCreatureCommand;
    friend class ExecuteMonsterTurnsCommand;
    friend class OpenChestCommand;
    friend class DrawTreasureCardCommand;
    friend class HeroQuestLevelWindow;

public:
    struct ActionStates {
        enum SetType
        {
            ALL_DONE = 0
        };
        
        ActionStates();
        virtual ~ActionStates();

        void reset();
        void set(const SetType& set_type);

        virtual bool save(std::ostream& stream) const;
        virtual bool load(std::istream& stream);

        bool _has_acted_at_all;
        bool _has_moved;
        unsigned int _num_attacks_left;
        bool _has_cast_spell;
        unsigned int _num_spell_casts_left;
        unsigned int _move_points;
        bool _sleeping;
    };

    struct HeroActionStates : ActionStates {
        typedef ActionStates Parent;

        HeroActionStates();

        void initOnStartTurn(const Hero& hero);

        void reset(bool start_turn);
        void set(const SetType& set_type);

        virtual bool save(std::ostream& stream) const;
        virtual bool load(std::istream& stream);

        bool _has_searched;
        bool _has_thrown_movement_dice;
        bool _may_cross_monsters_during_movement;
        std::vector<uint> _dice_results;
    };

    struct MonsterActionStates : ActionStates {
        typedef ActionStates Parent;

        MonsterActionStates();

        void init(const Monster& monster);

        void reset();

        virtual bool save(std::ostream& stream) const;
        virtual bool load(std::istream& stream);
    };

    struct LevelState
    {
        LevelState()
        : _round_no(0),
          _current_hero_idx(0), // heroes always start
          _current_monster_idx(UINT_MAX),
          _hero_action_states(),
          _monster_action_states(),
          _num_turns_left()
        {
            // NIX
        }

        bool isHeroActing() const
        {
            return _current_hero_idx != UINT_MAX;
        }

        bool isMonsterActing() const
        {
            return _current_monster_idx != UINT_MAX;
        }

        bool save(std::ostream& stream) const;
        bool load(std::istream& stream);

        uint _round_no; // number of current round
        uint _current_hero_idx; // index of current hero; index UINT_MAX means that no hero is currently acting
        uint _current_monster_idx; // index of current monster; index UINT_MAX means that no monster is currently acting

        std::vector<HeroActionStates> _hero_action_states;
        std::vector<MonsterActionStates> _monster_action_states;

        std::vector<uint> _num_turns_left; // hero index => number of turns left (usually 1)
    };

    enum ChestOpenState
    {
        CHEST_OPEN = 0, CHEST_CLOSED, CHEST_BOTH
    };

	static const unsigned int MOVE_DELAY;
	static const unsigned int ATTACK_DELAY;
	static const unsigned int ATTACK_FINISHED_DELAY;
	static const unsigned int TRAP_TRIGGER_DELAY;

    Level(uint num_heroes);
	virtual ~Level();

	virtual QString getName() const;

	void create();

    void setActingHeroes(const std::vector<Hero*>& heroes);

	Monster* getMonster(uint referencing_id);
    std::pair<Monster*, uint> getMonsterWithIndex(uint referencing_id);
    const std::vector<Monster*>& getMonsters() const;

	// inputs
	void initCurrentHeroActionStateToMoveOrAttack();
	void searchTrapsButtonClicked();
	void searchTreasuresButtonClicked();
	void delayTurnButtonClicked();
	void endTurnButtonClicked();

	const std::vector<Door*>& getDoors() const;
	std::vector<Door*>& getDoors();

	const std::vector<Trap*>& getTraps() const;
	std::vector<Trap*>& getTraps();

	const std::vector<Decoration*>& getDecoration() const;
	std::vector<Decoration*>& getDecoration();
    Decoration* getDecoration(uint referencing_id);

    /*!
     * Fills set "derived_decoration" with all Decorations in _decoration which are of type T.
     *
     * @param derived_decoration
     */
    template<typename T>
    void getDecoration(std::set<T*>* derived_decorations)
    {
        for (std::vector<Decoration*>::iterator it = _decoration.begin(); it != _decoration.end(); ++it)
        {
            T* casted_decoration = dynamic_cast<T*>(*it);
            if (casted_decoration != 0)
            {
                derived_decorations->insert(casted_decoration);
            }
        }
    }

	const Door* getDoor(const std::pair<NodeID, NodeID>& transition) const;

	bool getIncidentDoors(const NodeID& node_id, std::vector<Door*>* incident_doors);
	bool hasIncidentClosedDoors(const NodeID& node_id);
	bool getIncidentClosedDoors(const NodeID& node_id, std::vector<Door*>* incident_closed_doors);

	void openDoor(const Door* destination_door);

    void moveHeroMovementPath(Hero& hero, const std::vector<NodeID>& movement_path);

	bool moveCurrentMonster(const NodeID& dest_node, unsigned int additional_movement_cost);
	void moveCurrentMonster(const std::vector<NodeID>& movement_path);

    bool creatureAttacksCreature(Creature* attacker, Creature* defender);
    bool genieAttacksCreature(Creature* spell_caster, Creature* defender);
    bool attackCreatureWithSkulls(Creature* spell_caster, uint num_skulls, Creature* defender);
    bool attackCreatureWithSkulls(Creature* spell_caster, uint num_skulls, Creature* defender, uint num_defend_dice);
	void roamingMonsterAttacksCurrentHero();
	void currentHeroAttacks(const NodeID& dest_node);

	const LevelState& getLevelState() const;
	LevelState* getLevelState();
	Level::HeroActionStates* getCurrentlyActingHeroActionStates();
    const Level::HeroActionStates* getCurrentlyActingHeroActionStates() const;
	Level::HeroActionStates* getHeroActionStates(Hero* hero);
    Level::MonsterActionStates* getMonsterActionStates(Monster* monster);
    Level::ActionStates* getCreatureActionStates(Creature* creature);
	Level::MonsterActionStates* getCurrentlyActingMonsterActionStates();
    Level::ActionStates* getCurrentlyActingCreatureActionStates();

	const std::vector<Hero*>& getActingHeroes() const;

	bool heroesAreMoving() const;
	bool monstersAreMoving() const;

    void startRound();

	Creature* getCurrentlyActingCreature();
	Hero* getCurrentlyActingHero();
	Monster* getCurrentlyActingMonster();

	virtual const Monster* getRoamingMonsterType() const;

	bool creatureIsCaughtInPitTrap(const Creature& creature) const;

	QMutex& getTrapsMutex();

	//void discardTreasureCard(const TreasureCard& treasure_card);
	bool hasRoomBeenSearchedForTreasures(uint room_id) const;
	void setNonChestRoomHasBeenSearchedForTreasures(uint room_id);
	std::list<TreasureCard>& getTreasureCardDeposition();
    const std::map<NodeID, TreasureDescription>& getChestPosToTreasureDescription() const;
    bool roomContainsChest(uint room_id) const;
    bool roomContainsClosedChest(uint room_id) const;
    Chest* getNextChestOfRoom(uint room_id, ChestOpenState open_state);

	bool removeCreature(Creature* creature);
    bool isCreatureAlive(Creature* creature);

    virtual bool save(std::ostream& stream) const;
    virtual bool load(std::istream& stream);

    void updateTreasureImages();

    bool currentHeroMayAttack() const;

    virtual QString getLevelBriefing() const;

protected:
	bool prepareTreasureCards();
	bool prepareSpellCards();

	bool moveHero(Hero& hero, const NodeID& dest_node, unsigned int additional_movement_cost);

    void startHeroTurn();
	void endHeroTurn();

	void throwMovementDice();

	Level::HeroActionStates& getCurrentHeroActionStates();
	
	virtual void placeHeroes();
	virtual void createMonsters();
	virtual void createDoors();
	virtual void createTraps();
	virtual void createDecoration();
    virtual void createChestTreasureDescriptions();

	bool removeMonster(Monster* monster);
	bool removeHero(Hero* hero);
	void removeTrap(Trap* trap);

	/*!
	 * Creates a monster of type CreatureType and places it at node_id on the playground.
	 *
	 * \return The created monster.
	 */
	template <class CreatureType>
	CreatureType* createMonster(const NodeID& node_id)
	{
		CreatureType* monster = new CreatureType;
		monster->create();
		_monsters.push_back(monster);
		HeroQuestLevelWindow::_hero_quest->addMonsterToLevelAndPlayground(monster, node_id);

		return monster;
	}

	Monster* createMonster(const QString& name);
	Door* createDoor(const QString& name);
	Trap* createTrap(const QString& name);
	Decoration* createDecoration(const QString& name);

	void deleteMonsters();
	void deleteDoors();
	void deleteTraps();
	void deleteDecoration();

	int computeWeakestHeroes(std::vector<Hero*>* weakest_heroes);
	int computeWeakestHeroes(const std::vector<Hero*>& considered_heroes, std::vector<Hero*>* weakest_heroes);

	void handleTraps(
		const NodeID& orig_hero_pos,
		const NodeID& dest_node,
		bool dest_node_is_occupied,
		bool* hero_may_continue_moving);

	void handlePitTrap(
		const NodeID& orig_hero_pos,
		const NodeID& dest_node,
		bool dest_node_is_occupied,
		PitTrap* pit_trap,
		bool* hero_may_continue_moving);

	void handleSpearTrap(SpearTrap* spear_trap);

private:
	bool switchToNextHero();

    void shuffleTreasureCardDepositionIntoStock();
    void shuffleTreasureCardStock();
    void openChest(uint room_id);
	void drawTreasureCard();

	uint getHeroIndex(Hero* hero) const;
    uint getMonsterIndex(Monster* monster) const;
	
    void debugModeSaveGames();

protected:
	std::vector<Hero*> _acting_heroes;

	LevelState _level_state;

    std::set<Hero*> _delayed_heroes;

	// monsters are created and deleted in the level (cf. derived level ::createMonsters)
	std::vector<Monster*> _monsters;

	// regular and secret doors
	std::vector<Door*> _doors;

	// pit traps, spear traps, and boulder traps
	std::vector<Trap*> _traps;

	QMutex _traps_mutex;

	// decoration
	std::vector<Decoration*> _decoration;

	// treasure cards
	std::list<TreasureCard> _treasure_card_stock;
	std::list<TreasureCard> _treasure_card_deposition;

    // treasure descriptions for chests
    std::map<NodeID, TreasureDescription> _chest_pos_to_treasure_description;

	// contains the IDs of the rooms in which a hero has already searched for treasures
	std::set<uint> _non_chest_room_ids_treasures_searched;

	std::map<QString, Monster*> _roaming_monster_templates;

	// spell cards
	std::vector<SpellCard> _spell_card_stock;
	std::vector<SpellCard> _spell_card_deposition;
};

#endif
