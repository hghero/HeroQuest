#include "Inventory.h"
#include "Debug.h"
#include "StreamUtils.h"
#include "SaveContext.h"
#include "LoadContext.h"

using namespace std;

Inventory::ConstIterator::ConstIterator()
:
_inventory(0),
_iterator_target(INVALID),
_treasure_card_iterator()
{
	// NIX
}

Inventory::ConstIterator::ConstIterator(const Inventory* inventory)
:
_inventory(inventory),
_iterator_target(TREASURE_CARDS),
_treasure_card_iterator(inventory->getTreasureCards().begin())
{
	if (_treasure_card_iterator == inventory->getTreasureCards().end())
		_iterator_target = INVALID;
}

Inventory::ConstIterator::~ConstIterator()
{
	// NIX
}

Inventory::ConstIterator Inventory::ConstIterator::operator=(const Inventory::ConstIterator& other)
{
	_iterator_target = other._iterator_target;
	_treasure_card_iterator = other._treasure_card_iterator;
	return *this;
}

// prefix
Inventory::ConstIterator Inventory::ConstIterator::operator++()
{
	switch (_iterator_target)
	{
	case TREASURE_CARDS:
		++_treasure_card_iterator;
		if (_treasure_card_iterator == _inventory->getTreasureCards().end())
			_iterator_target = INVALID;
		break;
	case INVALID:
		cout << "Error: operator++ called on Inventory::Iterator, although Iterator is INVALID!" << endl;
		break;
	}

	return *this;
}

// postfix
Inventory::ConstIterator Inventory::ConstIterator::operator++(int)
{
	ConstIterator old = *this;
	++(*this);
	return old;
}

// prefix
Inventory::ConstIterator Inventory::ConstIterator::operator--()
{
	switch (_iterator_target)
	{
	case TREASURE_CARDS:
		if (_treasure_card_iterator == _inventory->getTreasureCards().begin())
			_iterator_target = INVALID;
		else
			--_treasure_card_iterator;
		break;
	case INVALID:
		cout << "Error: operator-- called on Inventory::Iterator, although Iterator is INVALID!" << endl;
		break;
	}

	return *this;
}

// postfix
Inventory::ConstIterator Inventory::ConstIterator::operator--(int)
{
	ConstIterator old = *this;
	--(*this);
	return old;
}

const TreasureCard* Inventory::ConstIterator::operator*()
{
	switch (_iterator_target)
	{
	case TREASURE_CARDS:
		return &(*_treasure_card_iterator);
		break;
	case INVALID:
		cout << "Error: operator* called on Inventory::Iterator, although Iterator is INVALID!" << endl;
		break;
	}
	return 0;
}

bool Inventory::ConstIterator::operator==(const Inventory::ConstIterator& other)
{
	if (_iterator_target != other._iterator_target)
		return false;

	if (_iterator_target == INVALID && other._iterator_target == INVALID)
		return true;

	switch (_iterator_target)
	{
	case TREASURE_CARDS:
		return _treasure_card_iterator == other._treasure_card_iterator;
		break;

	case INVALID:
		// this cannot happen
		break;
	}
	return false;
}

bool Inventory::ConstIterator::operator!=(const Inventory::ConstIterator& other)
{
	return !(*this == other);
}

const Inventory* Inventory::ConstIterator::getInventory() const
{
	return _inventory;
}

// -----------------------------------------------------------------------

Inventory::Inventory()
: _gold(0),
  _treasure_cards(),
  _spell_cards(), _equipment_cards()
{
	// NIX
}

Inventory::~Inventory()
{
	// NIX
}

int Inventory::getGold() const
{
	return _gold;
}

void Inventory::setGold(int gold)
{
	_gold = gold;
}

void Inventory::addGold(int add_gold)
{
	_gold += add_gold;
}

void Inventory::subtractGold(int subtract_gold)
{
    _gold -= subtract_gold;
}

const set<TreasureCard>& Inventory::getTreasureCards() const
{
	return _treasure_cards;
}

const TreasureCard* Inventory::getTreasureCard(const TreasureDataTypes::TreasureImageID card_image_id)
{
    for (set<TreasureCard>::const_iterator it = _treasure_cards.begin(); it != _treasure_cards.end(); ++it)
    {
        if (it->getTreasureDescription().getTreasureImageID() == card_image_id)
            return &(*it);
    }

    return 0;
}

void Inventory::addTreasureCard(const TreasureCard& treasure_card)
{
	_treasure_cards.insert(treasure_card);
}

void Inventory::removeTreasureCard(const TreasureCard& treasure_card)
{
	_treasure_cards.erase(treasure_card);
}

const set<SpellCard>& Inventory::getSpellCards() const
{
    return _spell_cards;
}

void Inventory::addSpellCard(const SpellCard& spell_card)
{
    _spell_cards.insert(spell_card);
}

void Inventory::addSpellCards(const vector<SpellCard>& spell_cards)
{
    for (vector<SpellCard>::const_iterator it = spell_cards.begin(); it != spell_cards.end(); ++it)
    {
        addSpellCard(*it);
    }
}

void Inventory::removeSpellCard(const SpellCard& spell_card)
{
    _spell_cards.erase(spell_card);
}

void Inventory::removeSpellCards()
{
    _spell_cards.clear();
}

bool Inventory::hasResistancePotion() const
{
    for (set<TreasureCard>::const_iterator it = _treasure_cards.begin(); it != _treasure_cards.end(); ++it)
    {
        if (it->getTreasureDescription().getTreasureImageID() == TreasureDataTypes::TREASURE_CARD_RESISTANCE_POTION)
            return true;
    }

    return false;
}

bool Inventory::hasImmunizationPotion() const
{
    for (set<TreasureCard>::const_iterator it = _treasure_cards.begin(); it != _treasure_cards.end(); ++it)
    {
        if (it->getTreasureDescription().getTreasureImageID() == TreasureDataTypes::TREASURE_CARD_IMMUNIZATION_POTION)
            return true;
    }

    return false;
}

bool Inventory::hasHealingPotion() const
{
    for (set<TreasureCard>::const_iterator it = _treasure_cards.begin(); it != _treasure_cards.end(); ++it)
    {
        if (it->getTreasureDescription().getTreasureImageID() == TreasureDataTypes::TREASURE_CARD_HEALING_POTION)
            return true;
    }

    return false;
}

const set<EquipmentCard>& Inventory::getEquipmentCards() const
{
    return _equipment_cards;
}

/*!
 * @param equipment_card
 * @return true if equipment_card was inserted into _equipment_cards, i.e. equipment_card didn't exist in that
 *         set already before
 */
bool Inventory::addEquipmentCard(const EquipmentCard& equipment_card)
{
    return _equipment_cards.insert(equipment_card).second;
}

bool Inventory::containsEquipmentCard(const EquipmentCard& equipment_card) const
{
    return _equipment_cards.find(equipment_card) != _equipment_cards.end();
}

void Inventory::removeEquipmentCard(const EquipmentCard& equipment_card)
{
_equipment_cards.erase(equipment_card);
}

void Inventory::removeEquipmentCards()
{
_equipment_cards.clear();
}

Inventory::ConstIterator Inventory::begin() const
{
	return ConstIterator(this);
}

Inventory::ConstIterator Inventory::end() const
{
	return ConstIterator();
}

size_t Inventory::size() const
{
    return _treasure_cards.size() + _spell_cards.size() + _equipment_cards.size();
}

bool Inventory::empty() const
{
    return size() == 0;
}

bool Inventory::save(SaveContext& save_context) const
{
    SaveContext::OpenChapter open_chapter(save_context, "Inventory");

    save_context.writeInt(_gold, "_gold");

    // treasure cards
    save_context.writeUInt(uint(_treasure_cards.size()), "_treasure_cards.size()");
    for (set<TreasureCard>::const_iterator it = _treasure_cards.begin(); it != _treasure_cards.end(); ++it)
    {
        it->save(save_context);
    }

    // spell cards
    save_context.writeUInt(uint(_spell_cards.size()), "_spell_cards.size()");
    for (set<SpellCard>::const_iterator it = _spell_cards.begin(); it != _spell_cards.end(); ++it)
    {
        it->save(save_context);
    }

    // equipment cards
    save_context.writeUInt(uint(_equipment_cards.size()), "_equipment_cards.size()");
    for (set<EquipmentCard>::const_iterator it = _equipment_cards.begin(); it != _equipment_cards.end(); ++it)
    {
        it->save(save_context);
    }

    return !save_context.fail();
}

bool Inventory::load(LoadContext& load_context)
{
    LoadContext::OpenChapter open_chapter(load_context, "Inventory");

    load_context.readInt(&_gold, "_gold");

    // treasure cards
    _treasure_cards.clear();
    uint treasure_cards_size;
    load_context.readUInt(&treasure_cards_size, "_treasure_cards.size()");
    for (uint i = 0; i < treasure_cards_size; ++i)
    {
        TreasureCard card;
        if (!card.load(load_context))
        {
            DVX(("Error loading TreasureCard!"));
            return false;
        }

        _treasure_cards.insert(card);
    }

    // spell cards
    _spell_cards.clear();
    uint spell_cards_size;
    load_context.readUInt(&spell_cards_size, "_spell_cards.size()");
    for (uint i = 0; i < spell_cards_size; ++i)
    {
        SpellCard spell_card;
        if (!spell_card.load(load_context))
        {
            DVX(("Error loading SpellCard!"));
            return false;
        }

        _spell_cards.insert(spell_card);
    }

    // equipment cards
    _equipment_cards.clear();
    uint equipment_cards_size;
    load_context.readUInt(&equipment_cards_size, "_equipment_cards.size()");
    for (uint i = 0; i < equipment_cards_size; ++i)
    {
        EquipmentCard equipment_card;
        if (!equipment_card.load(load_context))
        {
            DVX(("Error loading EquipmentCard!"));
            return false;
        }

        _equipment_cards.insert(equipment_card);
    }

    return !load_context.fail();
}

/*!
 * Called after save game loading: updates treasure card images in
 * std::set<TreasureCard> _treasure_cards.
 */
void Inventory::updateTreasureImages()
{
    for (set<TreasureCard>::iterator it = _treasure_cards.begin(); it != _treasure_cards.end(); ++it)
    {
        const TreasureCard& card_const = *it;
        const_cast<TreasureCard&>(card_const).updateTreasureImages(); // necessary, because set sorting depends on content of TreasureCard (but should not be harmful here)
    }
}
