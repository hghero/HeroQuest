#ifndef INVENTORY_H
#define INVENTORY_H

#include <list>
#include <set>

#include "TreasureCard.h"
#include "SpellCard.h"
#include "TreasureDataTypes.h"
#include "EquipmentCard.h"

class SaveContext;
class LoadContext;

/*!
 * A hero's inventory (e.g. weapons, gold, treasure cards, spells).
 */
class Inventory
{
public:
	class ConstIterator
	{
		friend class Inventory;
	public:
		enum IteratorTarget {
			TREASURE_CARDS = 0,
			INVALID
		};

		ConstIterator(); // just to be able to declare an Iterator without any arguments

	private:
		ConstIterator(const Inventory* inventory); // called only by Inventory

	public:
		~ConstIterator();

		ConstIterator operator=(const ConstIterator& other);

		ConstIterator operator++(); // prefix
		ConstIterator operator++(int); // postfix

		ConstIterator operator--(); // prefix
		ConstIterator operator--(int); // postfix

		const TreasureCard* operator*();

		bool operator==(const ConstIterator& other);
		bool operator!=(const ConstIterator& other);

		const Inventory* getInventory() const;

	private:
		const Inventory* _inventory;
		IteratorTarget _iterator_target;
		std::set<TreasureCard>::const_iterator _treasure_card_iterator;
	};

	Inventory();
	virtual ~Inventory();

	int getGold() const;
	void setGold(int gold);
	void addGold(int add_gold);
    void subtractGold(int subtract_gold);

	const std::set<TreasureCard>& getTreasureCards() const;
    const TreasureCard* getTreasureCard(const TreasureDataTypes::TreasureImageID card_image_id);
	void addTreasureCard(const TreasureCard& treasure_card);
	void removeTreasureCard(const TreasureCard& treasure_card);

	const std::set<SpellCard>& getSpellCards() const;
	void addSpellCard(const SpellCard& spell_card);
    void addSpellCards(const std::vector<SpellCard>& spell_cards);
	void removeSpellCard(const SpellCard& spell_card);
    void removeSpellCards();

	bool hasResistancePotion() const;
	bool hasImmunizationPotion() const;
	bool hasHealingPotion() const;

    const std::set<EquipmentCard>& getEquipmentCards() const;
    bool addEquipmentCard(const EquipmentCard& equipment_card);
    bool containsEquipmentCard(const EquipmentCard& equipment_card) const;
    void removeEquipmentCard(const EquipmentCard& equipment_card);
    void removeEquipmentCards();

	ConstIterator begin() const;
	ConstIterator end() const;

	size_t size() const;
	bool empty() const;

    virtual bool save(SaveContext& save_context) const;
    virtual bool load(LoadContext& load_context);

    void updateTreasureImages();

private:
	int _gold;
	std::set<TreasureCard> _treasure_cards;
	std::set<SpellCard> _spell_cards;
    std::set<EquipmentCard> _equipment_cards; // there are identical cards, but in an inventory there are never identical cards

	// TODO: weapons, ...
};

#endif
