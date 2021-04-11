#include "EquipmentCard.h"

#include <vector>

#include "StreamUtils.h"
#include "HeroQuestLevelWindow.h"
#include "Level.h"
#include "SoundManager.h"
#include "Debug.h"
#include "Hero.h"
#include "EquipmentCardStorage.h"

using namespace std;

EquipmentCard::EquipmentCard()
: InventoryItem(),
  _equipment_id(EQUIPMENT_ID_UNKNOWN)
{
}

EquipmentCard::EquipmentCard(const EquipmentCard& other)
        :
        InventoryItem(), _equipment_id(other._equipment_id)
{
}

EquipmentCard::EquipmentCard(const EquipmentCard::EquipmentID& equipment_id)
: InventoryItem(),
  _equipment_id(equipment_id)
{
    // NIX
}

EquipmentCard::~EquipmentCard()
{
	// NIX
}

bool EquipmentCard::isEquipmentCard() const
{
    return true;
}

bool EquipmentCard::canBeUsed() const
{
    bool can_be_used = false;

    switch (_equipment_id)
    {
        case HAND_AXE:
        case LANCE:
        case TOOLBOX:
        {
            can_be_used = true;
        }
            break;

        case CLUB:
        case BATTLE_AXE:
        case SHORT_SWORD:
        case LONG_SWORD:
        case CROSSBOW:
        case ARMOR:
        case CHAIN_ARMOR:
        case HELMET:
        case SHIELD:
            break;

        default:
            DVX(("EquipmentCard::canBeUsed: undefined equipment id!"))
            ;
            break;
    }

    return can_be_used;
}

/*!
 * "Use" the special feature of the EquipmentCard.
 */
bool EquipmentCard::execute() const
{
    bool equipment_card_used = false;

    Hero* current_hero = HeroQuestLevelWindow::_hero_quest->getLevel()->getCurrentlyActingHero();

    switch (_equipment_id)
    {
        case HAND_AXE:
        case LANCE:
        {
            // --- throw in horizontal or vertical line of sight ---

            // user selects the opponent
            Creature* creature =
                    HeroQuestLevelWindow::_hero_quest->userSelectsCreatureInHorizontalOrVerticalLineOfSightOf(
                            *current_hero);
            if (creature == 0)
                return false;
            DVX(("chosen creature for hand axe or lance throw attack: %s", qPrintable(creature->getName())));

            equipment_card_used = true;

            HeroQuestLevelWindow::_hero_quest->getLevel()->attackCreatureWithSkulls(current_hero, 2, creature);
        }
            break;

        case TOOLBOX:
        {
            // user selects an adjacent pit trap
            PitTrap* pit_trap = HeroQuestLevelWindow::_hero_quest->userSelectsAdjacentPitTrap(*current_hero);
            if (pit_trap == 0)
                return false;
            DVX(("chose pit trap for toolbox disarm"));
            HeroQuestLevelWindow::_hero_quest->getLevel()->disarmPitTrapUsingToolbox(current_hero, pit_trap);
        }
            break;

        case CLUB:
        case BATTLE_AXE:
        case SHORT_SWORD:
        case LONG_SWORD:
        case CROSSBOW:
        case ARMOR:
        case CHAIN_ARMOR:
        case HELMET:
        case SHIELD:
            DVX(("EquipmentCard::execute: Should not be called for equipment id %d!", uint(_equipment_id)))
            break;

        default:
            DVX(("EquipmentCard::execute: Undefined equipment id!"))
            ;
            break;
    }

    return equipment_card_used;
}

bool EquipmentCard::operator<(const EquipmentCard& other) const
{
    return _equipment_id < other._equipment_id;
}

EquipmentCard& EquipmentCard::operator=(const EquipmentCard& other)
{
    _equipment_id = other._equipment_id;
    return *this;
}

void EquipmentCard::setEquipmentID(const EquipmentCard::EquipmentID& equipment_id)
{
    _equipment_id = equipment_id;
}

EquipmentCard::EquipmentID EquipmentCard::getEquipmentID() const
{
    return _equipment_id;
}

QString EquipmentCard::getTitle() const
{
    QString title("UNDEFINED");
    switch (_equipment_id)
    {
        // attack
        case CLUB:
        {
            title = "Club";
        }
            break;

        case HAND_AXE:
        {
            title = "Hand Axe";
        }
            break;

        case BATTLE_AXE:
        {
            title = "Battle Axe";
        }
            break;

        case SHORT_SWORD:
        {
            title = "Short Sword";
        }
            break;
        
        case LONG_SWORD:
        {
            title = "Long Sword";
        }
            break;
        
        case LANCE:
        {
            title = "Lance";
        }
            break;
        
        case CROSSBOW:
        {
            title = "Crossbow";
        }
            break;

            // defend
        case ARMOR:
        {
            title = "Armor";
        }
            break;

        case CHAIN_ARMOR:
        {
            title = "Chain Armor";
        }
            break;

        case HELMET:
        {
            title = "Helmet";
        }
            break;

        case SHIELD:
        {
            title = "Shield";
        }
            break;

            // tools
        case TOOLBOX:
        {
            title = "Toolbox";
        }
            break;

        default:
            DVX(("EquipmentCard::getTitle: Undefined equipment id!"))
            ;
            break;
    }
    return title;
}

QString EquipmentCard::getDescription() const
{
    QString description("UNDEFINED");
    switch (_equipment_id)
    {
        // attack
        case CLUB:
        {
            description = "Use can use the club to attack your opponent on a diagonally-adjacent field "
                    "with 1 attack die.";
        }
            break;

        case HAND_AXE:
        {
            description = "You can throw the hand axe in horizontal or vertical line of sight to hurt an "
                    "opponent with 2 attack dice. But after throwing, the hand axe is gone. Must not be used "
                    "by the Magician.";
        }
            break;

        case BATTLE_AXE:
        {
            description = "Both hands are needed to hold the battle axe when fighting; therefore you cannot "
                    "wear a shield, but you can attack with 4 dice. Must not be used by the Magician.";
        }
            break;

        case SHORT_SWORD:
        {
            description = "With the short sword you can attack opponents on diagonally-adjacent fields "
                    "with 2 attack dice. Must not be used by the Magician.";
        }
            break;

        case LONG_SWORD:
        {
            description = "You can attack with 3 dice. Must not be used by the Magician.";
        }
            break;

        case LANCE:
        {
            description = "You can use the lance to attack your opponent on a diagonally-adjacent field "
                    "with 2 dice. You can even throw it in horizontal or vertical line of sight, "
                    "but then you lose it. Must not be used by the Magician.";
        }
            break;

        case CROSSBOW:
        {
            description = "With the crossbow you can attack opponents in horizontal or vertical line of sight with "
                    "3 dice. You cannot use it to attack opponents on directly adjecent fields though. Must not "
                    "be used by the Magician.";
        }
            break;

            // defend
        case ARMOR:
        {
            description = "This heavy armor allows you to throw 4 dice on a defend. But you can move only with "
                    "1 movement die. Must not be used by the Magician.";
        }
            break;

        case CHAIN_ARMOR:
        {
            description = "Wearing the chain armor you can defend with 3 dice. Must not be used by the Magician.";
        }
            break;

        case HELMET:
        {
            description = "The helmet increases your defend dice by 1. Must not be used by the Magician.";
        }
            break;

        case SHIELD:
        {
            description = "The shield increases your defend dice by 1. Must not be used by the Magician.";
        }
            break;

            // tools
        case TOOLBOX:
        {
            description = "You can use the toolbox to disarm already found traps. When using it, "
                    "you roll a movement die. If you get a 1, you lose 1 life point.";
        }
            break;
        default:
            DVX(("EquipmentCard::getDescription: Undefined equipment id!"))
            ;
            break;
    }
    return description;
}

int EquipmentCard::getCost() const
{
    int cost = 0;
    switch (_equipment_id)
    {
        // attack
        case CLUB:
        {
            cost = 100;
        }
            break;

        case HAND_AXE:
        {
            cost = 150;
        }
            break;

        case BATTLE_AXE:
        {
            cost = 400;
        }
            break;

        case SHORT_SWORD:
        {
            cost = 150;
        }
            break;

        case LONG_SWORD:
        {
            cost = 250;
        }
            break;

        case LANCE:
        {
            cost = 150;
        }
            break;

        case CROSSBOW:
        {
            cost = 350;
        }
            break;

            // defend
        case ARMOR:
        {
            cost = 850;
        }
            break;

        case CHAIN_ARMOR:
        {
            cost = 450;
        }
            break;

        case HELMET:
        {
            cost = 120;
        }
            break;

        case SHIELD:
        {
            cost = 100;
        }
            break;

            // tools
        case TOOLBOX:
        {
            cost = 250;
        }
            break;

        default:
            DVX(("EquipmentCard::getTitle: Undefined equipment id!"))
            ;
            break;
    }
    return cost;
}

bool EquipmentCard::canBeUsedByHero(const Hero* hero) const
{
    const Magician* magician = dynamic_cast<const Magician*>(hero);
    if (magician != 0)
    {
        // Magician: only club may be used
        return _equipment_id == CLUB;
    }

    // all other heroes may use everything
    return true;
}

QPixmap* EquipmentCard::getBackImage() const
{
    return EquipmentCardStorage::instance->getEquipmentCardBackImage();
}

QPixmap* EquipmentCard::getImage() const
{
    return EquipmentCardStorage::instance->getEquipmentCardImage(_equipment_id);
}

bool EquipmentCard::save(std::ostream& stream) const
{
    Parent::save(stream);

    StreamUtils::writeUInt(stream, uint(_equipment_id));

    return !stream.fail();
}

bool EquipmentCard::load(std::istream& stream)
{
    Parent::load(stream);

    uint equipment_id_uint = 0;
    StreamUtils::readUInt(stream, &equipment_id_uint);
    _equipment_id = EquipmentID(equipment_id_uint);

    return !stream.fail();
}

int EquipmentCard::getNumAttackDiceForRegularAttack() const
{
    int num_dice = 0;
    switch (_equipment_id)
    {
        // attack
        case CLUB:
        {
            num_dice = 1;
        }
            break;

        case HAND_AXE:
        {
            num_dice = 2;
        }
            break;

        case BATTLE_AXE:
        {
            num_dice = 4;
        }
            break;

        case SHORT_SWORD:
        {
            num_dice = 2;
        }
            break;

        case LONG_SWORD:
        {
            num_dice = 3;
        }
            break;

        case LANCE:
        {
            num_dice = 2;
        }
            break;

        case CROSSBOW:
        {
            num_dice = 0; // distance attack is no regular attack
        }
            break;

            // defend
        case ARMOR:
        case CHAIN_ARMOR:
        case HELMET:
        case SHIELD:
            // tools
        case TOOLBOX:
        {
            num_dice = 0;
        }
            break;
        default:
            DVX(("EquipmentCard::getNumAttackDiceForRegularAttack: Undefined equipment id!"))
            ;
            break;
    }
    return num_dice;
}

/*!
 * @return the additional number of dice which can be used for defence due to this equipment card
 */
int EquipmentCard::getAdditionalNumDefendDice() const
{
    int additional_num_dice = 0;
    switch (_equipment_id)
    {
        // tools
        case TOOLBOX:
            // attack
        case CLUB:
        case HAND_AXE:
        case BATTLE_AXE:
        case SHORT_SWORD:
        case LONG_SWORD:
        case LANCE:
        case CROSSBOW:
        {
            additional_num_dice = 0;
        }
            break;

            // defend
        case ARMOR:
        {
            additional_num_dice = 2; // 2 regular + 2 equipment = 4 total
        }
            break;

        case CHAIN_ARMOR:
        {
            additional_num_dice = 1; // 2 regular + 1 equipment = 3 total
        }
            break;

        case HELMET:
        case SHIELD:
        {
            additional_num_dice = 1;
        }
            break;

        default:
            DVX(("EquipmentCard::getAdditionalNumDiceDefend: Undefined equipment id!"))
            ;
            break;
    }
    return additional_num_dice;
}
