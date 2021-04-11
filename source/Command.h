#ifndef COMMAND_H
#define COMMAND_H

#include <vector>

#include <QtCore/QString>
#include <QtCore/QThread>

#include "GraphBase.h"

class Hero;
class Creature;
class PitTrap;

// =================================================================

class Command : public QThread
{
    protected:
    Command();

    public:
    Command(const QString& name);
    virtual ~Command();

    bool is(const QString& name) const;
    const QString& getName() const;

    virtual void run();

    bool succeeded() const;

    protected:
    QString _name;
    bool _success;
};

// =================================================================

/*!
 * Moves the current hero along the movement_path; step-by-step over non-occupied
 * fields, simply passing occupied fields.
 */
class MoveHeroMovementPathCommand : public Command
{
    public:
    MoveHeroMovementPathCommand(Hero& hero, const std::vector<NodeID>& movement_path);
    virtual ~MoveHeroMovementPathCommand();

    virtual void run();

    static const QString NAME;

    private:
    Hero& _hero;
    const std::vector<NodeID>& _movement_path;
};

// =================================================================

/*!
 * Performs the attack of one creature versus another.
 */
class CreatureAttacksCreatureCommand : public Command
{
    public:
    CreatureAttacksCreatureCommand(Creature& attacker, Creature& defender);
    virtual ~CreatureAttacksCreatureCommand();

    virtual void run();

    static const QString NAME;

    protected:
    void throwDefendDiceIfNecessary(unsigned int num_skulls, bool use_custom_num_defend_dice, uint num_defend_dice);

    private:
    Creature& _attacker;

    protected:
    Creature& _defender;
};

// =================================================================

/*!
 * Performs the attack of a genie versus a creature.
 */
class GenieAttacksCreatureCommand : public CreatureAttacksCreatureCommand
{
    public:
    GenieAttacksCreatureCommand(Creature& spell_caster, Creature& defender);
    virtual ~GenieAttacksCreatureCommand();

    virtual void run();

    static const QString NAME;

    private:
    Creature& _spell_caster;
};

// =================================================================

/*!
 * Attacks a creature with a fixed number of skulls.
 * This does not use nor affect any Creature's extra attack dice.
 */
class AttackCreatureWithSkullsCommand : public CreatureAttacksCreatureCommand
{
    public:
    AttackCreatureWithSkullsCommand(Creature& spell_caster, uint num_skulls, Creature& defender);
    AttackCreatureWithSkullsCommand(Creature& spell_caster, uint num_skulls, Creature& defender, uint num_defend_dice);
    virtual ~AttackCreatureWithSkullsCommand();

    virtual void run();

    static const QString NAME;

    private:
    Creature& _spell_caster;
    uint _num_skulls;

    bool _use_num_defend_dice;
    uint _num_defend_dice;
};

// =================================================================

/*!
 * A hero disarms a pit trap using his toolbox equipment.
 * For disarming it, the hero rolls a movement die. If he rolls a 1, he loses 1 life point.
 */
class DisarmPitTrapUsingToolboxCommand : public Command
{
    public:
    DisarmPitTrapUsingToolboxCommand(Hero& hero, PitTrap& pit_trap);
    virtual ~DisarmPitTrapUsingToolboxCommand();

    virtual void run();

    static const QString NAME;

    private:
    Hero& _hero;
    PitTrap& _pit_trap;
};

// =================================================================

/*!
 * Performs the turn executions of all monsters during this current round.
 */
class ExecuteMonsterTurnsCommand : public Command
{
    public:
    ExecuteMonsterTurnsCommand();
    virtual ~ExecuteMonsterTurnsCommand();

    virtual void run();

    static const QString NAME;

    private:
    void executeCurrentMonsterActions();
};

// =================================================================

class OpenChestCommand : public Command
{
    public:
    OpenChestCommand(uint room_id);
    virtual ~OpenChestCommand();

    virtual void run();

    static const QString NAME;

    private:
    uint _room_id;
};

// =================================================================

class DrawTreasureCardCommand : public Command
{
    public:
    DrawTreasureCardCommand();
    virtual ~DrawTreasureCardCommand();

    virtual void run();

    static const QString NAME;
};

// =================================================================

/*!
 * Performs the attack of one creature versus another.
 */
class DelayCommand : public Command
{
    public:
    DelayCommand(uint ms);
    virtual ~DelayCommand();

    virtual void run();

    static const QString NAME;

    private:
    uint _ms;
};

#endif
