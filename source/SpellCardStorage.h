#ifndef SPELL_CARD_STORAGE_H
#define SPELL_CARD_STORAGE_H

#include <vector>

#include <QtWidgets/QLabel>

#include "SpellCard.h"

/*!
 * Needed to deal with SpellCards before a level has begun.
 * Simply holds the spell cards before and after a level.
 */
class SpellCardStorage
{
    public:
    //! spell card image filenames
    static const QString SPELL_CARD_AIR_BACK_FILENAME;
    static const QString SPELL_CARD_WATER_BACK_FILENAME;
    static const QString SPELL_CARD_EARTH_BACK_FILENAME;
    static const QString SPELL_CARD_FIRE_BACK_FILENAME;

    static const QString SPELL_CARD_AIR_GENIE_FILENAME;
    static const QString SPELL_CARD_AIR_STORM_FILENAME;
    static const QString SPELL_CARD_AIR_TAILWIND_FILENAME;

    static const QString SPELL_CARD_WATER_FOG_FILENAME;
    static const QString SPELL_CARD_WATER_MORPHEUS_FILENAME;
    static const QString SPELL_CARD_WATER_HEALING_POTION_FILENAME;

    static const QString SPELL_CARD_EARTH_GRANITE_SKIN_FILENAME;
    static const QString SPELL_CARD_EARTH_THROUGH_WALL_FILENAME;
    static const QString SPELL_CARD_EARTH_HEALING_MAGIC_FILENAME;

    static const QString SPELL_CARD_FIRE_BRAVENESS_FILENAME;
    static const QString SPELL_CARD_FIRE_BURNING_RAGE_FILENAME;
    static const QString SPELL_CARD_FIRE_FIREBALL_FILENAME;

    SpellCardStorage();
    ~SpellCardStorage();

    bool loadSpellCards();
    std::vector<SpellCard>& getSpellCards();

    private:
    std::vector<SpellCard> _spell_cards;

    QPixmap* _spell_card_air_back_image;
    QPixmap* _spell_card_water_back_image;
    QPixmap* _spell_card_earth_back_image;
    QPixmap* _spell_card_fire_back_image;

    QPixmap* _spell_card_air_genie_image;
    QPixmap* _spell_card_air_storm_image;
    QPixmap* _spell_card_air_tailwind_image;

    QPixmap* _spell_card_water_fog_image;
    QPixmap* _spell_card_water_morpheus_image;
    QPixmap* _spell_card_water_healing_potion_image;

    QPixmap* _spell_card_earth_granite_skin_image;
    QPixmap* _spell_card_earth_through_wall_image;
    QPixmap* _spell_card_earth_healing_magic_image;

    QPixmap* _spell_card_fire_braveness_image;
    QPixmap* _spell_card_fire_burning_rage_image;
    QPixmap* _spell_card_fire_fireball_image;
};

#endif
