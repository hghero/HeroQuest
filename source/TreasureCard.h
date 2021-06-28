#ifndef TREASURE_CARD_H
#define TREASURE_CARD_H

#include "InventoryItem.h"
#include "TreasureDescription.h"

class Hero;
class SaveContext;
class LoadContext;

/*!
 * An item storable in a hero's inventory.
 */
class TreasureCard : public InventoryItem
{
public:
    typedef InventoryItem Parent;

    enum Characteristic {
    GOOD_CHARACTERISTIC = 0,
    BAD_CHARACTERISTIC
    };

    TreasureCard();
	TreasureCard(const TreasureDescription& treasure_card_description);
	virtual ~TreasureCard();

	const TreasureDescription& getTreasureDescription() const;

	TreasureCard& operator=(const TreasureCard& other);

	void executeImmediateActions() const;

	bool executeInventoryActions(Hero* user = 0) const;

	bool operator<(const TreasureCard& other) const;

	virtual bool isTreasureCard() const;

	bool isApplyableFromInventory() const;

	TreasureCard::Characteristic getCharacteristic() const;

    virtual bool save(SaveContext& save_context) const;
    virtual bool load(LoadContext& load_context);

    void updateTreasureImages();

protected:
	TreasureDescription _treasure_description;
};

#endif
