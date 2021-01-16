#include "Inventory.h"
#include "Debug.h"
#include "StreamUtils.h"

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
  _spell_cards()
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

const set<TreasureCard>& Inventory::getTreasureCards() const
{
	return _treasure_cards;
}

const TreasureCard* Inventory::getTreasureCard(const Playground::TreasureImageID card_image_id)
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

void Inventory::removeSpellCard(const SpellCard& spell_card)
{
    _spell_cards.erase(spell_card);
}

bool Inventory::hasResistancePotion() const
{
    for (set<TreasureCard>::const_iterator it = _treasure_cards.begin(); it != _treasure_cards.end(); ++it)
    {
        if (it->getTreasureDescription().getTreasureImageID() == Playground::TREASURE_CARD_RESISTANCE_POTION)
            return true;
    }

    return false;
}

bool Inventory::hasImmunizationPotion() const
{
    for (set<TreasureCard>::const_iterator it = _treasure_cards.begin(); it != _treasure_cards.end(); ++it)
    {
        if (it->getTreasureDescription().getTreasureImageID() == Playground::TREASURE_CARD_IMMUNIZATION_POTION)
            return true;
    }

    return false;
}

bool Inventory::hasHealingPotion() const
{
    for (set<TreasureCard>::const_iterator it = _treasure_cards.begin(); it != _treasure_cards.end(); ++it)
    {
        if (it->getTreasureDescription().getTreasureImageID() == Playground::TREASURE_CARD_HEALING_POTION)
            return true;
    }

    return false;
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
	return _treasure_cards.size() + _spell_cards.size();
}

bool Inventory::empty() const
{
    return size() == 0;
}

bool Inventory::save(std::ostream& stream) const
{
    StreamUtils::writeInt(stream, _gold);

    // treasure cards
    StreamUtils::writeUInt(stream, _treasure_cards.size());
    for (set<TreasureCard>::const_iterator it = _treasure_cards.begin(); it != _treasure_cards.end(); ++it)
    {
        it->save(stream);
    }

    // spell cards
    StreamUtils::writeUInt(stream, _spell_cards.size());
    for (set<SpellCard>::const_iterator it = _spell_cards.begin(); it != _spell_cards.end(); ++it)
    {
        it->save(stream);
    }

    return !stream.fail();
}

bool Inventory::load(std::istream& stream)
{
    StreamUtils::readInt(stream, &_gold);

    // treasure cards
    _treasure_cards.clear();
    uint treasure_cards_size;
    StreamUtils::readUInt(stream, &treasure_cards_size);
    for (uint i = 0; i < treasure_cards_size; ++i)
    {
        TreasureCard card;
        card.load(stream);

        _treasure_cards.insert(card);
    }

    // spell cards
    _spell_cards.clear();
    uint spell_cards_size;
    StreamUtils::readUInt(stream, &spell_cards_size);
    for (uint i = 0; i < spell_cards_size; ++i)
    {
        SpellCard spell_card;
        spell_card.load(stream);

        _spell_cards.insert(spell_card);
    }

    return !stream.fail();
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
