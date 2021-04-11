#include <iostream>
#include <algorithm>

#include "Hero.h"

#include "HeroQuestLevelWindow.h"
#include "Level.h"
#include "StreamUtils.h"
#include "SpellCardStorage.h"
#include "Playground.h"
#include "BoardGraph.h"
#include "QuestBoard.h"

using namespace std;

// ==================================

const QString Hero::ICON_FILENAME = "THIS_STRING_MAY_NOT_BE_USED";

Hero::Hero(const QString& name, int num_dice_move, int num_dice_attack, int num_dice_defend, int life_points,
        int life_points_max, int intelligence_points)
:
Creature(name, num_dice_attack, num_dice_defend, life_points, life_points_max, intelligence_points),
_num_dice_move(num_dice_move),
_num_dice_move_extra(0),
 _can_move_through_walls(false),
_num_dice_attack_extra(0),
_num_dice_defend_extra(0),
 _inventory(), _spell_families()
{
}


Hero::~Hero()
{
	; // nix
}

const QString& Hero::getIconFilename() const
{
	return ICON_FILENAME;
}

/*!
 * @return the current number of dice which can be used for movement
 */
int Hero::getNumDiceMove() const
{
    int num_dice = _num_dice_move;

    // having the ARMOR equipment card decreases the number of movement dice by 1
    EquipmentCard equipment_card_armor(EquipmentCard::ARMOR);
    if (_inventory.containsEquipmentCard(equipment_card_armor))
        num_dice -= 1;

    // extra
    num_dice += _num_dice_move_extra;

    return num_dice;
}

void Hero::setNumDiceMoveExtra(int num_dice_move_extra)
{
	_num_dice_move_extra = num_dice_move_extra;
}

bool Hero::getCanMoveThroughWalls() const
{
    return _can_move_through_walls;
}

void Hero::setCanMoveThroughWalls(bool value)
{
    _can_move_through_walls = value;
}

/*!
 * @return highest number of dice which can be used for a regular (no distance, no diagonal) attack, including
 * equipment, but WITHOUT respecting extra dice due to spells/treasure cards.
 */
int Hero::getHighestNumDiceRegularAttack() const
{
    return max(_num_dice_attack, computeHighestNumDiceForRegularAttackFromEquipment());
}

/*!
 * Computes the best (highest) possible numbers of dice used for attacking.
 *
 * @return The number of dice which can be used for the next non-regular attack (diagonally-adjacent, or distance combat),
 * including temporary extra dice gained from spells or treasure items.
 */
int Hero::getHighestNumDiceAttack(const Creature* defender) const
{
    const NodeID* attacker_node_id = HeroQuestLevelWindow::_hero_quest->getPlayground()->getCreaturePos(*this);
    const NodeID* defender_node_id = HeroQuestLevelWindow::_hero_quest->getPlayground()->getCreaturePos(*defender);

    // check regular attack, including equipment
    int num_attack_dice = 0;
    if (HeroQuestLevelWindow::_hero_quest->getPlayground()->getQuestBoard()->getBoardGraph()->isEdge(*attacker_node_id,
            *defender_node_id))
    {
        num_attack_dice = getHighestNumDiceRegularAttack();
    }

    // check diagonally-adjacent attack
    if (HeroQuestLevelWindow::_hero_quest->getPlayground()->getQuestBoard()->getBoardGraph()->isCorner(
            *attacker_node_id, *defender_node_id))
    {
        num_attack_dice = max(num_attack_dice, getNumAttackDiceDiagonallyAdjacent());
    }

    // check distance attack
    if (HeroQuestLevelWindow::_hero_quest->getPlayground()->getQuestBoard()->fieldCanBeViewedFromField(
            *attacker_node_id, *defender_node_id, true /*respect_field2_borders*/)
            && HeroQuestLevelWindow::_hero_quest->getPlayground()->getQuestBoard()->nodesAreInSameRowOrColumn(
                    *attacker_node_id, *defender_node_id))
    {
        num_attack_dice = max(num_attack_dice, getNumAttackDiceDistance());
    }

    // extra
    num_attack_dice += _num_dice_attack_extra;

    return num_attack_dice;
}

void Hero::setNumDiceAttackExtra(int num_dice_attack_extra)
{
    _num_dice_attack_extra = num_dice_attack_extra;
}

/*!
 * @return The number of temporary attack dice gained due to spells or treasure items.
 */
int Hero::getNumDiceAttackExtra() const
{
    return _num_dice_attack_extra;
}

int Hero::getNumDiceDefend() const
{
    return _num_dice_defend + _num_dice_defend_extra;
}

/*!
 * @return The currently best / highest number of dice usable for defence, also respecting equipment, spells
 * and treasure cards.
 */
int Hero::getHighestNumDiceDefend() const
{
    int num_dice = _num_dice_defend;

    // equipment cards owned by the Hero
    num_dice += computeAdditionalNumDiceForDefenceFromEquipment();

    // spells / treasure cards
    num_dice += _num_dice_defend_extra;

    return num_dice;
}

void Hero::setNumDiceDefendExtra(int num_dice_defend_extra)
{
    _num_dice_defend_extra = num_dice_defend_extra;
}

// deprecated
int Hero::getGold() const
{
	return _inventory.getGold();
}

// deprecated
void Hero::setGold(int gold)
{
	_inventory.setGold(gold);
}

Inventory& Hero::getInventory()
{
	return _inventory;
}

const Inventory& Hero::getInventory() const
{
	return _inventory;
}

bool Hero::isHero() const
{
	return true;
}

bool Hero::isMonster() const
{
	return false;
}

bool Hero::canCastSpells() const
{
    return false;
}

/*!
 * This is called when a spell is applied on the Hero. If the hero has a RESISTANCE_POTION treasure card,
 * he is asked whether he wants to use it and this render the spell ineffective.
 */
bool Hero::checkApplySpell()
{
    return HeroQuestLevelWindow::_hero_quest->checkApplySpell(this);
}

/*!
 * This is called when the Hero defends himself. If he has an IMMUNIZATION_POTION treasure card,
 * he is asked if he wants to drink it before throwing the defend dice.
 *
 * \return Always true.
 */
bool Hero::checkDefend()
{
    return HeroQuestLevelWindow::_hero_quest->checkDefend(this);
}

/*!
 * \return True if the hero really dies, false otherwise.
 */
bool Hero::checkDeath()
{
    return HeroQuestLevelWindow::_hero_quest->checkDeath(this);
}

//! It is forbidden to copy Heroes.
Creature& Hero::operator=(const Hero& other)
{
	return *this;
}

//! It is forbidden to copy Heroes.
Creature* Hero::copy() const
{
	cout << "Error: Hero::copy() called" << endl;
	return 0;
}

bool Hero::save(ostream& stream) const
{
    Parent::save(stream);

    // movement
    StreamUtils::writeInt(stream, _num_dice_move);
    StreamUtils::writeInt(stream, _num_dice_move_extra);
    StreamUtils::writeBool(stream, _can_move_through_walls);

    // attack
    StreamUtils::writeInt(stream, _num_dice_attack_extra);

    // defend
    StreamUtils::writeInt(stream, _num_dice_defend_extra);

    // equipment and valuables
    _inventory.save(stream);

    // spells
    StreamUtils::writeUInt(stream, _spell_families.size());
    for (uint i = 0; i < _spell_families.size(); ++i)
    {
        uint spell_family_uint = (uint) (_spell_families[i]);
        StreamUtils::writeUInt(stream, spell_family_uint);
    }

    return !stream.fail();
}

bool Hero::load(istream& stream)
{
    Parent::load(stream);

    // movement
    StreamUtils::readInt(stream, &_num_dice_move);
    StreamUtils::readInt(stream, &_num_dice_move_extra);
    StreamUtils::readBool(stream, &_can_move_through_walls);

    // attack
    StreamUtils::readInt(stream, &_num_dice_attack_extra);

    // defend
    StreamUtils::readInt(stream, &_num_dice_defend_extra);

    // equipment and valuables
    _inventory.load(stream);

    // spells
    uint num_spell_families;
    StreamUtils::readUInt(stream, &num_spell_families);
    _spell_families.clear();
    for (uint i = 0; i < num_spell_families; ++i)
    {
        uint spell_family_uint;
        StreamUtils::readUInt(stream, &spell_family_uint);

        _spell_families.push_back((SpellCard::SpellFamily) spell_family_uint);
    }

    return !stream.fail();
}

/*!
 * Called after save game loading: updates treasure card images in
 * std::set<TreasureCard> _treasure_cards (in inventory).
 */
void Hero::updateTreasureImages()
{
    _inventory.updateTreasureImages();
}

void Hero::addSpellFamily(SpellCard::SpellFamily family)
{
    _spell_families.push_back(family);
}

void Hero::obtainSpellCardsFromStorage()
{
    vector<SpellCard>& global_spell_card_stock = SpellCardStorage::instance->getSpellCardStock();
    vector<SpellCard> spell_cards_to_move;
    for (vector<SpellCard>::iterator it = global_spell_card_stock.begin(); it != global_spell_card_stock.end(); ++it)
    {
        SpellCard& spell_card = *it;
        for (uint i = 0; i < _spell_families.size(); ++i)
        {
            if (spell_card.getSpellFamily() == _spell_families[i])
            {
                spell_cards_to_move.push_back(spell_card); // copy
            }
        }
    }

    // remove from stock; move to inventory
    SpellCardStorage::instance->removeSpellCardsFromStock(spell_cards_to_move);
    _inventory.addSpellCards(spell_cards_to_move);
}

/*!
 * @return 2 if the inventory of this hero contains the hand axe or the lance; 1 if the inventory
 * of this hero contains the club; 0 otherwise.
 */
int Hero::getNumAttackDiceDiagonallyAdjacent() const
{
    EquipmentCard equipment_card_hand_axe(EquipmentCard::HAND_AXE);
    EquipmentCard equipment_card_lance(EquipmentCard::LANCE);
    EquipmentCard equipment_card_club(EquipmentCard::CLUB);

    if (_inventory.containsEquipmentCard(equipment_card_hand_axe)
            || _inventory.containsEquipmentCard(equipment_card_lance))
    {
        return 2;
    }
    else if (_inventory.containsEquipmentCard(equipment_card_club))
    {
        return 1;
    }
    return 0;
}

/*!
 * @return 3 if the inventory of this hero contains the crossbow; 0 otherwise.
 */
int Hero::getNumAttackDiceDistance() const
{
    EquipmentCard equipment_card_crossbow(EquipmentCard::CROSSBOW);

    if (_inventory.containsEquipmentCard(equipment_card_crossbow))
    {
        return 3;
    }
    return 0;
}

/*!
 * @return the highest ABSOLUTE number of dice which can be used for a regular (non-distance, non-diagonal) attack
 * due to equipment cards currently owned by the Hero
 */
int Hero::computeHighestNumDiceForRegularAttackFromEquipment() const
{
    int num_dice = 0;
    set<EquipmentCard> equipment_cards = _inventory.getEquipmentCards();
    for (set<EquipmentCard>::const_iterator it = equipment_cards.begin(); it != equipment_cards.end(); ++it)
    {
        const EquipmentCard& equipment_card = *it;
        num_dice = max(num_dice, equipment_card.getNumAttackDiceForRegularAttack());
    }
    return num_dice;
}

/*!
 * @return the highest ADDITIONAL number of defend dice due to equipment cards owned by the Hero.
 */
int Hero::computeAdditionalNumDiceForDefenceFromEquipment() const
{
    int additional_num_dice = 0;
    set<EquipmentCard> equipment_cards = _inventory.getEquipmentCards();
    for (set<EquipmentCard>::const_iterator it = equipment_cards.begin(); it != equipment_cards.end(); ++it)
    {
        const EquipmentCard& equipment_card = *it;
        additional_num_dice = max(additional_num_dice, equipment_card.getAdditionalNumDefendDice());
    }
    return additional_num_dice;
}

// ==================================

const QString Barbarian::ICON_FILENAME = ":/graphics/barbarian.jpg";

Barbarian::Barbarian()
:
Hero(
	Barbarian::className(),
	2, // num_dice_move
	3, // num_dice_attack
	2, // num_dice_defend
	8, // life_points
	8, // life_points_max
                2) // intelligence_points
{
}

Barbarian::~Barbarian()
{
}

const QString& Barbarian::getIconFilename() const
{
	return ICON_FILENAME;
}

QString Barbarian::className()
{
    return "Barbarian";
}

// ==================================

const QString Dwarf::ICON_FILENAME = ":/graphics/dwarf.jpg";

        Dwarf::Dwarf()
:
Hero(
	Dwarf::className(),
	2, // num_dice_move
	2, // num_dice_attack
	2, // num_dice_defend
	7, // life_points
	7, // life_points_max
                3) // intelligence_points
{
}

Dwarf::~Dwarf()
{
}

const QString& Dwarf::getIconFilename() const
{
	return ICON_FILENAME;
}

QString Dwarf::className()
{
    return "Dwarf";
}

// ==================================

const QString Alb::ICON_FILENAME = ":/graphics/alb.jpg";

                Alb::Alb()
:
Hero(
	Alb::className(),
	2, // num_dice_move
	2, // num_dice_attack
	2, // num_dice_defend
	6, // life_points
	6, // life_points_max
                4) // intelligence_points
{
}

Alb::~Alb()
{
}

const QString& Alb::getIconFilename() const
{
	return ICON_FILENAME;
}

bool Alb::canCastSpells() const
{
	return true;
}

QString Alb::className()
{
    return "Alb";
}

// ==================================

const QString Magician::ICON_FILENAME = ":/graphics/magician.jpg";

                Magician::Magician()
:
Hero(
	Magician::className(),
	2, // num_dice_move
	1, // num_dice_attack
	2, // num_dice_defend
	4, // life_points
	4, // life_points_max
                                6) // intelligence_points
{
}

Magician::~Magician()
{
}

const QString& Magician::getIconFilename() const
{
	return ICON_FILENAME;
}

bool Magician::canCastSpells() const
{
	return true;
}

QString Magician::className()
{
    return "Magician";
}

// ==================================
