#ifndef SPELL_CARD_STORAGE_H
#define SPELL_CARD_STORAGE_H

#include <vector>

#include <QtWidgets/QLabel>

#include "SpellCard.h"

class QPixmap;

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
    virtual ~SpellCardStorage();

    bool loadSpellCards();
    std::vector<SpellCard>& getSpellCardStock();
    std::vector<SpellCard>& getSpellCardDeposition();
    void removeSpellCardsFromStock(const std::vector<SpellCard>& spell_cards);

    QPixmap* getSpellCardBackImage(const SpellCard::SpellFamily& spell_family) const;
    QPixmap* getSpellCardImage(const SpellCard::SpellID& spell_id) const;

    virtual bool save(std::ostream& stream) const;
    virtual bool load(std::istream& stream);

    static SpellCardStorage* instance;

    private:
    void addSpellCardBackImageIfNotYetPresent(const SpellCard::SpellFamily& spell_family, const QString& filename);

    void addSpellCardImageIfNotYetPresent(const SpellCard::SpellID& spell_id, const QString& filename);

    std::vector<SpellCard> _spell_card_stock;
    std::vector<SpellCard> _spell_card_deposition;

    //! Spell card images
    std::map<SpellCard::SpellFamily, QPixmap*> _spell_card_back_images;
    std::map<SpellCard::SpellID, QPixmap*> _spell_card_images;
};

#endif
