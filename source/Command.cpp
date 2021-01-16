#include "Command.h"

#include <vector>
#include <stdlib.h>

#include "Hero.h"
#include "HeroQuestLevelWindow.h"
#include "Playground.h"
#include "QuestBoard.h"
#include "DiceRollPane.h"
#include "DialogTreasureCard.h"
#include "Debug.h"
#include "Level.h"
#include "NonWalkableDecoration.h"

using namespace std;


// =================================================================

Command::Command()
: QThread(),
  _name("undefined"),
  _success(false)
{
    // NIX
}

Command::Command(const QString& name)
: _name(name),
  _success(false)
{
    // NIX
}

Command::~Command()
{
    // NIX
}

bool Command::is(const QString& name) const
{
    return name == _name;
}

const QString& Command::getName() const
{
    return _name;
}

void Command::run()
{
    // NIX
}

bool Command::succeeded() const
{
    return _success;
}

// =================================================================

const QString MoveHeroMovementPathCommand::NAME = "MoveHeroMovementPathCommand";

MoveHeroMovementPathCommand::MoveHeroMovementPathCommand(Hero& hero, const std::vector<NodeID>& movement_path)
: Command(NAME),
  _movement_path(movement_path),
  _hero(hero)
{
    // NIX
}

MoveHeroMovementPathCommand::~MoveHeroMovementPathCommand()
{
    // NIX
}

void MoveHeroMovementPathCommand::run()
{
    if (_movement_path.empty())
    {
        _success = true;
        return;
    }

    unsigned int additional_movement_cost = 0; // used for fields which are "jumped" over (fields blocked by other creatures, and traps)
    for (unsigned int next_field_index = 0; next_field_index < _movement_path.size(); /*increment in loop body*/ )
    {
        if (HeroQuestLevelWindow::_hero_quest->getLevel()->getCurrentlyActingHeroActionStates()->_move_points > 0)
        {
            const NodeID& target_field = _movement_path[next_field_index];
            ++next_field_index; // update for next iteration

            if (!HeroQuestLevelWindow::_hero_quest->getPlayground()->isFieldOccupied(target_field))
            {
                // target_field not occupied => move to that field
                if (!HeroQuestLevelWindow::_hero_quest->getLevel()->moveHero(_hero, target_field, additional_movement_cost))
                    break;
                additional_movement_cost = 0;
            }
            else if (!HeroQuestLevelWindow::_hero_quest->getPlayground()->containsTrap(target_field))
            {
                // occupied, but no trap
                // => implicitly move over target_field to next field, because there can never be more than 1 creature on a field
                ++additional_movement_cost;
            }
            else
            {
                // occupied, and the creature occupying the field is in a trap
                // => cannot pass this field; end the creature movement (but successive movements remain possible)
                break;
            }
        }
        else
        {
            // no movement points left
            break;
        }
    }

    _success = true;

    HeroQuestLevelWindow::_hero_quest->getPlayground()->getQuestBoard()->computeReachableArea();
    HeroQuestLevelWindow::_hero_quest->getPlayground()->update();
}

// =================================================================

CreatureAttacksCreatureCommand::CreatureAttacksCreatureCommand(Creature& attacker, Creature& defender)
: _attacker(attacker),
  _defender(defender)
{
    // NIX
}

CreatureAttacksCreatureCommand::~CreatureAttacksCreatureCommand()
{
    // NIX
}

/*!
 * The _defender throws num_skulls to defend an attack. Sounds are handled.
 * If the _defender dies, he is removed from the level.
 *
 * @param num_skulls number of defend dice
 * @param use_custom_num_defend_dice If true, use num_defend_dice to determine the number of defend dice; otherwise use the _defender's number of defend dice.
 * @param num_defend_dice Use this number of defend dice, if use_custom_num_defend_dice is true
 */
void CreatureAttacksCreatureCommand::throwDefendDiceIfNecessary(unsigned int num_skulls,
        bool use_custom_num_defend_dice, uint num_defend_dice)
{
    // throw the defend dice, if necessary
    if (num_skulls != 0)
    {
        DVX(("num_skulls != 0"));
        // If the defender is a hero, he must be asked if he
        // - wants to drink his immunization potion (if he has one) before the defence
        if (_defender.isHero())
        {
            dynamic_cast<Hero&>(_defender).checkDefend();
        }
        std::vector<DiceRollPane::AttackDiceResult> defend_dice_results;
        if (use_custom_num_defend_dice)
            HeroQuestLevelWindow::_hero_quest->throwDefendDice(_defender, num_defend_dice, &defend_dice_results);
        else
            HeroQuestLevelWindow::_hero_quest->throwDefendDice(_defender, &defend_dice_results);

        unsigned int num_shields = 0;
        for (unsigned int i = 0; i < defend_dice_results.size(); ++i)
        {
            if (_defender.isHero() && (defend_dice_results[i] == DiceRollPane::ATTACK_DICE_RESULT_HUMAN_SHIELD))
                ++num_shields;
            else if (_defender.isMonster()
                    && (defend_dice_results[i] == DiceRollPane::ATTACK_DICE_RESULT_MONSTER_SHIELD))
                ++num_shields;
        }
        // does the attacked creature lose life points?
        int life_point_decrease = int(num_skulls) - int(num_shields);
        if (life_point_decrease < 0)
            life_point_decrease = 0;

        if (life_point_decrease != 0)
        {
            DVX(("life_point_decrease != 0"));
            // the attacked creature loses life points
            int current_life_points = _defender.getLifePoints();
            current_life_points -= life_point_decrease;
            if (current_life_points < 0)
                current_life_points = 0;

            _defender.setLifePoints(current_life_points);
            HeroQuestLevelWindow::_hero_quest->delay(Level::ATTACK_DELAY);
            // if the creature has died, remove it from the playground
            if (current_life_points == 0)
            {
                if (_defender.isHero())
                {
                    // hero
                    if (HeroQuestLevelWindow::_hero_quest->getLevel()->removeCreature(&_defender))
                        HeroQuestLevelWindow::_hero_quest->playSoundOnce(SoundManager::SOUND_HERO_DIES);
                    else
                    {
                        if (life_point_decrease == 1)
                            HeroQuestLevelWindow::_hero_quest->playSoundOnce(
                                    SoundManager::SOUND_ATTACK_HERO_SMALL_DECREASE_LIFE_POINTS);
                        else
                            HeroQuestLevelWindow::_hero_quest->playSoundOnce(
                                    SoundManager::SOUND_ATTACK_HERO_BIG_DECREASE_LIFE_POINTS);
                    }
                }
                else
                {
                    // monster
                    HeroQuestLevelWindow::_hero_quest->getLevel()->removeCreature(&_defender);
                    HeroQuestLevelWindow::_hero_quest->playSoundOnce(SoundManager::SOUND_MONSTER_DIES);
                }
            }
            else
            {
                if (_defender.isHero())
                {
                    if (life_point_decrease == 1)
                        HeroQuestLevelWindow::_hero_quest->playSoundOnce(
                                SoundManager::SOUND_ATTACK_HERO_SMALL_DECREASE_LIFE_POINTS);
                    else
                        HeroQuestLevelWindow::_hero_quest->playSoundOnce(
                                SoundManager::SOUND_ATTACK_HERO_BIG_DECREASE_LIFE_POINTS);
                }
                else
                {
                    if (life_point_decrease == 1)
                        HeroQuestLevelWindow::_hero_quest->playSoundOnce(
                                SoundManager::SOUND_ATTACK_MONSTER_SMALL_DECREASE_LIFE_POINTS);
                    else
                        HeroQuestLevelWindow::_hero_quest->playSoundOnce(
                                SoundManager::SOUND_ATTACK_MONSTER_BIG_DECREASE_LIFE_POINTS);
                }
            }
        }
        else
        {
            // life_point_decrease == 0
            HeroQuestLevelWindow::_hero_quest->playSoundOnce(SoundManager::SOUND_ATTACK_DEFENDED);
            HeroQuestLevelWindow::_hero_quest->delay(Level::ATTACK_DELAY);
        }
        HeroQuestLevelWindow::_hero_quest->delay(Level::ATTACK_FINISHED_DELAY);
    }
}

void CreatureAttacksCreatureCommand::run()
{
    HeroQuestLevelWindow::_debug_create_attacks_creature_command_running = true;
    DVX(("CreatureAttacksCreatureCommand::run BEGIN"));

    // throw the attack dice
    std::vector<DiceRollPane::AttackDiceResult> attack_dice_results;
    HeroQuestLevelWindow::_hero_quest->throwAttackDice(_attacker, &attack_dice_results);

    unsigned int num_skulls = 0;
    for (unsigned int i = 0; i < attack_dice_results.size(); ++i)
    {
        if (attack_dice_results[i] == DiceRollPane::ATTACK_DICE_RESULT_SKULL)
            ++num_skulls;
    }

    if (num_skulls == 0)
        HeroQuestLevelWindow::_hero_quest->playSoundOnce(SoundManager::SOUND_ATTACK_NO_SKULLS);
    HeroQuestLevelWindow::_hero_quest->delay(Level::ATTACK_DELAY);

    // throw the defend dice, if necessary
    throwDefendDiceIfNecessary(num_skulls, false, 0);

    // update the action states
    DVX(("CreatureAttacksCreatureCommand::run: update action states"));
    if (_attacker.isHero())
    {
        if (HeroQuestLevelWindow::_hero_quest->getLevel()->getCurrentlyActingHeroActionStates()->_num_attacks_left > 0)
        {
            --(HeroQuestLevelWindow::_hero_quest->getLevel()->getCurrentlyActingHeroActionStates()->_num_attacks_left);
            DVX(
                    ("decrease num_attacks_left of attacker %s, while currently active hero is %s", qPrintable(_attacker.getName()), qPrintable(HeroQuestLevelWindow::_hero_quest->getLevel()->getCurrentlyActingHero()->getName())));
        }
        // move => attack => no more move!
        if (HeroQuestLevelWindow::_hero_quest->getLevel()->getCurrentlyActingHeroActionStates()->_has_moved)
        {
            HeroQuestLevelWindow::_hero_quest->getLevel()->getCurrentlyActingHeroActionStates()->_move_points = 0;
        }
        // having attacked means that searching is not possible anymore
        HeroQuestLevelWindow::_hero_quest->getLevel()->getCurrentlyActingHeroActionStates()->_has_searched = true;
        // having attacked means that casting a spell is not possible anymore
        HeroQuestLevelWindow::_hero_quest->getLevel()->getCurrentlyActingHeroActionStates()->_has_cast_spell = true;
        HeroQuestLevelWindow::_hero_quest->getLevel()->getCurrentlyActingHeroActionStates()->_num_spell_casts_left = 0;
    }
    else if (_attacker.isMonster())
    {
        if (HeroQuestLevelWindow::_hero_quest->getLevel()->getCurrentlyActingMonsterActionStates()->_num_attacks_left > 0)
            --(HeroQuestLevelWindow::_hero_quest->getLevel()->getCurrentlyActingMonsterActionStates()->_num_attacks_left);
        // move => attack => no more move!
        if (HeroQuestLevelWindow::_hero_quest->getLevel()->getCurrentlyActingMonsterActionStates()->_has_moved)
            HeroQuestLevelWindow::_hero_quest->getLevel()->getCurrentlyActingMonsterActionStates()->_move_points = 0;
    }

    // reset the playground
    DV(("CreatureAttacksCreatureCommand::run: reset playground"));
    if (_attacker.isHero())
        HeroQuestLevelWindow::_hero_quest->getPlayground()->setActionMode(
                Playground::ACTION_MODE_MOVE_OR_ATTACK_OR_OPEN_DOOR);

    // --- update the turn activity buttons ---
    DV(("CreatureAttacksCreatureCommand::run: update buttons"));
    if (_attacker.isHero())
    {
        // (1) disable attack button if no more attack is allowed
        // (2) no move after attack, when already moved before
        HeroQuestLevelWindow::_hero_quest->updateButtons();
    }

    // update the field visibility
    if (_attacker.isHero())
    {
        DV(("CreatureAttacksCreatureCommand::run: update field visibility"));
        HeroQuestLevelWindow::_hero_quest->getPlayground()->updateFieldVisibility(_attacker);
    }

    // update the reachable area
    if (_attacker.isHero())
    {
        DV(("CreatureAttacksCreatureCommand::run: compute reachable area"));
        HeroQuestLevelWindow::_hero_quest->getPlayground()->getQuestBoard()->computeReachableArea();
    }

    DV(("CreatureAttacksCreatureCommand::run: update playground"));
    HeroQuestLevelWindow::_hero_quest->getPlayground()->update();

    // update the statistics
    DV(("CreatureAttacksCreatureCommand::run: update statistics"));
    HeroQuestLevelWindow::_hero_quest->updateHeroStatisticPanes();

    DVX(("CreatureAttacksCreatureCommand::run END"));
    HeroQuestLevelWindow::_debug_create_attacks_creature_command_running = false;
}

// =================================================================

GenieAttacksCreatureCommand::GenieAttacksCreatureCommand(Creature& spell_caster, Creature& defender)
        :
        CreatureAttacksCreatureCommand(spell_caster, defender), // spell_caster set as attacker, but not used actually
        _spell_caster(spell_caster)
{
    // NIX
}

GenieAttacksCreatureCommand::~GenieAttacksCreatureCommand()
{
    // NIX
}

void GenieAttacksCreatureCommand::run()
{
    DVX(("GenieAttacksCreatureCommand::run BEGIN"));

    // throw the attack dice
    std::vector<DiceRollPane::AttackDiceResult> attack_dice_results;
    HeroQuestLevelWindow::_hero_quest->throwAttackDice(_spell_caster, 5, &attack_dice_results);

    unsigned int num_skulls = 0;
    for (unsigned int i = 0; i < attack_dice_results.size(); ++i)
    {
        if (attack_dice_results[i] == DiceRollPane::ATTACK_DICE_RESULT_SKULL)
            ++num_skulls;
    }

    if (num_skulls == 0)
        HeroQuestLevelWindow::_hero_quest->playSoundOnce(SoundManager::SOUND_ATTACK_NO_SKULLS);
    HeroQuestLevelWindow::_hero_quest->delay(Level::ATTACK_DELAY);

    // throw the defend dice, if necessary
    throwDefendDiceIfNecessary(num_skulls, false, 0);

    // update the field visibility
    if (_spell_caster.isHero())
    {
        DV(("GenieAttacksCreatureCommand::run: update field visibility"));
        HeroQuestLevelWindow::_hero_quest->getPlayground()->updateFieldVisibility(_spell_caster);
    }

    // update the reachable area
    if (_spell_caster.isHero())
    {
        DV(("GenieAttacksCreatureCommand::run: compute reachable area"));
        HeroQuestLevelWindow::_hero_quest->getPlayground()->getQuestBoard()->computeReachableArea();
    }

    DV(("GenieAttacksCreatureCommand::run: update playground"));
    HeroQuestLevelWindow::_hero_quest->getPlayground()->update();

    // update the statistics
    DV(("GenieAttacksCreatureCommand::run: update statistics"));
    HeroQuestLevelWindow::_hero_quest->updateHeroStatisticPanes();

    DVX(("GenieAttacksCreatureCommand::run END"));
}

// =================================================================

AttackCreatureWithSkullsCommand::AttackCreatureWithSkullsCommand(Creature& spell_caster, uint num_skulls,
        Creature& defender)
        :
        CreatureAttacksCreatureCommand(spell_caster, defender), // spell_caster set as attacker, but not used actually
        _spell_caster(spell_caster), _num_skulls(num_skulls), _use_num_defend_dice(false), _num_defend_dice(0)
{
    // NIX
}

AttackCreatureWithSkullsCommand::AttackCreatureWithSkullsCommand(Creature& spell_caster, uint num_skulls,
        Creature& defender, uint num_defend_dice)
        :
        CreatureAttacksCreatureCommand(spell_caster, defender), // spell_caster set as attacker, but not used actually
        _spell_caster(spell_caster), _num_skulls(num_skulls), _use_num_defend_dice(true), _num_defend_dice(
                num_defend_dice)
{
    // NIX
}

AttackCreatureWithSkullsCommand::~AttackCreatureWithSkullsCommand()
{
    // NIX
}

void AttackCreatureWithSkullsCommand::run()
{
    DVX(("AttackCreatureWithSkullsCommand::run BEGIN"));

    HeroQuestLevelWindow::_hero_quest->getDiceRollPane()->setAttackSkulls(&_spell_caster, _num_skulls);

    // throw the defend dice, if necessary
    throwDefendDiceIfNecessary(_num_skulls, _use_num_defend_dice, _num_defend_dice);

    // update the field visibility
    if (_spell_caster.isHero())
    {
        DV(("AttackCreatureWithSkullsCommand::run: update field visibility"));
        HeroQuestLevelWindow::_hero_quest->getPlayground()->updateFieldVisibility(_spell_caster);
    }

    // update the reachable area
    if (_spell_caster.isHero())
    {
        DV(("AttackCreatureWithSkullsCommand::run: compute reachable area"));
        HeroQuestLevelWindow::_hero_quest->getPlayground()->getQuestBoard()->computeReachableArea();
    }

    DV(("AttackCreatureWithSkullsCommand::run: update playground"));
    HeroQuestLevelWindow::_hero_quest->getPlayground()->update();

    DVX(("AttackCreatureWithSkullsCommand::run END"));
}

// =================================================================

const QString ExecuteMonsterTurnsCommand::NAME = "ExecuteMonsterTurnsCommand";

ExecuteMonsterTurnsCommand::ExecuteMonsterTurnsCommand()
{
    // NIX
}

ExecuteMonsterTurnsCommand::~ExecuteMonsterTurnsCommand()
{
    // NIX
}

void ExecuteMonsterTurnsCommand::run()
{
    Level* level = HeroQuestLevelWindow::_hero_quest->getLevel();
    Level::LevelState* level_state = HeroQuestLevelWindow::_hero_quest->getLevel()->getLevelState();

    // monsters as referencing ids
    vector<uint> monster_ids;
    monster_ids.reserve(level->getMonsters().size());
    for (uint i = 0; i < level->getMonsters().size(); ++i)
    {
        monster_ids.push_back(level->getMonsters()[i]->getReferencingID());
    }

    for (vector<uint>::iterator it = monster_ids.begin(); it != monster_ids.end(); ++it)
    {
        pair<Monster*, uint> current_monster_with_index = level->getMonsterWithIndex(*it);
        if (current_monster_with_index.first == 0)
        {
            // this monster is already dead => go to next monster
            continue;
        }

        level_state->_current_monster_idx = current_monster_with_index.second;
        executeCurrentMonsterActions();
    }

    // all monsters moved => switch to next round and give over control to the user to move the heroes
    HeroQuestLevelWindow::_hero_quest->getLevel()->startRound();
}

void ExecuteMonsterTurnsCommand::executeCurrentMonsterActions()
{
    Level* level = HeroQuestLevelWindow::_hero_quest->getLevel();

    Creature* current_creature = level->getCurrentlyActingCreature();
    Monster* current_monster = dynamic_cast<Monster*>(current_creature);
    if (current_monster == 0)
        return;

    // may the monster move in this round?
    if (current_monster->getSkipRounds() > 0)
    {
        current_monster->decreaseSkipRounds();
        return;
    }

    const NodeID* current_monster_pos = HeroQuestLevelWindow::_hero_quest->getPlayground()->getCreaturePos(current_monster);
    if (!HeroQuestLevelWindow::_hero_quest->getPlayground()->getNodeVisibility(*current_monster_pos))
    {
        // current_monster is not visible => switch to next monster
        return;
    }

    Level::MonsterActionStates* monster_action_states = level->getCurrentlyActingMonsterActionStates();
    if (monster_action_states == 0)
    {
        DVX(("Error: no monster currently acting!"));
        return;
    }

    // sleeping monster must throw a 6 before waking up
    if (monster_action_states->_sleeping)
    {
        if (!HeroQuestLevelWindow::_hero_quest->creatureTriesToWakeUp(*current_monster))
        {
            // still sleeping => switch to next monster
            return;
        }
    }

    // prepare monster
    monster_action_states->init(*current_monster);

    // prepare dice roll panel
    HeroQuestLevelWindow::_hero_quest->initActionPane();

    // re-render the playground, so that the highlight of the currently acting monster is updated
    HeroQuestLevelWindow::_hero_quest->getPlayground()->update();

    // === artificial intelligence for monster ===
    // (a hero is weak, if (life_points - attack_dice + defend_dice) is minimal)
    // phase 1: attack if possible:
    //          (a) if there is a hero next to current_monster, determine the weakest hero (a.2) of the adjacent heroes (a.1)
    //          (b) attack (b.2) one of these (randomly (b.1))
    //          (c) has_attacked = true

    // (a.1)
    vector<Hero*> adjacent_heroes;
    HeroQuestLevelWindow::_hero_quest->getPlayground()->computeAdjacentHeroes(current_monster, &adjacent_heroes);

    if (adjacent_heroes.size() != 0)
    {
        // (a.2)
        vector<Hero*> weakest_heroes;
        level->computeWeakestHeroes(adjacent_heroes, &weakest_heroes);

        if (weakest_heroes.size() != 0)
        {
            // (b.1)
            unsigned int weakest_hero_index = rand() % weakest_heroes.size();
            Hero* destination_hero = weakest_heroes[weakest_hero_index];

            // (b.2)
            CreatureAttacksCreatureCommand cmd(*current_monster, *destination_hero);
            cmd.run();

            // (c)
            if (level->getCurrentlyActingMonsterActionStates()->_num_attacks_left > 0)
                --(level->getCurrentlyActingMonsterActionStates()->_num_attacks_left);
        }
    }

    // phase 2: move:
    //          (a) if has_attacked, then move to (a.4) one of the nearest (a.3) of the weakest (a.2) of the reachable heroes (a.1)
    //          (b) else, then
    //              (b.1) if a hero can be reached now: move to one of the weakest of the reachable heroes
    //              (b.2) else, then move to the weakest of the nearest heroes
    if (level->getCurrentlyActingMonsterActionStates()->_num_attacks_left > 0)
    {
        // (a.1): compute reachable heroes
        vector<Hero*> reachable_heroes_1_turn;
        HeroQuestLevelWindow::_hero_quest->getPlayground()->computeReachableHeroes(current_monster, true, &reachable_heroes_1_turn);  // reachable in 1 turn?

        // (a.2): compute weakest of these
        vector<Hero*> weakest_heroes_1_turn;
        level->computeWeakestHeroes(reachable_heroes_1_turn, &weakest_heroes_1_turn);

        if (!weakest_heroes_1_turn.empty())
        {
            // (a.3): attack one of the weakest
            Hero* hero_target = getRandomElementByValue<Hero*>(reachable_heroes_1_turn);

            // (a.4): move to hero_target
            vector<NodeID> path_to_hero_target;
            HeroQuestLevelWindow::_hero_quest->getPlayground()->computeShortestPathNextTo(current_monster, hero_target,
                    false, true, false, level->getCurrentlyActingMonsterActionStates()->_move_points,
                    &path_to_hero_target);
            level->moveCurrentMonster(path_to_hero_target);
        }
        else
        {
            // (a.5): compute reachable heroes
            vector<Hero*> reachable_heroes;
            HeroQuestLevelWindow::_hero_quest->getPlayground()->computeReachableHeroes(current_monster, false, &reachable_heroes);

            // (a.6): compute weakest of these
            vector<Hero*> weakest_heroes;
            level->computeWeakestHeroes(reachable_heroes, &weakest_heroes);

            // (a.7): get nearest of these
            vector<Hero*> nearest_weakest_heroes;
            HeroQuestLevelWindow::_hero_quest->getPlayground()->computeNearestHeroes(current_monster, weakest_heroes, &nearest_weakest_heroes);
            if (!nearest_weakest_heroes.empty())
            {
                Hero* hero_target = getRandomElementByValue<Hero*>(nearest_weakest_heroes);

                // (a.8): move to hero_target
                vector<NodeID> path_to_hero_target;
                HeroQuestLevelWindow::_hero_quest->getPlayground()->computeShortestPathNextTo(current_monster,
                        hero_target, false, true, false, level->getCurrentlyActingMonsterActionStates()->_move_points,
                        &path_to_hero_target);
                level->moveCurrentMonster(path_to_hero_target);
            }
        }
    }
    else
    {
        // (b)
        vector<Hero*> reachable_heroes;
        HeroQuestLevelWindow::_hero_quest->getPlayground()->computeReachableHeroes(current_monster, true, &reachable_heroes); // reachable in 1 turn?

        if (reachable_heroes.size() != 0)
        {
            // (b.1) hero can be reached
            // compute weakest heros
            vector<Hero*> weakest_reachable_heroes;
            level->computeWeakestHeroes(reachable_heroes, &weakest_reachable_heroes);
            if (!weakest_reachable_heroes.empty())
            {
                // randomly choose one of the weakest reachable heroes
                Hero* hero_target = getRandomElementByValue<Hero*>(weakest_reachable_heroes);
                // move to hero_target
                vector<NodeID> path_to_hero_target;
                HeroQuestLevelWindow::_hero_quest->getPlayground()->computeShortestPathNextTo(current_monster,
                        hero_target, false, true, false, level->getCurrentlyActingMonsterActionStates()->_move_points,
                        &path_to_hero_target);
                level->moveCurrentMonster(path_to_hero_target);
            }
        }
        else
        {
            // (b.2) hero cannot be reached in 1 turn
            // get nearest heroes
            vector<Hero*> nearest_heroes;
            HeroQuestLevelWindow::_hero_quest->getPlayground()->computeNearestHeroes(current_monster, &nearest_heroes);
            // get the weakest among these
            vector<Hero*> weakest_nearest_heroes;
            level->computeWeakestHeroes(nearest_heroes, &weakest_nearest_heroes);
            if (!weakest_nearest_heroes.empty())
            {
                // randomly choose one
                Hero* hero_target = getRandomElementByValue<Hero*>(weakest_nearest_heroes);
                // move to hero_target
                vector<NodeID> path_to_hero_target;
                HeroQuestLevelWindow::_hero_quest->getPlayground()->computeShortestPathNextTo(current_monster,
                        hero_target, false, true, false, level->getCurrentlyActingMonsterActionStates()->_move_points,
                        &path_to_hero_target);
                level->moveCurrentMonster(path_to_hero_target);
            }
        }
    }

    // phase 3: attack: if (!has_attacked && standing next to a hero), then
    //          (a) get one of the weakest heroes next to me
    //          (b) attack that hero
    if (level->getCurrentlyActingMonsterActionStates()->_num_attacks_left > 0)
    {
        vector<Hero*> adjacent_heroes;
        HeroQuestLevelWindow::_hero_quest->getPlayground()->computeAdjacentHeroes(current_monster, &adjacent_heroes);
        if (adjacent_heroes.size() != 0)
        {
            // (3.a)
            // get the weakest of the adjacent heroes
            vector<Hero*> weakest_adjacent_heroes;
            level->computeWeakestHeroes(adjacent_heroes, &weakest_adjacent_heroes);
            if (!weakest_adjacent_heroes.empty())
            {
                // randomly choose one
                Hero* hero_target = getRandomElementByValue<Hero*>(weakest_adjacent_heroes);

                // (3.b)
                CreatureAttacksCreatureCommand cmd(*current_monster, *hero_target);
                cmd.run();

                if (level->getCurrentlyActingMonsterActionStates()->_num_attacks_left > 0)
                  --(level->getCurrentlyActingMonsterActionStates()->_num_attacks_left);
            }
        }
    }
}

// =================================================================

const QString OpenChestCommand::NAME = "OpenChestCommand";

OpenChestCommand::OpenChestCommand(uint room_id)
        :
        _room_id(room_id)
{
    // NIX
}

OpenChestCommand::~OpenChestCommand()
{
    // NIX
}

void OpenChestCommand::run()
{
    Chest* chest = HeroQuestLevelWindow::_hero_quest->getLevel()->getNextChestOfRoom(_room_id, Level::CHEST_CLOSED);
    if (chest == 0)
    {
        DVX(("ERROR: OpenChestCommand::run: no closed chest found in room %d!", _room_id));
        return;
    }

    Level* level = HeroQuestLevelWindow::_hero_quest->getLevel();

    map<NodeID, TreasureDescription>::iterator it_chest_pos_to_treasure_description =
            level->_chest_pos_to_treasure_description.find(
                    chest->getNodeTopLeft());
    if (it_chest_pos_to_treasure_description == level->_chest_pos_to_treasure_description.end())
    {
        DVX(
                ("ERROR: OpenChestCommand::run: no treasure description available for chest at (%d, %d)!", chest->getNodeTopLeft()._ix, chest->getNodeTopLeft()._iy));
        return;
    }
    TreasureDescription& treasure_description = it_chest_pos_to_treasure_description->second;

    HeroQuestLevelWindow::_hero_quest->playSoundOnce(SoundManager::SOUND_CHEST_OPENS);

    HeroQuestLevelWindow::_hero_quest->execDialogOpenChest(QVariant::fromValue(&treasure_description));

    chest->setOpen(true);
}

// =================================================================

const QString DrawTreasureCardCommand::NAME = "DrawTreasureCardCommand";

DrawTreasureCardCommand::DrawTreasureCardCommand()
{
    // NIX
}

DrawTreasureCardCommand::~DrawTreasureCardCommand()
{
    // NIX
}

void DrawTreasureCardCommand::run()
{
    Level* level = HeroQuestLevelWindow::_hero_quest->getLevel();

    // if the stock is empty, re-shuffle the deposition into the stock (does never happen)
    //if (level->_treasure_card_stock.empty())
    //    level->shuffleTreasureCards();
    HG_ASSERT(!level->_treasure_card_stock.empty(), "DrawTreasureCardCommand::run: The treasure card stock is empty!");

    // draw card from the stock
#if 1
    TreasureCard treasure_card = level->_treasure_card_stock.front();
#else
    // debug: draw a specific card (warning: anyway the first card is popped below!)
    list<TreasureCard>::iterator it_treasure_card = level->_treasure_card_stock.begin();
    TreasureCard& treasure_card_ref = *it_treasure_card;
    while (it_treasure_card != level->_treasure_card_stock.end() &&
            (it_treasure_card->getTreasureDescription().getTreasureImageID() != Playground::TREASURE_CARD_POWER_POTION) &&
            (it_treasure_card->getTreasureDescription().getTreasureImageID() != Playground::TREASURE_CARD_IMMUNIZATION_POTION))
    {
        cout << "id: " << qPrintable(it_treasure_card->getTreasureDescription().getID()) << endl;
        ++it_treasure_card;
        if (it_treasure_card != level->_treasure_card_stock.end())
        {
            treasure_card_ref = *it_treasure_card;
        }
    }
    TreasureCard treasure_card = treasure_card_ref;
    level->_treasure_card_stock.erase(it_treasure_card);
#endif

    if (treasure_card.getCharacteristic() == TreasureCard::GOOD_CHARACTERISTIC)
        HeroQuestLevelWindow::_hero_quest->playSoundOnce(SoundManager::DRAW_TREASURE_CARD_GOOD);
    else
        HeroQuestLevelWindow::_hero_quest->playSoundOnce(SoundManager::DRAW_TREASURE_CARD_BAD);

    HeroQuestLevelWindow::_hero_quest->execDialogTreasureCard(QVariant::fromValue(&treasure_card));

    // execute immediate actions
    if (treasure_card.getTreasureDescription().containsImmediateActions())
    {
        treasure_card.executeImmediateActions();
    }

    // remove the card from the stock
    level->_treasure_card_stock.pop_front();

    // store the card in the hero's inventory (e.g. potion), depose it (e.g. gold), or shuffle it back into the stock
    if (treasure_card.getTreasureDescription().containsInventoryItem())
    {
        level->getCurrentlyActingHero()->getInventory().addTreasureCard(treasure_card);
        HeroQuestLevelWindow::_hero_quest->updateHeroStatisticPaneOfCurrentHero();
    }
    else if (treasure_card.getCharacteristic() == TreasureCard::BAD_CHARACTERISTIC)
    {
        level->_treasure_card_stock.push_back(treasure_card);
        level->shuffleTreasureCardStock();
    }
    else
    {
        level->_treasure_card_deposition.push_back(treasure_card);
    }
}

// =================================================================

const QString DelayCommand::NAME = "DelayCommand";

DelayCommand::DelayCommand(uint ms)
        :
        _ms(ms)
{
    // NIX
}

DelayCommand::~DelayCommand()
{
    // NIX
}

void DelayCommand::run()
{
    QThread::usleep(1000 * _ms);
}

// =================================================================
