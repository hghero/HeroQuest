#include "SpellCardStorage.h"

#include <iostream>

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


SpellCardStorage::SpellCardStorage()
: _spell_cards(),

  _spell_card_air_back_image(0),
  _spell_card_water_back_image(0),
  _spell_card_earth_back_image(0),
  _spell_card_fire_back_image(0),

  _spell_card_air_genie_image(0),
  _spell_card_air_storm_image(0),
  _spell_card_air_tailwind_image(0),

  _spell_card_water_fog_image(0),
  _spell_card_water_morpheus_image(0),
  _spell_card_water_healing_potion_image(0),

  _spell_card_earth_granite_skin_image(0),
  _spell_card_earth_through_wall_image(0),
  _spell_card_earth_healing_magic_image(0),

  _spell_card_fire_braveness_image(0),
  _spell_card_fire_burning_rage_image(0),
  _spell_card_fire_fireball_image(0)
{
    // NIX
}

SpellCardStorage::~SpellCardStorage()
{
    // NIX
}

bool SpellCardStorage::loadSpellCards()
{
    // (1) images

    // spell_card_air
    _spell_card_air_back_image = new QPixmap(SPELL_CARD_AIR_BACK_FILENAME);
    if (_spell_card_air_back_image == 0) {
       cout << qPrintable(SPELL_CARD_AIR_BACK_FILENAME) << " not found or failed to load" << endl;
       return false;
    }
    _spell_card_air_genie_image = new QPixmap(SPELL_CARD_AIR_GENIE_FILENAME);
    if (_spell_card_air_genie_image == 0) {
        cout << qPrintable(SPELL_CARD_AIR_GENIE_FILENAME) << " not found or failed to load" << endl;
        return false;
    }
    _spell_card_air_storm_image = new QPixmap(SPELL_CARD_AIR_STORM_FILENAME);
    if (_spell_card_air_storm_image == 0) {
        cout << qPrintable(SPELL_CARD_AIR_STORM_FILENAME) << " not found or failed to load" << endl;
        return false;
    }
    _spell_card_air_tailwind_image = new QPixmap(SPELL_CARD_AIR_TAILWIND_FILENAME);
    if (_spell_card_air_tailwind_image == 0) {
        cout << qPrintable(SPELL_CARD_AIR_TAILWIND_FILENAME) << " not found or failed to load" << endl;
        return false;
    }

    // spell_card_water
    _spell_card_water_back_image = new QPixmap(SPELL_CARD_WATER_BACK_FILENAME);
    if (_spell_card_water_back_image == 0) {
       cout << qPrintable(SPELL_CARD_WATER_BACK_FILENAME) << " not found or failed to load" << endl;
       return false;
    }
    _spell_card_water_fog_image = new QPixmap(SPELL_CARD_WATER_FOG_FILENAME);
    if (_spell_card_water_fog_image == 0) {
        cout << qPrintable(SPELL_CARD_WATER_FOG_FILENAME) << " not found or failed to load" << endl;
        return false;
    }
    _spell_card_water_morpheus_image = new QPixmap(SPELL_CARD_WATER_MORPHEUS_FILENAME);
    if (_spell_card_water_morpheus_image == 0) {
        cout << qPrintable(SPELL_CARD_WATER_MORPHEUS_FILENAME) << " not found or failed to load" << endl;
        return false;
    }
    _spell_card_water_healing_potion_image = new QPixmap(SPELL_CARD_WATER_HEALING_POTION_FILENAME);
    if (_spell_card_water_healing_potion_image == 0) {
        cout << qPrintable(SPELL_CARD_WATER_HEALING_POTION_FILENAME) << " not found or failed to load" << endl;
        return false;
    }

    // spell_card_earth
    _spell_card_earth_back_image = new QPixmap(SPELL_CARD_EARTH_BACK_FILENAME);
    if (_spell_card_earth_back_image == 0) {
       cout << qPrintable(SPELL_CARD_EARTH_BACK_FILENAME) << " not found or failed to load" << endl;
       return false;
    }
    _spell_card_earth_granite_skin_image = new QPixmap(SPELL_CARD_EARTH_GRANITE_SKIN_FILENAME);
    if (_spell_card_earth_granite_skin_image == 0) {
        cout << qPrintable(SPELL_CARD_EARTH_GRANITE_SKIN_FILENAME) << " not found or failed to load" << endl;
        return false;
    }
    _spell_card_earth_through_wall_image = new QPixmap(SPELL_CARD_EARTH_THROUGH_WALL_FILENAME);
    if (_spell_card_earth_through_wall_image == 0) {
        cout << qPrintable(SPELL_CARD_EARTH_THROUGH_WALL_FILENAME) << " not found or failed to load" << endl;
        return false;
    }
    _spell_card_earth_healing_magic_image = new QPixmap(SPELL_CARD_EARTH_HEALING_MAGIC_FILENAME);
    if (_spell_card_earth_healing_magic_image == 0) {
        cout << qPrintable(SPELL_CARD_EARTH_HEALING_MAGIC_FILENAME) << " not found or failed to load" << endl;
        return false;
    }

    // spell_card_fire
    _spell_card_fire_back_image = new QPixmap(SPELL_CARD_FIRE_BACK_FILENAME);
    if (_spell_card_fire_back_image == 0) {
       cout << qPrintable(SPELL_CARD_FIRE_BACK_FILENAME) << " not found or failed to load" << endl;
       return false;
    }
    _spell_card_fire_braveness_image = new QPixmap(SPELL_CARD_FIRE_BRAVENESS_FILENAME);
    if (_spell_card_fire_braveness_image == 0) {
        cout << qPrintable(SPELL_CARD_FIRE_BRAVENESS_FILENAME) << " not found or failed to load" << endl;
        return false;
    }
    _spell_card_fire_burning_rage_image = new QPixmap(SPELL_CARD_FIRE_BURNING_RAGE_FILENAME);
    if (_spell_card_fire_burning_rage_image == 0) {
        cout << qPrintable(SPELL_CARD_FIRE_BURNING_RAGE_FILENAME) << " not found or failed to load" << endl;
        return false;
    }
    _spell_card_fire_fireball_image = new QPixmap(SPELL_CARD_FIRE_FIREBALL_FILENAME);
    if (_spell_card_fire_fireball_image == 0) {
        cout << qPrintable(SPELL_CARD_FIRE_FIREBALL_FILENAME) << " not found or failed to load" << endl;
        return false;
    }

    // (2) spell cards
    const uint NUM_SPELL_CARDS = 12;
    _spell_cards.clear();
    _spell_cards.reserve(NUM_SPELL_CARDS);
    for (uint i = 0; i < NUM_SPELL_CARDS; ++i)
    {
        SpellCard::SpellID spell_id = (SpellCard::SpellID)i;
        SpellCard spell_card(spell_id);
        _spell_cards.push_back(spell_card);
    }

    return true;
}

vector<SpellCard>& SpellCardStorage::getSpellCards()
{
    return _spell_cards;
}


