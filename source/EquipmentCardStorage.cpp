#include "EquipmentCardStorage.h"

#include <iostream>
#include <algorithm>

#include <QtGui/QPixmap>

#include "StreamUtils.h"
#include "Debug.h"

using namespace std;

// equipment card image filenames
const QString EquipmentCardStorage::EQUIPMENT_CARD_BACK_FILENAME = ":/graphics/equipment_card_back.jpg";
const QString EquipmentCardStorage::EQUIPMENT_CARD_ARMOR_FILENAME = ":/graphics/equipment_card_armor.jpg";
const QString EquipmentCardStorage::EQUIPMENT_CARD_BATTLE_AXE_FILENAME = ":/graphics/equipment_card_battle_axe.jpg";
const QString EquipmentCardStorage::EQUIPMENT_CARD_CHAIN_ARMOR_FILENAME = ":/graphics/equipment_card_chain_armor.jpg";
const QString EquipmentCardStorage::EQUIPMENT_CARD_CLUB_FILENAME = ":/graphics/equipment_card_club.jpg";
const QString EquipmentCardStorage::EQUIPMENT_CARD_CROSSBOW_FILENAME = ":/graphics/equipment_card_crossbow.jpg";
const QString EquipmentCardStorage::EQUIPMENT_CARD_HAND_AXE_FILENAME = ":/graphics/equipment_card_hand_axe.jpg";
const QString EquipmentCardStorage::EQUIPMENT_CARD_HELMET_FILENAME = ":/graphics/equipment_card_helmet.jpg";
const QString EquipmentCardStorage::EQUIPMENT_CARD_LANCE_FILENAME = ":/graphics/equipment_card_lance.jpg";
const QString EquipmentCardStorage::EQUIPMENT_CARD_LONG_SWORD_FILENAME = ":/graphics/equipment_card_long_sword.jpg";
const QString EquipmentCardStorage::EQUIPMENT_CARD_SHIELD_FILENAME = ":/graphics/equipment_card_shield.jpg";
const QString EquipmentCardStorage::EQUIPMENT_CARD_SHORT_SWORD_FILENAME = ":/graphics/equipment_card_short_sword.jpg";
const QString EquipmentCardStorage::EQUIPMENT_CARD_TOOLBOX_FILENAME = ":/graphics/equipment_card_toolbox.jpg";

EquipmentCardStorage* EquipmentCardStorage::instance = 0;

EquipmentCardStorage::EquipmentCardStorage()
        :
        _equipment_card_stock(), _equipment_card_back_image(0), _equipment_card_images()
{
    instance = this;
}

EquipmentCardStorage::~EquipmentCardStorage()
{
    delete _equipment_card_back_image;

    for (map<EquipmentCard::EquipmentID, QPixmap*>::iterator it = _equipment_card_images.begin();
            it != _equipment_card_images.end();
            ++it)
    {
        delete it->second;
    }
}

void EquipmentCardStorage::sortEquipmentCardStock()
{
    std::sort(_equipment_card_stock.begin(), _equipment_card_stock.end());
}

bool EquipmentCardStorage::loadEquipmentCards()
{
    // (1) images

    // _spell_card_back_image
    addEquipmentCardBackImageIfNotYetPresent(EQUIPMENT_CARD_BACK_FILENAME);
    if (_equipment_card_back_image == 0)
    {
        cout << "Equipment card back image not found or failed to load" << endl;
        return false;
    }

    // _equipment_card_images
    addEquipmentCardImageIfNotYetPresent(EquipmentCard::ARMOR, EQUIPMENT_CARD_ARMOR_FILENAME);
    addEquipmentCardImageIfNotYetPresent(EquipmentCard::BATTLE_AXE, EQUIPMENT_CARD_BATTLE_AXE_FILENAME);
    addEquipmentCardImageIfNotYetPresent(EquipmentCard::CHAIN_ARMOR, EQUIPMENT_CARD_CHAIN_ARMOR_FILENAME);
    addEquipmentCardImageIfNotYetPresent(EquipmentCard::CLUB, EQUIPMENT_CARD_CLUB_FILENAME);
    addEquipmentCardImageIfNotYetPresent(EquipmentCard::CROSSBOW, EQUIPMENT_CARD_CROSSBOW_FILENAME);
    addEquipmentCardImageIfNotYetPresent(EquipmentCard::HAND_AXE, EQUIPMENT_CARD_HAND_AXE_FILENAME);
    addEquipmentCardImageIfNotYetPresent(EquipmentCard::HELMET, EQUIPMENT_CARD_HELMET_FILENAME);
    addEquipmentCardImageIfNotYetPresent(EquipmentCard::LANCE, EQUIPMENT_CARD_LANCE_FILENAME);
    addEquipmentCardImageIfNotYetPresent(EquipmentCard::LONG_SWORD, EQUIPMENT_CARD_LONG_SWORD_FILENAME);
    addEquipmentCardImageIfNotYetPresent(EquipmentCard::SHIELD, EQUIPMENT_CARD_SHIELD_FILENAME);
    addEquipmentCardImageIfNotYetPresent(EquipmentCard::SHORT_SWORD, EQUIPMENT_CARD_SHORT_SWORD_FILENAME);
    addEquipmentCardImageIfNotYetPresent(EquipmentCard::TOOLBOX, EQUIPMENT_CARD_TOOLBOX_FILENAME);
    for (map<EquipmentCard::EquipmentID, QPixmap*>::const_iterator it = _equipment_card_images.begin();
            it != _equipment_card_images.end(); ++it)
    {
        if (it->second == 0)
        {
            cout << "Equipment card image for equipment ID " << uint(it->first) << " not found or failed to load"
                    << endl;
            return false;
        }
    }

    // (2) add equipment cards to _equipment_card_stock
    const uint NUM_EQUIPMENT_CARD_TYPES = uint(EquipmentCard::EQUIPMENT_ID_UNKNOWN);
    _equipment_card_stock.clear();
    _equipment_card_stock.reserve(NUM_EQUIPMENT_CARD_TYPES + 2); // +1 shield, +1 helmet
    for (uint i = 0; i < NUM_EQUIPMENT_CARD_TYPES; ++i)
    {
        EquipmentCard::EquipmentID equipment_id = (EquipmentCard::EquipmentID) i;
        EquipmentCard equipment_card(equipment_id);
        _equipment_card_stock.push_back(equipment_card);
    }
    // +1 shield, +1 helmet
    _equipment_card_stock.push_back(EquipmentCard(EquipmentCard::SHIELD));
    _equipment_card_stock.push_back(EquipmentCard(EquipmentCard::HELMET));

    sortEquipmentCardStock();

    return true;
}

void EquipmentCardStorage::addEquipmentCardBackImageIfNotYetPresent(const QString& filename)
{
    if (_equipment_card_back_image == 0)
        _equipment_card_back_image = new QPixmap(filename);
}

void EquipmentCardStorage::addEquipmentCardImageIfNotYetPresent(const EquipmentCard::EquipmentID& equipment_id,
        const QString& filename)
{
    if (_equipment_card_images.find(equipment_id) == _equipment_card_images.end())
        _equipment_card_images[equipment_id] = new QPixmap(filename);
}

vector<EquipmentCard>& EquipmentCardStorage::getEquipmentCardStock()
{
    return _equipment_card_stock;
}

/*!
 * @param spell_cards
 */
void EquipmentCardStorage::removeEquipmentCardFromStock(const EquipmentCard& equipment_card)
{
    for (vector<EquipmentCard>::iterator it_stock = _equipment_card_stock.begin();
            it_stock != _equipment_card_stock.end(); ++it_stock)
    {
        if (it_stock->getEquipmentID() == equipment_card.getEquipmentID())
        {
            _equipment_card_stock.erase(it_stock);
            break;
        }
    }
}

/*!
 * @param equipment_card
 * @return true iff added
 */
void EquipmentCardStorage::addEquipmentCardToStock(const EquipmentCard& equipment_card)
{
    _equipment_card_stock.push_back(equipment_card);
    sortEquipmentCardStock();
}

QPixmap* EquipmentCardStorage::getEquipmentCardBackImage() const
{
    return _equipment_card_back_image;
}

QPixmap* EquipmentCardStorage::getEquipmentCardImage(const EquipmentCard::EquipmentID& equipment_id) const
{
    map<EquipmentCard::EquipmentID, QPixmap*>::const_iterator it = _equipment_card_images.find(equipment_id);
    if (it == _equipment_card_images.end())
    {
        cout << "Error: equipment card ID " << uint(equipment_id) << " not found in _equipment_card_images map!"
                << endl;
        return 0;
    }
    return it->second;
}

bool EquipmentCardStorage::save(ostream& stream) const
{
    StreamUtils::writeUInt(stream, _equipment_card_stock.size());
    for (uint i = 0; i < _equipment_card_stock.size(); ++i)
    {
        if (!_equipment_card_stock[i].save(stream))
        {
            DVX(("Error saving card %d from equipment card stock!", i));
            return false;
        }
    }

    return true;
}

bool EquipmentCardStorage::load(istream& stream)
{
    _equipment_card_stock.clear();
    uint num_equipment_cards_stock;
    StreamUtils::readUInt(stream, &num_equipment_cards_stock);
    for (uint i = 0; i < num_equipment_cards_stock; ++i)
    {
        EquipmentCard equipment_card;
        if (!equipment_card.load(stream))
        {
            DVX(("Error loading equipment card %d to equipment card stock!", i));
            return false;
        }

        _equipment_card_stock.push_back(equipment_card);
    }

    return true;
}
