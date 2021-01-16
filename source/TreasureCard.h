#ifndef TREASURE_CARD_H
#define TREASURE_CARD_H

#include "InventoryItem.h"
#include "TreasureDescription.h"

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

    virtual bool save(std::ostream& stream) const;
    virtual bool load(std::istream& stream);

    void updateTreasureImages();

protected:
	TreasureDescription _treasure_description;

private:
	static QString getEmbracedValue(const QString& action, QString::SplitBehavior split_behaviour = QString::SkipEmptyParts);
	static bool getValueOfGoldExpression(const QString& expr, uint* result_value);
	static bool interpretRoundExpression(const QString& expr, QString* command, uint* result_value);
};

#endif
