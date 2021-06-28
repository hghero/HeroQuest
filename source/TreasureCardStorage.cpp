#include "TreasureCardStorage.h"

#include <iostream>
#include <set>

#include <QtGui/QPixmap>

#include "StreamUtils.h"
#include "Debug.h"
#include "TreasureDescription.h"
#include "TreasureCard.h"
#include "SaveContext.h"
#include "LoadContext.h"

using namespace std;

const QString TreasureCardStorage::TREASURE_CARD_BACK_FILENAME = ":/graphics/treasure_card_back.jpg";
const QString TreasureCardStorage::TREASURE_CHEST_3D_FILENAME = ":/graphics/chest_3d.tga";
const QString TreasureCardStorage::TREASURE_CARD_BRAVENESS_POTION_FILENAME =
        ":/graphics/treasure_card_braveness_potion.jpg";
const QString TreasureCardStorage::TREASURE_CARD_GEMSTONE_FILENAME = ":/graphics/treasure_card_gemstone.jpg";
const QString TreasureCardStorage::TREASURE_CARD_GOLD_FILENAME = ":/graphics/treasure_card_gold.jpg";
const QString TreasureCardStorage::TREASURE_CARD_GOLD_TREASURE_FILENAME = ":/graphics/treasure_card_gold_treasure.jpg";
const QString TreasureCardStorage::TREASURE_CARD_HEALING_POTION_FILENAME = ":/graphics/treasure_card_healing_potion.jpg";
const QString TreasureCardStorage::TREASURE_CARD_HOLY_WATER_FILENAME = ":/graphics/treasure_card_holy_water.jpg";
const QString TreasureCardStorage::TREASURE_CARD_IMMUNIZATION_POTION_FILENAME =
        ":/graphics/treasure_card_immunization_potion.jpg";
const QString TreasureCardStorage::TREASURE_CARD_JEWELS_FILENAME = ":/graphics/treasure_card_jewels.jpg";
const QString TreasureCardStorage::TREASURE_CARD_MAGIC_POTION_FILENAME = ":/graphics/treasure_card_magic_potion.jpg";
const QString TreasureCardStorage::TREASURE_CARD_NOTHING_FILENAME = ":/graphics/treasure_card_nothing.jpg";
const QString TreasureCardStorage::TREASURE_CARD_PHILOSOPHERS_STONE_FILENAME =
        ":/graphics/treasure_card_philosophers_stone.jpg";
const QString TreasureCardStorage::TREASURE_CARD_POISON_FILENAME = ":/graphics/treasure_card_poison.jpg";
const QString TreasureCardStorage::TREASURE_CARD_POWER_POTION_FILENAME = ":/graphics/treasure_card_power_potion.jpg";
const QString TreasureCardStorage::TREASURE_CARD_RESISTANCE_POTION_FILENAME =
        ":/graphics/treasure_card_resistance_potion.jpg";
const QString TreasureCardStorage::TREASURE_CARD_ROAMING_MONSTER_FILENAME =
        ":/graphics/treasure_card_roaming_monster.jpg";
const QString TreasureCardStorage::TREASURE_CARD_SPRINT_POTION_FILENAME = ":/graphics/treasure_card_sprint_potion.jpg";
const QString TreasureCardStorage::TREASURE_CARD_TRAP_PIT_FILENAME = ":/graphics/treasure_card_trap_pit.jpg";
const QString TreasureCardStorage::TREASURE_CARD_TRAP_WALL_FILENAME = ":/graphics/treasure_card_trap_wall.jpg";

TreasureCardStorage* TreasureCardStorage::instance = 0;

TreasureCardStorage::TreasureCardStorage()
        :
        _treasure_cards(), _treasure_card_back_image(0), _chest_3d_image(0), _treasure_card_images()
{
    instance = this;
}

TreasureCardStorage::~TreasureCardStorage()
{
    for (map<TreasureDataTypes::TreasureImageID, QPixmap*>::iterator it = _treasure_card_images.begin();
            it != _treasure_card_images.end();
            ++it)
    {
        delete it->second;
    }

    delete _treasure_card_back_image;
    delete _chest_3d_image;
}

bool TreasureCardStorage::loadPixmap(const QString& filename, QPixmap** pixmap)
{
    if (*pixmap == 0)
    {
        *pixmap = new QPixmap(filename);
        if (*pixmap == 0)
        {
            cout << qPrintable(TREASURE_CARD_BACK_FILENAME) << " not found or failed to load" << endl;
            return false;
        }
    }
    else
    {
        DVX(("Warning: while trying to load %s, the traget pixmap already had sth. stored!", qPrintable(filename)));
        return false;
    }

    return true;
}

bool TreasureCardStorage::addTreasureCardImageIfNotYetPresent(const QString& filename,
        TreasureDataTypes::TreasureImageID treasure_image_id)
{
    QPixmap* tmp_image = 0;
    if (_treasure_card_images.find(treasure_image_id) == _treasure_card_images.end())
    {
        tmp_image = new QPixmap(filename);
        if (tmp_image == 0)
        {
            cout << qPrintable(filename) << " not found or failed to load" << endl;
            return false;
        }
        _treasure_card_images[treasure_image_id] = tmp_image;
    }
    else
    {
        cout << "Warning: Image for TreasureImageID " << treasure_image_id << " has already benn loaded!" << endl;
        return false;
    }

    return true;
}

/*!
 * Loads all treasure card images. True, if everything has been loaded successfully.
 */
bool TreasureCardStorage::loadTreasureCards()
{
    // --- 1. images ---

    loadPixmap(TREASURE_CARD_BACK_FILENAME, &_treasure_card_back_image);
    loadPixmap(TREASURE_CHEST_3D_FILENAME, &_chest_3d_image);

    QPixmap* tmp_image = 0;

    addTreasureCardImageIfNotYetPresent(TREASURE_CARD_BRAVENESS_POTION_FILENAME,
            TreasureDataTypes::TREASURE_CARD_BRAVENESS_POTION);
    addTreasureCardImageIfNotYetPresent(TREASURE_CARD_GEMSTONE_FILENAME, TreasureDataTypes::TREASURE_CARD_GEMSTONE);
    addTreasureCardImageIfNotYetPresent(TREASURE_CARD_GOLD_FILENAME, TreasureDataTypes::TREASURE_CARD_GOLD);
    addTreasureCardImageIfNotYetPresent(TREASURE_CARD_GOLD_TREASURE_FILENAME,
            TreasureDataTypes::TREASURE_CARD_GOLD_TREASURE);
    addTreasureCardImageIfNotYetPresent(TREASURE_CARD_HEALING_POTION_FILENAME,
            TreasureDataTypes::TREASURE_CARD_HEALING_POTION);
    addTreasureCardImageIfNotYetPresent(TREASURE_CARD_HOLY_WATER_FILENAME, TreasureDataTypes::TREASURE_CARD_HOLY_WATER);
    addTreasureCardImageIfNotYetPresent(TREASURE_CARD_IMMUNIZATION_POTION_FILENAME,
            TreasureDataTypes::TREASURE_CARD_IMMUNIZATION_POTION);
    addTreasureCardImageIfNotYetPresent(TREASURE_CARD_JEWELS_FILENAME, TreasureDataTypes::TREASURE_CARD_JEWELS);
    addTreasureCardImageIfNotYetPresent(TREASURE_CARD_MAGIC_POTION_FILENAME,
            TreasureDataTypes::TREASURE_CARD_MAGIC_POTION);
    addTreasureCardImageIfNotYetPresent(TREASURE_CARD_NOTHING_FILENAME, TreasureDataTypes::TREASURE_CARD_NOTHING);
    addTreasureCardImageIfNotYetPresent(TREASURE_CARD_PHILOSOPHERS_STONE_FILENAME,
            TreasureDataTypes::TREASURE_CARD_PHILOSOPHERS_STONE);
    addTreasureCardImageIfNotYetPresent(TREASURE_CARD_POISON_FILENAME, TreasureDataTypes::TREASURE_CARD_POISON);
    addTreasureCardImageIfNotYetPresent(TREASURE_CARD_POWER_POTION_FILENAME,
            TreasureDataTypes::TREASURE_CARD_POWER_POTION);
    addTreasureCardImageIfNotYetPresent(TREASURE_CARD_RESISTANCE_POTION_FILENAME,
            TreasureDataTypes::TREASURE_CARD_RESISTANCE_POTION);
    addTreasureCardImageIfNotYetPresent(TREASURE_CARD_ROAMING_MONSTER_FILENAME,
            TreasureDataTypes::TREASURE_CARD_ROAMING_MONSTER);
    addTreasureCardImageIfNotYetPresent(TREASURE_CARD_SPRINT_POTION_FILENAME,
            TreasureDataTypes::TREASURE_CARD_SPRINT_POTION);
    addTreasureCardImageIfNotYetPresent(TREASURE_CARD_TRAP_PIT_FILENAME, TreasureDataTypes::TREASURE_CARD_TRAP_PIT);
    addTreasureCardImageIfNotYetPresent(TREASURE_CARD_TRAP_WALL_FILENAME, TreasureDataTypes::TREASURE_CARD_TRAP_WALL);

#if 0
    cout << "_treasure_card_images content:" << endl;
    for (map<TreasureImageID, QPixmap*>::const_iterator it = _treasure_card_images.begin(); it != _treasure_card_images.end(); ++it)
    {
        cout << "  id " << int(it->first) << endl;
    }
#endif

    // --- 2. treasure cards ---

    // read the treasure card description text files => build set of TreasureDescriptions
    set<TreasureDescription> treasure_card_descriptions;
    if (!TreasureDescription::buildDescriptions(&treasure_card_descriptions))
        return false;

    if (treasure_card_descriptions.empty())
        cout << "Warning: no treasure card descriptions loaded!" << endl;

    // build vector of treasure cards, and firstly put them into the treasure card deposition
    _treasure_cards.clear();
    for (set<TreasureDescription>::const_iterator it = treasure_card_descriptions.begin();
            it != treasure_card_descriptions.end(); ++it)
    {
        const TreasureDescription& description = *it;

        for (unsigned int i = 0; i < description.getAmount(); ++i)
        {
            _treasure_cards.push_back(TreasureCard(description));
        }
    }

    return true;
}

vector<TreasureCard>& TreasureCardStorage::getTreasureCards()
{
    return _treasure_cards;
}

QPixmap* TreasureCardStorage::getTreasureCardBackImage() const
{
    return _treasure_card_back_image;
}

QPixmap* TreasureCardStorage::getTreasureCardImage(const TreasureDataTypes::TreasureImageID& id) const
{
#if 0
    cout << "_treasure_card_images content:" << endl;
    for (map<TreasureImageID, QPixmap*>::const_iterator it = _treasure_card_images.begin(); it != _treasure_card_images.end(); ++it)
    {
        cout << "  id " << int(it->first) << endl;
    }
    cout << "requested: id " << int(id) << endl;
#endif
    map<TreasureDataTypes::TreasureImageID, QPixmap*>::const_iterator it = _treasure_card_images.find(id);
    if (it == _treasure_card_images.end())
    {
        cout << "Error: treasure card ID " << int(id) << " not found in _treasure_card_images map!" << endl;
        return 0;
    }
    return it->second;
}

QPixmap* TreasureCardStorage::getChest3DImage() const
{
    return _chest_3d_image;
}

bool TreasureCardStorage::save(SaveContext& save_context) const
{
    SaveContext::OpenChapter open_chapter(save_context, "TreasureCardStorage");

    save_context.writeUInt(_treasure_cards.size(), "_treasure_cards.size()");
    for (uint i = 0; i < _treasure_cards.size(); ++i)
    {
        if (!_treasure_cards[i].save(save_context))
        {
            DVX(("Error saving treasure card %d!", i));
            return false;
        }
    }
    return true;
}

bool TreasureCardStorage::load(LoadContext& load_context)
{
    _treasure_cards.clear();
    uint num_treasure_cards;
    load_context.readUInt(&num_treasure_cards, "_treasure_cards.size()");
    for (uint i = 0; i < num_treasure_cards; ++i)
    {
        TreasureCard treasure_card;
        if (!treasure_card.load(load_context))
        {
            DVX(("Error loading treasure card %d!", i));
            return false;
        }

        _treasure_cards.push_back(treasure_card);
    }
    return true;
}
