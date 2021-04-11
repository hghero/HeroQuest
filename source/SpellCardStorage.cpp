#include "SpellCardStorage.h"

#include <iostream>

#include <QtGui/QPixmap>

#include "StreamUtils.h"
#include "Debug.h"

using namespace std;

// spell card image filenames
const QString SpellCardStorage::SPELL_CARD_AIR_BACK_FILENAME = ":/graphics/spell_card_air_back.jpg";
const QString SpellCardStorage::SPELL_CARD_WATER_BACK_FILENAME = ":/graphics/spell_card_water_back.jpg";
const QString SpellCardStorage::SPELL_CARD_EARTH_BACK_FILENAME = ":/graphics/spell_card_earth_back.jpg";
const QString SpellCardStorage::SPELL_CARD_FIRE_BACK_FILENAME = ":/graphics/spell_card_fire_back.jpg";

const QString SpellCardStorage::SPELL_CARD_AIR_GENIE_FILENAME = ":/graphics/spell_card_air_genie.jpg";
const QString SpellCardStorage::SPELL_CARD_AIR_STORM_FILENAME = ":/graphics/spell_card_air_storm.jpg";
const QString SpellCardStorage::SPELL_CARD_AIR_TAILWIND_FILENAME = ":/graphics/spell_card_air_tailwind.jpg";

const QString SpellCardStorage::SPELL_CARD_WATER_FOG_FILENAME = ":/graphics/spell_card_water_fog.jpg";
const QString SpellCardStorage::SPELL_CARD_WATER_MORPHEUS_FILENAME = ":/graphics/spell_card_water_morpheus.jpg";
const QString SpellCardStorage::SPELL_CARD_WATER_HEALING_POTION_FILENAME = ":/graphics/spell_card_water_healing_potion.jpg";

const QString SpellCardStorage::SPELL_CARD_EARTH_GRANITE_SKIN_FILENAME = ":/graphics/spell_card_earth_granite_skin.jpg";
const QString SpellCardStorage::SPELL_CARD_EARTH_THROUGH_WALL_FILENAME = ":/graphics/spell_card_earth_through_wall.jpg";
const QString SpellCardStorage::SPELL_CARD_EARTH_HEALING_MAGIC_FILENAME = ":/graphics/spell_card_earth_healing_magic.jpg";

const QString SpellCardStorage::SPELL_CARD_FIRE_BRAVENESS_FILENAME = ":/graphics/spell_card_fire_braveness.jpg";
const QString SpellCardStorage::SPELL_CARD_FIRE_BURNING_RAGE_FILENAME = ":/graphics/spell_card_fire_burning_rage.jpg";
const QString SpellCardStorage::SPELL_CARD_FIRE_FIREBALL_FILENAME = ":/graphics/spell_card_fire_fireball.jpg";

SpellCardStorage* SpellCardStorage::instance = 0;

SpellCardStorage::SpellCardStorage()
        :
        _spell_card_stock(), _spell_card_deposition(), _spell_card_back_images(), _spell_card_images()
{
    instance = this;
}

SpellCardStorage::~SpellCardStorage()
{
    for (map<SpellCard::SpellFamily, QPixmap*>::iterator it = _spell_card_back_images.begin();
            it != _spell_card_back_images.end(); ++it)
    {
        delete it->second;
    }

    for (map<SpellCard::SpellID, QPixmap*>::iterator it = _spell_card_images.begin(); it != _spell_card_images.end();
            ++it)
    {
        delete it->second;
    }
}

bool SpellCardStorage::loadSpellCards()
{
    // (1) images

    // _spell_card_back_images
    addSpellCardBackImageIfNotYetPresent(SpellCard::AIR, SPELL_CARD_AIR_BACK_FILENAME);
    addSpellCardBackImageIfNotYetPresent(SpellCard::WATER, SPELL_CARD_WATER_BACK_FILENAME);
    addSpellCardBackImageIfNotYetPresent(SpellCard::EARTH, SPELL_CARD_EARTH_BACK_FILENAME);
    addSpellCardBackImageIfNotYetPresent(SpellCard::FIRE, SPELL_CARD_FIRE_BACK_FILENAME);
    for (map<SpellCard::SpellFamily, QPixmap*>::const_iterator it = _spell_card_back_images.begin();
            it != _spell_card_back_images.end(); ++it)
    {
        if (it->second == 0)
        {
            cout << "Spell card back image for spell family " << uint(it->first) << " not found or failed to load"
                    << endl;
            return false;
        }
    }

    // _spell_card_images
    addSpellCardImageIfNotYetPresent(SpellCard::GENIE, SPELL_CARD_AIR_GENIE_FILENAME);
    addSpellCardImageIfNotYetPresent(SpellCard::STORM, SPELL_CARD_AIR_STORM_FILENAME);
    addSpellCardImageIfNotYetPresent(SpellCard::TAILWIND, SPELL_CARD_AIR_TAILWIND_FILENAME);

    addSpellCardImageIfNotYetPresent(SpellCard::FOG, SPELL_CARD_WATER_FOG_FILENAME);
    addSpellCardImageIfNotYetPresent(SpellCard::HEALING_POTION, SPELL_CARD_WATER_HEALING_POTION_FILENAME);
    addSpellCardImageIfNotYetPresent(SpellCard::MORPHEUS, SPELL_CARD_WATER_MORPHEUS_FILENAME);

    addSpellCardImageIfNotYetPresent(SpellCard::GRANITE_SKIN, SPELL_CARD_EARTH_GRANITE_SKIN_FILENAME);
    addSpellCardImageIfNotYetPresent(SpellCard::HEALING_MAGIC, SPELL_CARD_EARTH_HEALING_MAGIC_FILENAME);
    addSpellCardImageIfNotYetPresent(SpellCard::THROUGH_WALL, SPELL_CARD_EARTH_THROUGH_WALL_FILENAME);

    addSpellCardImageIfNotYetPresent(SpellCard::BRAVENESS, SPELL_CARD_FIRE_BRAVENESS_FILENAME);
    addSpellCardImageIfNotYetPresent(SpellCard::BURNING_RAGE, SPELL_CARD_FIRE_BURNING_RAGE_FILENAME);
    addSpellCardImageIfNotYetPresent(SpellCard::FIREBALL, SPELL_CARD_FIRE_FIREBALL_FILENAME);
    for (map<SpellCard::SpellID, QPixmap*>::const_iterator it = _spell_card_images.begin();
            it != _spell_card_images.end(); ++it)
    {
        if (it->second == 0)
        {
            cout << "Spell card image for spell ID " << uint(it->first) << " not found or failed to load" << endl;
            return false;
        }
    }

    // (2) add spell cards to _spell_card_stock
    const uint NUM_SPELL_CARDS = 12;
    _spell_card_stock.clear();
    _spell_card_stock.reserve(NUM_SPELL_CARDS);
    for (uint i = 0; i < NUM_SPELL_CARDS; ++i)
    {
        SpellCard::SpellID spell_id = (SpellCard::SpellID) i;
        SpellCard spell_card(spell_id);
        _spell_card_stock.push_back(spell_card);
    }

    return true;
}

void SpellCardStorage::addSpellCardBackImageIfNotYetPresent(const SpellCard::SpellFamily& spell_family,
        const QString& filename)
{
    if (_spell_card_back_images.find(spell_family) == _spell_card_back_images.end())
        _spell_card_back_images[spell_family] = new QPixmap(filename);
}

void SpellCardStorage::addSpellCardImageIfNotYetPresent(const SpellCard::SpellID& spell_id, const QString& filename)
{
    if (_spell_card_images.find(spell_id) == _spell_card_images.end())
        _spell_card_images[spell_id] = new QPixmap(filename);
}

vector<SpellCard>& SpellCardStorage::getSpellCardStock()
{
    return _spell_card_stock;
}

/*!
 * @param spell_cards
 */
void SpellCardStorage::removeSpellCardsFromStock(const vector<SpellCard>& spell_cards)
{
    for (vector<SpellCard>::const_iterator it = spell_cards.begin(); it != spell_cards.end(); ++it)
    {
        const SpellCard& spell_card_to_remove = *it;
        for (vector<SpellCard>::iterator it_stock = _spell_card_stock.begin(); it_stock != _spell_card_stock.end();
                ++it_stock)
        {
            if (it_stock->getSpellID() == spell_card_to_remove.getSpellID())
            {
                _spell_card_stock.erase(it_stock);
                break;
            }
        }
    }
}

vector<SpellCard>& SpellCardStorage::getSpellCardDeposition()
{
    return _spell_card_deposition;
}

QPixmap* SpellCardStorage::getSpellCardBackImage(const SpellCard::SpellFamily& spell_family) const
{
    map<SpellCard::SpellFamily, QPixmap*>::const_iterator it = _spell_card_back_images.find(spell_family);
    if (it == _spell_card_back_images.end())
    {
        cout << "Error: spell card family " << uint(spell_family) << " not found in _spell_card_back_images map!"
                << endl;
        return 0;
    }
    return it->second;
}

QPixmap* SpellCardStorage::getSpellCardImage(const SpellCard::SpellID& spell_id) const
{
    map<SpellCard::SpellID, QPixmap*>::const_iterator it = _spell_card_images.find(spell_id);
    if (it == _spell_card_images.end())
    {
        cout << "Error: spell card ID " << uint(spell_id) << " not found in _spell_card_images map!" << endl;
        return 0;
    }
    return it->second;
}

bool SpellCardStorage::save(ostream& stream) const
{
    StreamUtils::writeUInt(stream, _spell_card_stock.size());
    for (uint i = 0; i < _spell_card_stock.size(); ++i)
    {
        if (!_spell_card_stock[i].save(stream))
        {
            DVX(("Error saving card %d from spell card stock!", i));
            return false;
        }
    }

    StreamUtils::writeUInt(stream, _spell_card_deposition.size());
    for (uint i = 0; i < _spell_card_deposition.size(); ++i)
    {
        if (!_spell_card_deposition[i].save(stream))
        {
            DVX(("Error saving card %d from spell card deposition!", i));
            return false;
        }
    }

    return true;
}

bool SpellCardStorage::load(istream& stream)
{
    _spell_card_stock.clear();
    uint num_spell_cards_stock;
    StreamUtils::readUInt(stream, &num_spell_cards_stock);
    for (uint i = 0; i < num_spell_cards_stock; ++i)
    {
        SpellCard spell_card;
        if (!spell_card.load(stream))
        {
            DVX(("Error loading spell card %d to spell card stock!", i));
            return false;
        }

        _spell_card_stock.push_back(spell_card);
    }

    _spell_card_deposition.clear();
    uint num_spell_cards_deposition;
    StreamUtils::readUInt(stream, &num_spell_cards_deposition);
    for (uint i = 0; i < num_spell_cards_deposition; ++i)
    {
        SpellCard spell_card;
        if (!spell_card.load(stream))
        {
            DVX(("Error loading spell card %d to spell card deposition!", i));
            return false;
        }

        _spell_card_deposition.push_back(spell_card);
    }

    return true;
}
