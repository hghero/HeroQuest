#ifndef EQUIPMENT_CARD_H
#define EQUIPMENT_CARD_H

#include <iostream>

#include <QtCore/QString>
#include <QtWidgets/QWidget>

#include "InventoryItem.h"

class Hero;


/*!
 * Swords, shields etc.
 */
class EquipmentCard : public InventoryItem
{
public:
    typedef InventoryItem Parent;

    /*!
     * The EquipmentID defines which equipment this actually is, i.e. equipments are hard-coded here.
     */
    enum EquipmentID
    {
        // attack
        CLUB = 0,
        HAND_AXE,
        BATTLE_AXE,
        SHORT_SWORD,
        LONG_SWORD,
        LANCE,
        CROSSBOW,

        // defend
        ARMOR,
        CHAIN_ARMOR,
        HELMET,
        SHIELD,

        // tools
        TOOLBOX,

        EQUIPMENT_ID_UNKNOWN
    };

    EquipmentCard();
    EquipmentCard(const EquipmentCard& other);
    EquipmentCard(const EquipmentID& eqipment_id);
    virtual ~EquipmentCard();

    virtual bool isEquipmentCard() const;

    bool canBeUsed() const;

    bool execute() const;

    bool operator<(const EquipmentCard& other) const;
    EquipmentCard& operator=(const EquipmentCard& other);

    void setEquipmentID(const EquipmentCard::EquipmentID& equipment_id);
    EquipmentCard::EquipmentID getEquipmentID() const;

    QString getTitle() const;
    QString getDescription() const;
    int getCost() const;
    bool canBeUsedByHero(const Hero* hero) const;

    QPixmap* getBackImage() const;
    QPixmap* getImage() const;

    virtual bool save(std::ostream& stream) const;
    virtual bool load(std::istream& stream);

    int getNumAttackDiceForRegularAttack() const;
    int getAdditionalNumDefendDice() const;

    private:
    EquipmentCard::EquipmentID _equipment_id;
};

#endif
