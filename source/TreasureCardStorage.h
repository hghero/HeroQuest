#ifndef TREASURE_CARD_STORAGE_H
#define TREASURE_CARD_STORAGE_H

#include <vector>

#include <QtWidgets/QLabel>

#include "TreasureCard.h"
#include "TreasureDataTypes.h"

class QPixmap;

/*!
 * Needed to deal with TreasureCards before and after a level.
 */
class TreasureCardStorage
{
    public:
    //! treasure card image filenames
    static const QString TREASURE_CARD_BACK_FILENAME;
    static const QString TREASURE_CARD_BRAVENESS_POTION_FILENAME;
    static const QString TREASURE_CARD_GEMSTONE_FILENAME;
    static const QString TREASURE_CARD_GOLD_FILENAME;
    static const QString TREASURE_CARD_GOLD_TREASURE_FILENAME;
    static const QString TREASURE_CARD_HEALING_POTION_FILENAME;
    static const QString TREASURE_CARD_HOLY_WATER_FILENAME;
    static const QString TREASURE_CARD_IMMUNIZATION_POTION_FILENAME;
    static const QString TREASURE_CARD_JEWELS_FILENAME;
    static const QString TREASURE_CARD_MAGIC_POTION_FILENAME;
    static const QString TREASURE_CARD_NOTHING_FILENAME;
    static const QString TREASURE_CARD_PHILOSOPHERS_STONE_FILENAME;
    static const QString TREASURE_CARD_POISON_FILENAME;
    static const QString TREASURE_CARD_POWER_POTION_FILENAME;
    static const QString TREASURE_CARD_RESISTANCE_POTION_FILENAME;
    static const QString TREASURE_CARD_ROAMING_MONSTER_FILENAME;
    static const QString TREASURE_CARD_SPRINT_POTION_FILENAME;
    static const QString TREASURE_CARD_TRAP_PIT_FILENAME;
    static const QString TREASURE_CARD_TRAP_WALL_FILENAME;
    static const QString TREASURE_CHEST_3D_FILENAME;

    TreasureCardStorage();
    virtual ~TreasureCardStorage();

    bool loadTreasureCards();
    std::vector<TreasureCard>& getTreasureCards();

    QPixmap* getTreasureCardBackImage() const;
    QPixmap* getTreasureCardImage(const TreasureDataTypes::TreasureImageID& id) const;
    QPixmap* getChest3DImage() const;

    virtual bool save(std::ostream& stream) const;
    virtual bool load(std::istream& stream);

    static TreasureCardStorage* instance;

    private:
    bool loadPixmap(const QString& filename, QPixmap** pixmap);
    bool addTreasureCardImageIfNotYetPresent(const QString& filename,
            TreasureDataTypes::TreasureImageID treasure_image_id);

    std::vector<TreasureCard> _treasure_cards;
    QPixmap* _treasure_card_back_image;
    QPixmap* _chest_3d_image;
    std::map<TreasureDataTypes::TreasureImageID, QPixmap*> _treasure_card_images;
};

#endif
