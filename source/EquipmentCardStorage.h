#ifndef EQUIPMENT_CARD_STORAGE_H
#define EQUIPMENT_CARD_STORAGE_H

#include <vector>

#include <QtWidgets/QLabel>

#include "EquipmentCard.h"

class QPixmap;

/*!
 * Needed to deal with EquipmentCards before and after the levels.
 */
class EquipmentCardStorage
{
    public:
    //! equipment card image filenames
    static const QString EQUIPMENT_CARD_BACK_FILENAME;
    static const QString EQUIPMENT_CARD_ARMOR_FILENAME;
    static const QString EQUIPMENT_CARD_BATTLE_AXE_FILENAME;
    static const QString EQUIPMENT_CARD_CHAIN_ARMOR_FILENAME;
    static const QString EQUIPMENT_CARD_CLUB_FILENAME;
    static const QString EQUIPMENT_CARD_CROSSBOW_FILENAME;
    static const QString EQUIPMENT_CARD_HAND_AXE_FILENAME;
    static const QString EQUIPMENT_CARD_HELMET_FILENAME;
    static const QString EQUIPMENT_CARD_LANCE_FILENAME;
    static const QString EQUIPMENT_CARD_LONG_SWORD_FILENAME;
    static const QString EQUIPMENT_CARD_SHIELD_FILENAME;
    static const QString EQUIPMENT_CARD_SHORT_SWORD_FILENAME;
    static const QString EQUIPMENT_CARD_TOOLBOX_FILENAME;

    EquipmentCardStorage();
    virtual ~EquipmentCardStorage();

    bool loadEquipmentCards();
    std::vector<EquipmentCard>& getEquipmentCardStock();
    void addEquipmentCardToStock(const EquipmentCard& equipment_card);
    void removeEquipmentCardFromStock(const EquipmentCard& equipment_card);

    QPixmap* getEquipmentCardBackImage() const;
    QPixmap* getEquipmentCardImage(const EquipmentCard::EquipmentID& equipment_id) const;

    virtual bool save(std::ostream& stream) const;
    virtual bool load(std::istream& stream);

    static EquipmentCardStorage* instance;

    private:
    void addEquipmentCardBackImageIfNotYetPresent(const QString& filename);
    void addEquipmentCardImageIfNotYetPresent(const EquipmentCard::EquipmentID& equipment_id, const QString& filename);
    void sortEquipmentCardStock();

    std::vector<EquipmentCard> _equipment_card_stock;

    //! Equipment card images
    QPixmap* _equipment_card_back_image;
    std::map<EquipmentCard::EquipmentID, QPixmap*> _equipment_card_images;
};

#endif
