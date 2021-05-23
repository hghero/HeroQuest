#include "TreasureCard.h"

#include <vector>

#include "HeroQuestLevelWindow.h"
#include "SoundManager.h"
#include "Playground.h"
#include "Math.h"
#include "Debug.h"
#include "Level.h"
#include "TreasureDescription.h"
#include "TreasureDataTypes.h"
#include "Hero.h"

using namespace std;

TreasureCard::TreasureCard()
: InventoryItem(),
  _treasure_description()
{
    // NIX
}

TreasureCard::TreasureCard(const TreasureDescription& treasure_description)
: InventoryItem(),
  _treasure_description(treasure_description)
{
	// NIX
}

TreasureCard::~TreasureCard()
{
	// NIX
}

const TreasureDescription& TreasureCard::getTreasureDescription() const
{
	return _treasure_description;
}

TreasureCard& TreasureCard::operator=(const TreasureCard& other)
{
	_treasure_description = other._treasure_description;
	return *this;
}

bool TreasureCard::operator<(const TreasureCard& other) const
{
	return _treasure_description < other._treasure_description;
}

bool TreasureCard::isTreasureCard() const
{
    return true;
}

/*!
 * Some treasure cards cannot be applied from the inventory. These return false here.
 */
bool TreasureCard::isApplyableFromInventory() const
{
    //cout << "id: " << qPrintable(_treasure_card_description.getID()) << endl;
    return _treasure_description.getTreasureImageID() != TreasureDataTypes::TREASURE_CARD_RESISTANCE_POTION;
}

TreasureCard::Characteristic TreasureCard::getCharacteristic() const
{
    TreasureCard::Characteristic characteristic = BAD_CHARACTERISTIC;

    const TreasureDataTypes::TreasureImageID& image_id = _treasure_description.getTreasureImageID();
    switch (image_id)
    {
        case TreasureDataTypes::TREASURE_CARD_UNKNOWN:
        case TreasureDataTypes::TREASURE_CARD_NOTHING:
        case TreasureDataTypes::TREASURE_CARD_GOLD:
        case TreasureDataTypes::TREASURE_CARD_JEWELS:
        case TreasureDataTypes::TREASURE_CARD_GEMSTONE:
        case TreasureDataTypes::TREASURE_CARD_GOLD_TREASURE:
        case TreasureDataTypes::TREASURE_CARD_BRAVENESS_POTION:
        case TreasureDataTypes::TREASURE_CARD_HEALING_POTION:
        case TreasureDataTypes::TREASURE_CARD_HOLY_WATER:
        case TreasureDataTypes::TREASURE_CARD_IMMUNIZATION_POTION:
        case TreasureDataTypes::TREASURE_CARD_MAGIC_POTION:
        case TreasureDataTypes::TREASURE_CARD_PHILOSOPHERS_STONE:
        case TreasureDataTypes::TREASURE_CARD_POWER_POTION:
        case TreasureDataTypes::TREASURE_CARD_RESISTANCE_POTION:
        case TreasureDataTypes::TREASURE_CARD_SPRINT_POTION:
            characteristic = GOOD_CHARACTERISTIC;
            break;

        case TreasureDataTypes::TREASURE_CARD_POISON:
        case TreasureDataTypes::TREASURE_CARD_TRAP_WALL:
        case TreasureDataTypes::TREASURE_CARD_TRAP_PIT:
        case TreasureDataTypes::TREASURE_CARD_ROAMING_MONSTER:
            characteristic = BAD_CHARACTERISTIC;
            break;
    }

    return characteristic;
}

bool TreasureCard::save(std::ostream& stream) const
{
    Parent::save(stream);

    _treasure_description.save(stream);

    return !stream.fail();
}

bool TreasureCard::load(std::istream& stream)
{
    Parent::load(stream);

    _treasure_description.load(stream);

    return !stream.fail();
}

void TreasureCard::updateTreasureImages()
{
    _treasure_description.updateTreasureImage();
}

void TreasureCard::executeImmediateActions() const
{
    _treasure_description.executeImmediateActions();
}

/*!
 * Executes all inventory actions which are registered for this TreasureCard.
 *
 * \user Hero who uses this treasure card. If 0, the currently acting hero uses the card.
 * \return True, if the treasure card actions have actually been applied.
 */
bool TreasureCard::executeInventoryActions(Hero* user) const
{
	Level* level = HeroQuestLevelWindow::_hero_quest->getLevel();

    if (user == 0)
    {
        user = level->getCurrentlyActingHero();
    }
    const NodeID* user_node_id = HeroQuestLevelWindow::_hero_quest->getPlayground()->getCreaturePos(user);
    if (user_node_id == 0)
    {
        DVX(("Node id of hero %s unknown!", qPrintable(user->getName())));
        return false;
    }

	vector<QString> items;
	_treasure_description.getInventoryItems(&items);

	for (vector<QString>::const_iterator it = items.begin(); it != items.end(); ++it)
	{
		const QString& item = *it;

		if (item.compare("PHILOSOPHERS_STONE") == 0)
		{
		    DVX(("NYI: PHILOSOPHERS_STONE (can do this only after having implemented equipment cards)"));
		}
		else if (item.compare("IMMUNIZATION_POTION") == 0)
		{
		    // next defence: +2 combat dice
		    user->setNumDiceDefendExtra(2);
		}
		else if (item.compare("POWER_POTION") == 0)
		{
		    // next attack: +2 combat dice
            user->setNumDiceAttackExtra(user->getNumDiceAttackExtra() + 2);
		}
		else if (item.compare("RESISTANCE_POTION") == 0)
		{
		    // This guy is called via an other, special way: when a spell is cast on the hero, the hero can directly
		    // choose if he wants to apply this card and thus render the spell ineffective.
		    // => Nothing to do here!
		}
		else if (item.compare("BRAVENESS_POTION") == 0)
		{
			// the current hero may attack a second time
			++(level->getHeroActionStates(user)->_num_attacks_left);

			// update the attack button's state
		    HeroQuestLevelWindow::_hero_quest->updateButtons();
		}
		else if (item.compare("HEALING_POTION") == 0)
		{
			user->addLifePoints(4);
		}
		else if (item.compare("HOLY_WATER") == 0)
		{
		    // Using holy water is only possible instead of attacking, i.e. if the hero already has attacked,
		    // he cannot use holy water anymore during this round.
		    if (level->getHeroActionStates(user)->_num_attacks_left == 0)
		        return false;

		    set<NodeID> viewable_nodes;
		    HeroQuestLevelWindow::_hero_quest->getPlayground()->computeViewableNodes(*user_node_id, false, &viewable_nodes);
		    bool undead_creature_killed = false;
		    for (set<NodeID>::iterator it = viewable_nodes.begin(); it != viewable_nodes.end(); ++it)
		    {
		        Creature* creature = HeroQuestLevelWindow::_hero_quest->getPlayground()->getCreature(*it);
		        if (creature != 0 && creature->isMonster())
		        {
		            Skeleton* skeleton = dynamic_cast<Skeleton*>(creature);
		            Zombie* zombie = dynamic_cast<Zombie*>(creature);
		            Mummy* mummy = dynamic_cast<Mummy*>(creature);

		            if (skeleton != 0 || zombie != 0 || mummy != 0)
		            {
		                creature->setLifePoints(0);

		                // remove creature from the playground
		                HeroQuestLevelWindow::_hero_quest->getLevel()->removeCreature(creature);

		                undead_creature_killed = true;
		            }
		        }

		        if (undead_creature_killed)
		            HeroQuestLevelWindow::_hero_quest->playSoundOnce(SoundManager::SOUND_MONSTER_DIES);
		    }

		    // Holy water is used "instead of attacking", i.e. the hero may attack anymore in this round.
		    --(level->getHeroActionStates(user)->_num_attacks_left);
            DVX(("decrease num_attacks_left (holy water)"));
		}
		else if (item.compare("SPRINT_POTION") == 0)
		{
			// the current hero rolls twice as much dice for his next movement dice roll
			// (is reset automatically after the next roll of this Hero)
			int current_num_dice = user->getNumDiceMove();
			user->setNumDiceMoveExtra(current_num_dice);
		}
		else if (item.compare("MAGIC_POTION") == 0)
		{
			if (user->canCastSpells())
			{
				// the hero may cast two spells instead of only one
				++(level->getHeroActionStates(user)->_num_spell_casts_left);
				HeroQuestLevelWindow::_hero_quest->updateButtons();
			}
		}
		else
		{
			DVX(("TODO: implement item actions: %s", item.toUtf8().constData()));
		}

		HeroQuestLevelWindow::_hero_quest->playSoundOnce(SoundManager::SOUND_DRAW_TREASURE_CARD_GOOD);
		HeroQuestLevelWindow::_hero_quest->updateHeroStatisticPaneOfCurrentHero();
	}

	return true;
}
