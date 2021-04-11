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

/*!
 * Extracts the substring between "<" and ">".
 *
 * \param action Sth. like "GOLD <MOVEMENT_DICE * 10>" or "GOLD <100>".
 * \param split_behavior Specifies how to handle empty string parts.
 * \return The string between the braces.
 */
QString TreasureCard::getEmbracedValue(const QString& action, QString::SplitBehavior split_behavior)
{
    // "GOLD <40>" => ["GOLD ", "40>"]
    QStringList str1 = action.split("<", split_behavior);
    if (str1.size() < 2)
        return "";
    QStringList str2 = str1[1].split(">", split_behavior);
    if (str2.size() == 0)
        return "";
    return str2[0];
}

/*!
 * Evaluation of simple expressions (used for gold values of treasure cards).
 *
 * \param expr A (simple) expression.
 * \param result_value Will be assigned the numerical value of the expression expr.
 * \return True if a result_value has been assigned.
 */
bool TreasureCard::getValueOfGoldExpression(const QString& expr, uint* result_value)
{
    QStringList tokens = expr.split(" ", QString::SkipEmptyParts);

    if (tokens.size() >= 3)
    {
        // a) >= 3 tokens => evaluate first 3 tokens; then combine with remaining sub-expressions

        uint result0 = 0;
        uint result2 = 0;

        if (!getValueOfGoldExpression(tokens[0], &result0))
            return false;
        if (!getValueOfGoldExpression(tokens[2], &result2))
            return false;

        bool operation_valid = false;
        uint combined_result = 0;

        if (tokens[1] == "+")
        {
            combined_result = result0 + result2;
            operation_valid = true;
        }
        else if (tokens[1] == "-")
        {
            combined_result = result0 - result2;
            operation_valid = true;
        }
        else if (tokens[1] == "*")
        {
            combined_result = result0 * result2;
            operation_valid = true;
        }
        else if (tokens[1] == "/")
        {
            combined_result = uint(result0 / double(result2) + 0.5);
            operation_valid = true;
        }

        if (!operation_valid)
        {
            DVX(("Invalid operation: %s", qPrintable(tokens[1])));
            return false;
        }

        if (tokens.size() == 3)
        {
            *result_value = combined_result;
            return true;
        }
        else
        {
            // further sub-expressions
            QString remaining_sub_expressions;
            for (uint i = 3; i < uint(tokens.size()); ++i)
            {
                remaining_sub_expressions = remaining_sub_expressions + " " + tokens[i];
            }
            remaining_sub_expressions = QString("%1 ").arg(combined_result) + remaining_sub_expressions;

            return getValueOfGoldExpression(remaining_sub_expressions, result_value);
        }
    }
    else if (tokens.size() == 1)
    {
        // b) = 1 token; get constant or variable number

        // constant number?
        bool ok = false;
        uint number = tokens[0].toUInt(&ok);
        if (ok)
        {
            *result_value = number;
            return true;
        }

        // supported variable?

        if (tokens[0] == "MOVEMENT_DICE")
        {
            // get random number between 1 and 6
            *result_value = randomNumber(6) + 1;
            return true;
        }

        DVX(("Variable \"%s\" not supported in expression \"%s\".", qPrintable(tokens[0]), qPrintable(expr)));
    }
    else
    {
        DVX(("Invalid number of tokens (%d) in expression %s", tokens.size(), qPrintable(expr)));
        return false;
    }

    return false;
}

/*!
 * Interprets expressions like "SKIP = 1" by setting command := "SKIP" and result_value := 1.
 *
 * \param expr The source expression.
 * \param command The command in the expression (first token before "=").
 * \param result_value Will be set to the value behind the "=".
 */
bool TreasureCard::interpretRoundExpression(const QString& expr, QString* command, uint* result_value)
{
    QStringList str1 = expr.split("=", QString::SkipEmptyParts);
    if (str1.size() != 3 || str1[1] != "=")
    {
        DVX(("Error interpreting round expression \"%s\"", qPrintable(expr)));
        return false;
    }

    *command = str1[0];

    bool ok = false;
    *result_value = str1[2].toUInt(&ok);

    return ok;
}

void TreasureCard::executeImmediateActions() const
{
	vector<QString> actions;
	_treasure_description.getImmediateActions(&actions);

	Hero* current_hero = HeroQuestLevelWindow::_hero_quest->getLevel()->getCurrentlyActingHero();
	if (current_hero == 0)
	    return;

	for (vector<QString>::const_iterator it = actions.begin(); it != actions.end(); ++it)
	{
		const QString& action = *it;

		if (action.compare("ROAMING_MONSTER") == 0)
			HeroQuestLevelWindow::_hero_quest->getLevel()->roamingMonsterAttacksCurrentHero();
		else if (action.startsWith("GOLD"))
		{
		    // extract the substring between "<" and ">"
		    QString gold_value_str = getEmbracedValue(action);
		    if (gold_value_str == "")
		    {
		        cout << "Invalid action \"" << qPrintable(action) << "\"!" << endl;
		        continue;
		    }

		    // interpret the expression
		    uint gold_value = 0;
		    bool ok = getValueOfGoldExpression(gold_value_str, &gold_value);
		    if (!ok)
		        continue;

		    // add gold to inventory
		    uint current_gold = current_hero->getGold();
		    current_hero->setGold(current_gold + gold_value);

		    HeroQuestLevelWindow::_hero_quest->updateHeroStatisticPaneOfCurrentHero();
		}
		else if (action.startsWith("ROUND"))
		{
		    //"ROUND <SKIP = 1>"

		    // extract the substring between "<" and ">"
		    QString expr = getEmbracedValue(action);
		    if (expr == "")
		    {
		        cout << "Invalid action \"" << qPrintable(action) << "\"!" << endl;
		        continue;
		    }

		    // interpret expression of kind "a = 34"

		    // get command ("SKIP") and value (1)
		    QString command = "";
		    uint value = 0;

		    bool ok = interpretRoundExpression(expr, &command, &value);
		    if (!ok)
		        continue;

		    // add skip info to hero
		    if (command == "SKIP")
		    {
		        if (value > 0)
		        {
		            current_hero->increaseSkipRounds(value);

		            // also skip the remaining part of the current turn
		            HeroQuestLevelWindow::_hero_quest->getLevel()->getCurrentlyActingHeroActionStates()->set(Level::HeroActionStates::ALL_DONE);
		            // TODO: this doesn't seem to work!
		        }
		    }
		    else
		    {
		        DVX(("Unknown ROUND command \"%s\" with value \"%d\".", qPrintable(command), value));
		        continue;
		    }
		}
		else if (action.startsWith("LIFE_POINTS"))
		{
		    // LIFE_POINTS <-1>
		    // LIFE_POINTS <COMBAT_DICE != SKULL ? -1 : 0>

		    // extract the substring between "<" and ">"
		    QString expr = getEmbracedValue(action);
		    if (expr == "")
		    {
		        cout << "Invalid action \"" << qPrintable(action) << "\"!" << endl;
		        continue;
		    }

		    // simple number?
		    bool ok = false;
		    int diff_life_points = expr.toInt(&ok);
		    if (ok)
		    {
		        current_hero->addLifePoints(diff_life_points);
		        if (diff_life_points != 0)
		            HeroQuestLevelWindow::_hero_quest->updateHeroStatisticPaneOfCurrentHero();

		        if (current_hero->getLifePoints() > 0 && diff_life_points == -1)
		            HeroQuestLevelWindow::_hero_quest->playSoundOnce(SoundManager::SOUND_ATTACK_HERO_SMALL_DECREASE_LIFE_POINTS);
		        else if (current_hero->getLifePoints() > 0 && diff_life_points < -1)
		            HeroQuestLevelWindow::_hero_quest->playSoundOnce(SoundManager::SOUND_ATTACK_HERO_BIG_DECREASE_LIFE_POINTS);
		        else if (current_hero->getLifePoints() == 0)
		            HeroQuestLevelWindow::_hero_quest->playSoundOnce(SoundManager::SOUND_HERO_DIES);

		        continue;
		    }

		    // complex expr; currently supporting only exactly "COMBAT_DICE != SKULL ? -1 : 0" for ease of programming
		    if (expr != "COMBAT_DICE != SKULL ? -1 : 0")
		    {
                DVX(("Expression \"%s\" not supported.", qPrintable(expr)));
                continue;
		    }

		    // handle special expression "COMBAT_DICE != SKULL ? -1 : 0":
		    // 1-3 (SKULL): 0
		    // 4-6 (!= SKULL): -1
		    int number = randomNumber(6) + 1;
		    if (number >= 4 && number <= 6)
		    {
		        current_hero->addLifePoints(-1);

		        HeroQuestLevelWindow::_hero_quest->updateHeroStatisticPaneOfCurrentHero();

		        if (current_hero->getLifePoints() > 0)
		            HeroQuestLevelWindow::_hero_quest->playSoundOnce(SoundManager::SOUND_ATTACK_HERO_SMALL_DECREASE_LIFE_POINTS);
		        else
		            HeroQuestLevelWindow::_hero_quest->playSoundOnce(SoundManager::SOUND_HERO_DIES);
		        continue;
		    }
		}
		else
		{
			DVX(("Immediate action not implemented: %s", action.toUtf8().constData()));
			continue;
		}
	}
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

		HeroQuestLevelWindow::_hero_quest->playSoundOnce(SoundManager::DRAW_TREASURE_CARD_GOOD);
		HeroQuestLevelWindow::_hero_quest->updateHeroStatisticPaneOfCurrentHero();
	}

	return true;
}
