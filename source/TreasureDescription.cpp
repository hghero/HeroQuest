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

using namespace std;

const QString TreasureDescription::TREASURE_CARD_DIR = ":/treasure_cards";

TreasureDescription::TreasureDescription()
:
_id("unknown"),
_treasure_image(0),
_amount(0),
_image_id(Playground::TREASURE_CARD_UNKNOWN),
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
_image_id(Playground::TREASURE_CARD_UNKNOWN),
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

void TreasureDescription::setTreasureImageID(const Playground::TreasureImageID& treasure_image_id)
{
	_image_id = treasure_image_id;
}

const Playground::TreasureImageID& TreasureDescription::getTreasureImageID() const
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

bool TreasureDescription::containsInventoryItem() const
{
	return getInventoryItemsInternal(0);
}

bool TreasureDescription::getInventoryItems(vector<QString>* items) const
{
	return getInventoryItemsInternal(items);
}

bool TreasureDescription::save(std::ostream& stream) const
{
    StreamUtils::write(stream, _id);
    //QPixmap* _treasure_image; // stored in Playground
    StreamUtils::writeUInt(stream, _amount);
    StreamUtils::writeUInt(stream, uint(_image_id));
    StreamUtils::write(stream, _text);
    StreamUtils::write(stream, _actions);
    return !stream.fail();
}

bool TreasureDescription::load(std::istream& stream)
{
    StreamUtils::read(stream, &_id);
    // _treasure_image is assigned later, see loadImagesAndAdjustPointers
    StreamUtils::readUInt(stream, &_amount);

    uint tmp_id;
    StreamUtils::readUInt(stream, &tmp_id);
    _image_id = (Playground::TreasureImageID)(tmp_id);

    StreamUtils::read(stream, &_text);
    StreamUtils::read(stream, &_actions);
    return !stream.fail();
}

void TreasureDescription::updateTreasureImage()
{
    if (_image_id != Playground::TREASURE_CARD_UNKNOWN)
        _treasure_image = HeroQuestLevelWindow::_hero_quest->getPlayground()->getTreasureCardImage(_image_id);
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
			if (_image_id == Playground::TREASURE_CARD_UNKNOWN)
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

	_treasure_image = HeroQuestLevelWindow::_hero_quest->getPlayground()->getTreasureCardImage(_image_id);

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

Playground::TreasureImageID TreasureDescription::getTreasureImageID(const QString& str)
{
	if (str.compare("TREASURE_CARD_NOTHING") == 0)
		return Playground::TREASURE_CARD_NOTHING;

	if (str.compare("TREASURE_CARD_POISON") == 0)
		return Playground::TREASURE_CARD_POISON;

	if (str.compare("TREASURE_CARD_TRAP_WALL") == 0)
		return Playground::TREASURE_CARD_TRAP_WALL;

	if (str.compare("TREASURE_CARD_TRAP_PIT") == 0)
		return Playground::TREASURE_CARD_TRAP_PIT;

	if (str.compare("TREASURE_CARD_ROAMING_MONSTER") == 0)
		return Playground::TREASURE_CARD_ROAMING_MONSTER;

	if (str.compare("TREASURE_CARD_GOLD") == 0)
		return Playground::TREASURE_CARD_GOLD;

	if (str.compare("TREASURE_CARD_JEWELS") == 0)
		return Playground::TREASURE_CARD_JEWELS;

	if (str.compare("TREASURE_CARD_GEMSTONE") == 0)
		return Playground::TREASURE_CARD_GEMSTONE;

	if (str.compare("TREASURE_CARD_GOLD_TREASURE") == 0)
		return Playground::TREASURE_CARD_GOLD_TREASURE;

	if (str.compare("TREASURE_CARD_BRAVENESS_POTION") == 0)
		return Playground::TREASURE_CARD_BRAVENESS_POTION;

	if (str.compare("TREASURE_CARD_HEALING_POTION") == 0)
		return Playground::TREASURE_CARD_HEALING_POTION;

	if (str.compare("TREASURE_CARD_HOLY_WATER") == 0)
		return Playground::TREASURE_CARD_HOLY_WATER;

	if (str.compare("TREASURE_CARD_IMMUNIZATION_POTION") == 0)
		return Playground::TREASURE_CARD_IMMUNIZATION_POTION;

	if (str.compare("TREASURE_CARD_MAGIC_POTION") == 0)
		return Playground::TREASURE_CARD_MAGIC_POTION;

	if (str.compare("TREASURE_CARD_PHILOSOPHERS_STONE") == 0)
		return Playground::TREASURE_CARD_PHILOSOPHERS_STONE;

	if (str.compare("TREASURE_CARD_POWER_POTION") == 0)
		return Playground::TREASURE_CARD_POWER_POTION;

	if (str.compare("TREASURE_CARD_RESISTANCE_POTION") == 0)
		return Playground::TREASURE_CARD_RESISTANCE_POTION;

	if (str.compare("TREASURE_CARD_SPRINT_POTION") == 0)
		return Playground::TREASURE_CARD_SPRINT_POTION;

	return Playground::TREASURE_CARD_UNKNOWN;
}

bool TreasureDescription::operator<(const TreasureDescription& other) const
{
    if (_image_id != other._image_id)
        return _image_id < other._image_id;

    return _id < other._id;
}
