#ifndef SPELL_CARD_H
#define SPELL_CARD_H

#include <iostream>

#include <QtCore/QString>
#include <QtWidgets/QWidget>

#include "InventoryItem.h"

/*!
 * Spells: Fire, Earth, Water, Air.
 */
class SpellCard : public InventoryItem
{
public:
    typedef InventoryItem Parent;

    /*!
     * The SpellID defines which spell this actually is, i.e. spells are hard-coded here.
     */
    enum SpellID {
    // AIR
    GENIE = 0,
    STORM,
    TAILWIND,

    // WATER
    FOG,
    MORPHEUS,
    HEALING_POTION,

    // EARTH
    GRANITE_SKIN,
    THROUGH_WALL,
    HEALING_MAGIC,

    // FIRE
    BRAVENESS,
    BURNING_RAGE,
    FIREBALL,

    UNKNOWN
    };

    enum SpellFamily {
    AIR = 0,
    WATER,
    EARTH,
    FIRE,

        SPELL_FAMILY_UNKNOWN
    };

    SpellCard();
    SpellCard(const SpellID& spell_id);
	virtual ~SpellCard();

    virtual bool isSpellCard() const;

	bool operator<(const SpellCard& other) const;
	SpellCard& operator=(const SpellCard& other);

	void setSpellID(const SpellCard::SpellID& spell_id);
	SpellCard::SpellID getSpellID() const;

    SpellCard::SpellFamily getSpellFamily() const;

    QString getTitle() const;
    QString getDescription() const;

    QPixmap* getBackImage() const;
    QPixmap* getImage() const;

    bool execute() const;

    virtual bool save(std::ostream& stream) const;
    virtual bool load(std::istream& stream);

private:
    SpellCard::SpellID _spell_id;
};

#endif
