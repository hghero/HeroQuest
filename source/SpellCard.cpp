#include "SpellCard.h"

#include <vector>

#include "StreamUtils.h"
#include "HeroQuestLevelWindow.h"
#include "Playground.h"
#include "Level.h"
#include "SoundManager.h"
#include "QuestBoard.h"
#include "SpellCardStorage.h"
#include "Debug.h"

using namespace std;

SpellCard::SpellCard()
: InventoryItem(),
  _spell_id(UNKNOWN)
{
    // NIX
}

SpellCard::SpellCard(const SpellCard::SpellID& spell_id)
: InventoryItem(),
  _spell_id(spell_id)
{
    // NIX
}

SpellCard::~SpellCard()
{
	// NIX
}

bool SpellCard::isSpellCard() const
{
    return true;
}

/*!
 * Assumes that spell_ids are unique!
 */
bool SpellCard::operator<(const SpellCard& other) const
{
    if (_spell_id != UNKNOWN && other._spell_id != UNKNOWN)
        return _spell_id < other._spell_id;

    // UNKNOWN is greater
    if (_spell_id == UNKNOWN && other._spell_id != UNKNOWN)
        return false;
    if (_spell_id != UNKNOWN && other._spell_id == UNKNOWN)
        return true;

    // both UNKNOWN: use pointers
    return this < &other;
}

SpellCard& SpellCard::operator=(const SpellCard& other)
{
    _spell_id = other._spell_id;
    return *this;
}

SpellCard::SpellFamily SpellCard::getSpellFamily() const
{
    uint spell_id_uint = uint(_spell_id);

    if (spell_id_uint >= 0 && spell_id_uint < 3)
        return AIR;

    if (spell_id_uint >= 3 && spell_id_uint < 6)
        return WATER;

    if (spell_id_uint >= 6 && spell_id_uint < 9)
        return EARTH;

    if (spell_id_uint >= 9 && spell_id_uint < 12)
        return FIRE;

    DVX(("SpellCard::getSpellFamily: Undefined spell ID %d!", spell_id_uint));
    return AIR;
}

void SpellCard::setSpellID(const SpellCard::SpellID& spell_id)
{
    _spell_id = spell_id;
}

SpellCard::SpellID SpellCard::getSpellID() const
{
    return _spell_id;
}

QString SpellCard::getTitle() const
{
    QString title("UNDEFINED");
    switch (_spell_id)
    {
        // === air spells ===
        case GENIE:
            title = "Genie in a Bottle";
            break;
        case TAILWIND:
            title = "Tailwind";
            break;
        case STORM:
            title = "Thunderstorm";
            break;

            // === water spells ===
        case HEALING_POTION:
            title = "Spa Waters";
            break;
        case FOG:
            title = "Shroud of Mist";
            break;
        case MORPHEUS:
            title = "Morpheus' Arms";
            break;

            // === earth spells ===
        case GRANITE_SKIN:
            title = "Granite Skin";
            break;
        case THROUGH_WALL:
            title = "Through-the-Wall";
            break;
        case HEALING_MAGIC:
            title = "Healing Magic";
            break;

            // === fire spells ===
        case BRAVENESS:
            title = "Braveness";
            break;
        case BURNING_RAGE:
            title = "Burning Rage";
            break;
        case FIREBALL:
            title = "Fireball";
            break;

        default:
            DVX(("SpellCard::getTitle: Undefined spell id!"))
            ;
            break;
    }
    return title;
}

QString SpellCard::getDescription() const
{
    QString description("UNDEFINED");
    switch (_spell_id)
    {
        // === air spells ===
        case GENIE:
            description = "This spell summons a genie, who EITHER opens a visible doors of your choice and shows "
                    "you the contents of the adjacent room, OR attacks a creature of your choice with 5 attack dice.";
            break;
        case TAILWIND:
            description =
                    "This spell can be cast on any hero. That hero may then use twice as many movement dice as usual.";
            break;
        case STORM:
            description = "This spell can be cast on any hero or monster. That creature will be enclosed by a local "
                    "thunderstom, so that it can't execute its next turn.";
            break;

            // === water spells ===
        case HEALING_POTION:
            description =
                    "This spell can be cast on any hero. It restocks up to 4 life points that the hero has lost so far.";
            break;
        case FOG:
            description =
                    "This spell can be cast on any hero. That hero may, during his next move, cross fields which are occupied by a monster.";
            break;
        case MORPHEUS:
            description =
                    "This spell makes a hero or monster fall asleep. The target creature can throw 1 attack die per intelligence point to defeat the spell: "
                            "only if it throws a shield, it stays awake. A sleeping creature needs to throw a 6 before it can continue playing.";
            break;

            // === earth spells ===
        case GRANITE_SKIN:
            description = "This spell can be cast on any hero. That hero may use 2 extra dice for his next defence.";
            break;
        case THROUGH_WALL:
            description =
                    "This spell can be cast on any hero. That hero may, during his next turn, move through walls.";
            break;
        case HEALING_MAGIC:
            description =
                    "This spell can be cast on any hero. It restocks up to 4 life points that the hero has lost so far.";
            break;

            // === fire spells ===
        case BRAVENESS:
            description =
                    "This spell can be cast on any hero. That hero may attack his next opponent with 2 extra dice.";
            break;
        case BURNING_RAGE:
            description = "This spell can be cast on any hero or monster. The spell reduces the creature's life points "
                    "by 1, if the creature does not throw a shield.";
            break;
        case FIREBALL:
            description = "This spell can be cast on any hero or monster. The spell attacks creature with 2 skulls. "
                    "The creature may throw 2 defend dice.";
            break;

        default:
            DVX(("SpellCard::getDescription: Undefined spell id!"));
            break;
    }
    return description;
}

QPixmap* SpellCard::getBackImage() const
{
    return SpellCardStorage::instance->getSpellCardBackImage(getSpellFamily());
}

QPixmap* SpellCard::getImage() const
{
    return SpellCardStorage::instance->getSpellCardImage(getSpellID());
}

bool SpellCard::save(std::ostream& stream) const
{
    Parent::save(stream);

    StreamUtils::writeUInt(stream, uint(_spell_id));

    return !stream.fail();
}

bool SpellCard::load(std::istream& stream)
{
    Parent::load(stream);

    uint spell_id_uint = 0;
    StreamUtils::readUInt(stream, &spell_id_uint);
    _spell_id = SpellCard::SpellID(spell_id_uint);

    return !stream.fail();
}

bool SpellCard::execute() const
{
    bool spell_card_used = false;

    switch (_spell_id)
    {
        // === air spells ===
        case GENIE:
        {
            Creature* creature = 0;
            Door* door = 0;
            HeroQuestLevelWindow::_hero_quest->userSelectsCreatureOrDoor(&creature, &door);
            if (creature == 0 && door == 0)
                return false;

            if (creature != 0)
            {
                DVX(("chosen creature for GENIE: %s", qPrintable(creature->getName())));
                // attack target creature with 5 attack dice
                if (HeroQuestLevelWindow::_hero_quest->getLevel()->genieAttacksCreature(
                        HeroQuestLevelWindow::_hero_quest->getLevel()->getCurrentlyActingCreature(), creature))
                {
                    spell_card_used = true;
                    HeroQuestLevelWindow::_hero_quest->playSoundOnce(SoundManager::CAST_SPELL);
                }
            }
            if (door != 0)
            {
                DVX(("chosen door for GENIE: %s", qPrintable(Door::getName(door))));
                // open the door...
                HeroQuestLevelWindow::_hero_quest->getLevel()->openDoor(door);
                // ...and show the contents of the adjacent room
                HeroQuestLevelWindow::_hero_quest->getPlayground()->setRoomVisible(door);

                spell_card_used = true;
                HeroQuestLevelWindow::_hero_quest->playSoundOnce(SoundManager::CAST_SPELL);
            }
        }
            break;
        case TAILWIND:
        {
            Hero* hero = HeroQuestLevelWindow::_hero_quest->userSelectsHero();
            if (hero == 0)
                return false;
            DVX(("chosen hero for TAILWIND: %s", qPrintable(hero->getName())));

            spell_card_used = true;
            hero->setNumDiceMoveExtra(2);

            HeroQuestLevelWindow::_hero_quest->updateHeroStatisticPane(hero);

            HeroQuestLevelWindow::_hero_quest->playSoundOnce(SoundManager::CAST_SPELL);
        }
            break;
        case STORM:
        {
            Creature* creature = HeroQuestLevelWindow::_hero_quest->userSelectsCreature();
            if (creature == 0)
                return false;
            DVX(("chosen creature for STORM: %s", qPrintable(creature->getName())));

            spell_card_used = true;
            creature->increaseSkipRounds(1);

            if (creature->isHero())
                HeroQuestLevelWindow::_hero_quest->updateHeroStatisticPane(dynamic_cast<Hero*>(creature));

            HeroQuestLevelWindow::_hero_quest->playSoundOnce(SoundManager::CAST_SPELL);
        }
            break;

            // === water spells ===
        case HEALING_POTION:
        {
            Hero* hero = HeroQuestLevelWindow::_hero_quest->userSelectsHero();
            if (hero == 0)
                return false;
            DVX(("chosen hero for HEALING_POTION: %s", qPrintable(hero->getName())));

            hero->addLifePoints(4);
            spell_card_used = true;

            HeroQuestLevelWindow::_hero_quest->updateHeroStatisticPane(hero);

            HeroQuestLevelWindow::_hero_quest->playSoundOnce(SoundManager::CAST_SPELL);
        }
            break;
        case FOG:
        {
            Hero* hero = HeroQuestLevelWindow::_hero_quest->userSelectsHero();
            if (hero == 0)
                return false;
            DVX(("chosen hero for FOG: %s", qPrintable(hero->getName())));

            HeroQuestLevelWindow::_hero_quest->getLevel()->getHeroActionStates(hero)->_may_cross_monsters_during_movement =
                    true;

            HeroQuestLevelWindow::_hero_quest->playSoundOnce(SoundManager::CAST_SPELL);
        }
            break;
        case MORPHEUS:
        {
            Creature* creature = HeroQuestLevelWindow::_hero_quest->userSelectsCreature();
            if (creature == 0)
                return false;
            DVX(("chosen creature for MORPHEUS: %s", qPrintable(creature->getName())));

            spell_card_used = true;

            HeroQuestLevelWindow::_hero_quest->playSoundOnce(SoundManager::CAST_SPELL);
            HeroQuestLevelWindow::_hero_quest->delay(Level::ATTACK_DELAY);

            // The attacked creature throws 1 defend die to defend the spell.
            vector<DiceRollPane::AttackDiceResult> dice_results;
            HeroQuestLevelWindow::_hero_quest->getDiceRollPane()->throwDefendDice(*creature, 1, &dice_results);
            HeroQuestLevelWindow::_hero_quest->getDiceRollPane()->update();
            HeroQuestLevelWindow::_hero_quest->delay(Level::ATTACK_DELAY);
            
            if ((creature->isHero() && (dice_results[0] != DiceRollPane::ATTACK_DICE_RESULT_HUMAN_SHIELD))
                    || (creature->isMonster() && (dice_results[0] != DiceRollPane::ATTACK_DICE_RESULT_MONSTER_SHIELD)))
            {
                // creature falls asleep
                Level::ActionStates* creature_action_states =
                        HeroQuestLevelWindow::_hero_quest->getLevel()->getCreatureActionStates(creature);
                creature_action_states->_sleeping = true;
                if (creature->isHero())
                    HeroQuestLevelWindow::_hero_quest->updateHeroStatisticPane(dynamic_cast<Hero*>(creature));
                HeroQuestLevelWindow::_hero_quest->playSoundOnce(SoundManager::CAST_SPELL);
            }
            else
            {
                HeroQuestLevelWindow::_hero_quest->playSoundOnce(SoundManager::SOUND_ATTACK_DEFENDED);
            }
        }
            break;

            // === earth spells ===
        case GRANITE_SKIN:
        {
            Hero* hero = HeroQuestLevelWindow::_hero_quest->userSelectsHero();
            if (hero == 0)
                return false;
            DVX(("chosen hero for GRANITE_SKIN: %s", qPrintable(hero->getName())));

            hero->setNumDiceDefendExtra(2);
            spell_card_used = true;

            HeroQuestLevelWindow::_hero_quest->updateHeroStatisticPane(hero);

            HeroQuestLevelWindow::_hero_quest->playSoundOnce(SoundManager::CAST_SPELL);
        }
            break;
        case THROUGH_WALL:
        {
            Hero* hero = HeroQuestLevelWindow::_hero_quest->userSelectsHero();
            if (hero == 0)
                return false;
            DVX(("chosen hero for THROUGH_WALL: %s", qPrintable(hero->getName())));
            hero->setCanMoveThroughWalls(true);
            spell_card_used = true;

            HeroQuestLevelWindow::_hero_quest->updateHeroStatisticPane(hero);

            HeroQuestLevelWindow::_hero_quest->playSoundOnce(SoundManager::CAST_SPELL);
        }
            break;
        case HEALING_MAGIC:
        {
            Hero* hero = HeroQuestLevelWindow::_hero_quest->userSelectsHero();
            if (hero == 0)
                return false;
            DVX(("chosen hero for HEALING_MAGIC: %s", qPrintable(hero->getName())));

            hero->addLifePoints(4);
            spell_card_used = true;

            HeroQuestLevelWindow::_hero_quest->updateHeroStatisticPane(hero);
            HeroQuestLevelWindow::_hero_quest->getPlayground()->getQuestBoard()->computeReachableArea();

            HeroQuestLevelWindow::_hero_quest->playSoundOnce(SoundManager::CAST_SPELL);
        }
            break;

            // === fire spells ===
        case BRAVENESS:
        {
            Hero* hero = HeroQuestLevelWindow::_hero_quest->userSelectsHero();
            if (hero == 0)
                return false;
            DVX(("chosen hero for BRAVENESS: %s", qPrintable(hero->getName())));

            spell_card_used = true;
            hero->setNumDiceAttackExtra(hero->getNumDiceAttackExtra() + 2);

            HeroQuestLevelWindow::_hero_quest->updateHeroStatisticPane(hero);

            HeroQuestLevelWindow::_hero_quest->playSoundOnce(SoundManager::CAST_SPELL);
        }
            break;
        case BURNING_RAGE:
        {
            Creature* creature = HeroQuestLevelWindow::_hero_quest->userSelectsCreature();
            if (creature == 0)
                return false;
            DVX(("chosen creature for BURNING_RAGE: %s", qPrintable(creature->getName())));

            HeroQuestLevelWindow::_hero_quest->playSoundOnce(SoundManager::CAST_SPELL);
            if (!HeroQuestLevelWindow::_hero_quest->getLevel()->attackCreatureWithSkulls(
                    HeroQuestLevelWindow::_hero_quest->getLevel()->getCurrentlyActingCreature(), 1, creature))
                return false;

            spell_card_used = true;

            if (HeroQuestLevelWindow::_hero_quest->getLevel()->isCreatureAlive(creature) && creature->isHero())
                HeroQuestLevelWindow::_hero_quest->updateHeroStatisticPane(dynamic_cast<Hero*>(creature));
        }
            break;
        case FIREBALL:
        {
            Creature* creature = HeroQuestLevelWindow::_hero_quest->userSelectsCreature();
            if (creature == 0)
                return false;
            DVX(("chosen creature for FIREBALL: %s", qPrintable(creature->getName())));

            HeroQuestLevelWindow::_hero_quest->playSoundOnce(SoundManager::CAST_SPELL);
            if (!HeroQuestLevelWindow::_hero_quest->getLevel()->attackCreatureWithSkulls(
                    HeroQuestLevelWindow::_hero_quest->getLevel()->getCurrentlyActingCreature(), 2, creature, 2))
                return false;

            spell_card_used = true;

            if (HeroQuestLevelWindow::_hero_quest->getLevel()->isCreatureAlive(creature) && creature->isHero())
                HeroQuestLevelWindow::_hero_quest->updateHeroStatisticPane(dynamic_cast<Hero*>(creature));
        }
            break;

        default:
        {
            DVX(("SpellCard::execute: Undefined spell id!"))
            ;
        }
            break;
    }

    return spell_card_used;
}
