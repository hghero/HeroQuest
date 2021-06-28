#include "TreasureDescription.h"

#include <iostream>
#include <list>
#include <set>
#include <sstream>
#include <fstream>

#include "FileUtils.h"
#include "StreamUtils.h"
#include "HeroQuestLevelWindow.h"
#include "Debug.h"
#include "TreasureCardStorage.h"
#include "Level.h"
#include "SaveContext.h"
#include "LoadContext.h"

using namespace std;

const QString TreasureDescription::TREASURE_CARD_DIR = ":/treasure_cards";

TreasureDescription::TreasureDescription()
:
_id("unknown"),
_treasure_image(0),
_amount(0),
 _image_id(TreasureDataTypes::TREASURE_CARD_UNKNOWN),
_text(),
_actions()
{
	// NIX
}

TreasureDescription::TreasureDescription(const QString& filename)
:
_id("unknown"),
_treasure_image(0),
_amount(0),
 _image_id(TreasureDataTypes::TREASURE_CARD_UNKNOWN),
_text(),
_actions()
{
	readFromFile(filename);
}

TreasureDescription::~TreasureDescription()
{
	// NIX
}

void TreasureDescription::setID(const QString& id)
{
	_id = id;
}

const QString& TreasureDescription::getID() const
{
	return _id;
}

void TreasureDescription::setAmount(unsigned int amount)
{
	_amount = amount;
}

unsigned int TreasureDescription::getAmount() const
{
	return _amount;
}

void TreasureDescription::setTreasureImageID(const TreasureDataTypes::TreasureImageID& treasure_image_id)
{
	_image_id = treasure_image_id;
}

const TreasureDataTypes::TreasureImageID& TreasureDescription::getTreasureImageID() const
{
	return _image_id;
}

void TreasureDescription::setText(const QString& text)
{
	_text = text;
}

const QString& TreasureDescription::getText() const
{
	return _text;
}

void TreasureDescription::clearActions()
{
	_actions.clear();
}

void TreasureDescription::addActionString(const QString& action_str)
{
	_actions.push_back(action_str);
}

const list<QString>& TreasureDescription::getActions() const
{
	return _actions;
}

bool TreasureDescription::containsImmediateActions() const
{
	return getImmediateActions(0);
}

bool TreasureDescription::getImmediateActions(vector<QString>* actions) const
{
	return getImmediateActionsInternal(actions);
}

/*!
 * Returns true, if at least one immediate action is contained in the _actions strings.
 * If actions != 0, the immediate action ID strings are added to the actions vector (which
 * is initially cleared).
 */
bool TreasureDescription::getImmediateActionsInternal(vector<QString>* actions) const
{
	bool break_because_immediate_action_found = false;
	bool immediate_action_found = false;
	if (actions != 0)
		actions->clear();
	for (list<QString>::const_iterator it = _actions.begin(); !break_because_immediate_action_found && it != _actions.end(); ++it)
	{
		const QString& line = *it;
		if (line.contains("GOLD") ||
			line.contains("LIFE_POINTS") ||
			line.contains("ROAMING_MONSTER"))
		{
			immediate_action_found = true;
			if (actions == 0)
			{
				break_because_immediate_action_found = true;
			}
			else
			{
				actions->push_back(line);
			}
		}
	}
	return immediate_action_found;
}

/*!
 * Extracts the substring between "<" and ">".
 *
 * \param action Sth. like "GOLD <MOVEMENT_DICE * 10>" or "GOLD <100>".
 * \param split_behavior Specifies how to handle empty string parts.
 * \return The string between the braces.
 */
QString TreasureDescription::getEmbracedValue(const QString& action, QString::SplitBehavior split_behavior)
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
bool TreasureDescription::getValueOfGoldExpression(const QString& expr, uint* result_value)
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
bool TreasureDescription::interpretRoundExpression(const QString& expr, QString* command, uint* result_value)
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

void TreasureDescription::executeImmediateActions() const
{
    vector<QString> actions;
    getImmediateActions(&actions);

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
                    HeroQuestLevelWindow::_hero_quest->getLevel()->getCurrentlyActingHeroActionStates()->set(
                            Level::HeroActionStates::ALL_DONE);
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
                    HeroQuestLevelWindow::_hero_quest->playSoundOnce(
                            SoundManager::SOUND_ATTACK_HERO_SMALL_DECREASE_LIFE_POINTS);
                else if (current_hero->getLifePoints() > 0 && diff_life_points < -1)
                    HeroQuestLevelWindow::_hero_quest->playSoundOnce(
                            SoundManager::SOUND_ATTACK_HERO_BIG_DECREASE_LIFE_POINTS);
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
                    HeroQuestLevelWindow::_hero_quest->playSoundOnce(
                            SoundManager::SOUND_ATTACK_HERO_SMALL_DECREASE_LIFE_POINTS);
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

bool TreasureDescription::containsInventoryItem() const
{
	return getInventoryItemsInternal(0);
}

bool TreasureDescription::getInventoryItems(vector<QString>* items) const
{
	return getInventoryItemsInternal(items);
}

bool TreasureDescription::save(SaveContext& save_context) const
{
    SaveContext::OpenChapter open_chapter(save_context, "TreasureDescription");

    save_context.writeString(_id, "_id");
    //QPixmap* _treasure_image; // stored in Playground
    save_context.writeUInt(_amount, "_amount");
    save_context.writeUInt(uint(_image_id), "_image_id");
    save_context.writeString(_text, "_text");
    save_context.writeStrings(_actions, "_actions");
    return !save_context.fail();
}

bool TreasureDescription::load(LoadContext& load_context)
{
    LoadContext::OpenChapter open_chapter(load_context, "TreasureDescription");

    load_context.readString(&_id, "_id");
    // _treasure_image is assigned later, see loadImagesAndAdjustPointers
    load_context.readUInt(&_amount, "_amount");

    uint tmp_id;
    load_context.readUInt(&tmp_id, "_image_id");
    _image_id = (TreasureDataTypes::TreasureImageID) (tmp_id);

    load_context.readString(&_text, "_text");
    load_context.readStrings(&_actions, "_actions");
    return !load_context.fail();
}

void TreasureDescription::updateTreasureImage()
{
    if (_image_id != TreasureDataTypes::TREASURE_CARD_UNKNOWN)
        _treasure_image = TreasureCardStorage::instance->getTreasureCardImage(_image_id);
}

/*!
 * Removes all action strings which start with "LIFE_POINTS <-1>".
 */
void TreasureDescription::disarmTraps()
{
    bool erased = true;
    while (erased)
    {
        erased = false;
        for (list<QString>::iterator it = _actions.begin(); it != _actions.end(); ++it)
        {
            QString& str = *it;
            if (str.startsWith("LIFE_POINTS <-1>"))
            {
                _actions.erase(it);
                erased = true;
                break;
            }
        }
    }
}

/*!
 * Returns true, if at least one inventory item is contained in the _actions strings.
 * If items != 0, the inventory item ID strings are added to the items vector (which
 * is initially cleared).
 */
bool TreasureDescription::getInventoryItemsInternal(vector<QString>* items) const
{
	bool break_because_inventory_item_found = false;
	bool inventory_item_found = false;
	if (items != 0)
		items->clear();
	for (list<QString>::const_iterator it = _actions.begin(); !break_because_inventory_item_found && it != _actions.end(); ++it)
	{
		const QString& line = *it;
		if (line.contains("BRAVENESS_POTION") ||
			line.contains("HEALING_POTION") ||
			line.contains("HOLY_WATER") ||
			line.contains("SPRINT_POTION") ||
			line.contains("MAGIC_POTION") ||
			line.contains("PHILOSOPHERS_STONE") ||
			line.contains("IMMUNIZATION_POTION") ||
			line.contains("POWER_POTION") ||
			line.contains("RESISTANCE_POTION"))
		{
			inventory_item_found = true;
			if (items == 0)
			{
				break_because_inventory_item_found = true;
			}
			else
			{
				items->push_back(line);
			}
		}
	}
	return inventory_item_found;
}

bool TreasureDescription::readFromFile(const QString& filename)
{
	/*
	1

    TREASURE_CARD_BRAVENESS_POTION

    At the wall you see a leather sleeve containing a potion of braveness. You can drink it just before you attack someone. The potion than enables you to execute two attacks in this round instead of just one.

    BRAVENESS_POTION
	...
    */

    if (filename.startsWith(":/") && filename.length() >= 2)
        _id = filename.right(3); // remove QResource stuff
    else
        _id = filename;

	list<QString> lines;
	if (!FileUtils::getFileContent(filename, &lines))
		return false;

	enum TreasureDescriptionContent {
		AMOUNT = 0,
		IMAGE_ID = 1,
		TEXT = 2,
		ACTION = 3
	};
	TreasureDescriptionContent content_type = AMOUNT;
	unsigned int line_no = 0;
	for (list<QString>::const_iterator it = lines.begin(); it != lines.end(); ++it)
	{
		if (it->size() == 0)
		{
			++line_no;
			continue;
		}

		stringstream str_stream;
		switch (content_type)
		{
		case AMOUNT:
			str_stream << it->toUtf8().constData();
			str_stream >> _amount;
			if (str_stream.fail())
			{
                    cout << "Error interpreting treasure amount \"" << it->toUtf8().constData()
                            << "\" from TreasureDescription file \"" << qPrintable(filename) << "\" (line no. "
                            << line_no << ")!" << endl;
				return false;
			}
			content_type = IMAGE_ID;
			break;

		case IMAGE_ID:
			_image_id = getTreasureImageID(*it);
                if (_image_id == TreasureDataTypes::TREASURE_CARD_UNKNOWN)
			{
                    cout << "Error interpreting treasure image id \"" << it->toUtf8().constData()
                            << "\" from TreasureDescription file \"" << qPrintable(filename) << "\"(line no. "
                            << line_no << ")!" << endl;
				return false;
			}
			content_type = TEXT;
			break;

		case TEXT:
			_text = *it;
			content_type = ACTION;
			break;

		case ACTION:
			_actions.push_back(*it);
			// TODO: interpret/execute inventory action
			break;
		}

		++line_no;
	}

    _treasure_image = TreasureCardStorage::instance->getTreasureCardImage(_image_id);

	return true;
}

bool TreasureDescription::buildDescriptions(set<TreasureDescription>* treasure_card_descriptions)
{
	bool result = true;

	// get all files in TREASURE_CARD_DIR
	list<QString> filenames;
	if (!FileUtils::getFilesOfDirectory(TREASURE_CARD_DIR, &filenames))
		return false;

	for (list<QString>::const_iterator it = filenames.begin(); result && (it != filenames.end()); ++it)
	{
		set<TreasureDescription>::iterator tcd_it = treasure_card_descriptions->insert(TreasureDescription()).first;
        //TreasureDescription& tcd = *tcd_it; // why does this not compile? seems to be a compiler bug?!
		TreasureDescription& tcd = const_cast<TreasureDescription&>(*tcd_it);
		result = tcd.readFromFile(*it);
	}
	return result;
}

TreasureDataTypes::TreasureImageID TreasureDescription::getTreasureImageID(const QString& str)
{
	if (str.compare("TREASURE_CARD_NOTHING") == 0)
        return TreasureDataTypes::TREASURE_CARD_NOTHING;

	if (str.compare("TREASURE_CARD_POISON") == 0)
        return TreasureDataTypes::TREASURE_CARD_POISON;

	if (str.compare("TREASURE_CARD_TRAP_WALL") == 0)
        return TreasureDataTypes::TREASURE_CARD_TRAP_WALL;

	if (str.compare("TREASURE_CARD_TRAP_PIT") == 0)
        return TreasureDataTypes::TREASURE_CARD_TRAP_PIT;

	if (str.compare("TREASURE_CARD_ROAMING_MONSTER") == 0)
        return TreasureDataTypes::TREASURE_CARD_ROAMING_MONSTER;

	if (str.compare("TREASURE_CARD_GOLD") == 0)
        return TreasureDataTypes::TREASURE_CARD_GOLD;

	if (str.compare("TREASURE_CARD_JEWELS") == 0)
        return TreasureDataTypes::TREASURE_CARD_JEWELS;

	if (str.compare("TREASURE_CARD_GEMSTONE") == 0)
        return TreasureDataTypes::TREASURE_CARD_GEMSTONE;

	if (str.compare("TREASURE_CARD_GOLD_TREASURE") == 0)
        return TreasureDataTypes::TREASURE_CARD_GOLD_TREASURE;

	if (str.compare("TREASURE_CARD_BRAVENESS_POTION") == 0)
        return TreasureDataTypes::TREASURE_CARD_BRAVENESS_POTION;

	if (str.compare("TREASURE_CARD_HEALING_POTION") == 0)
        return TreasureDataTypes::TREASURE_CARD_HEALING_POTION;

	if (str.compare("TREASURE_CARD_HOLY_WATER") == 0)
        return TreasureDataTypes::TREASURE_CARD_HOLY_WATER;

	if (str.compare("TREASURE_CARD_IMMUNIZATION_POTION") == 0)
        return TreasureDataTypes::TREASURE_CARD_IMMUNIZATION_POTION;

	if (str.compare("TREASURE_CARD_MAGIC_POTION") == 0)
        return TreasureDataTypes::TREASURE_CARD_MAGIC_POTION;

	if (str.compare("TREASURE_CARD_PHILOSOPHERS_STONE") == 0)
        return TreasureDataTypes::TREASURE_CARD_PHILOSOPHERS_STONE;

	if (str.compare("TREASURE_CARD_POWER_POTION") == 0)
        return TreasureDataTypes::TREASURE_CARD_POWER_POTION;

	if (str.compare("TREASURE_CARD_RESISTANCE_POTION") == 0)
        return TreasureDataTypes::TREASURE_CARD_RESISTANCE_POTION;

	if (str.compare("TREASURE_CARD_SPRINT_POTION") == 0)
        return TreasureDataTypes::TREASURE_CARD_SPRINT_POTION;

    return TreasureDataTypes::TREASURE_CARD_UNKNOWN;
}

bool TreasureDescription::operator<(const TreasureDescription& other) const
{
    if (_image_id != other._image_id)
        return _image_id < other._image_id;

    return _id < other._id;
}
