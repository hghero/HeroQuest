#ifndef DIALOGBUYEQUIPMENT_H
#define DIALOGBUYEQUIPMENT_H

#include <vector>
#include <QtWidgets/QDialog>
#include <QtGui/QPixmap>

class Hero;
class HeroCamp;
class QPushButton;
class QLabel;
class InventoryPane;
class EquipmentMarketPlacePane;

class DialogBuyEquipment : public QDialog
{
    Q_OBJECT

    friend class BuyItemButton;
    friend class UseInventoryItemButton;

    public:
    DialogBuyEquipment(HeroCamp& hero_camp);
    virtual ~DialogBuyEquipment();

    private slots:
    void hero0pressed();
    void hero1pressed();
    void hero2pressed();
    void hero3pressed();

    private:
    void initHeroAvatars();
    void updateContent();
    void updateInventory();
    void updateHeroButtonIcons();
    void updateHeroGoldLabelOfCurrentlyChosenHero();
    void updateEquipmentMarketPlace();

    static const int HERO_ICON_HEIGHT;

    std::vector<Hero*> _heroes;
    int _currently_chosen_hero_index;
    std::vector<QPixmap> _hero_avatars_color;
    std::vector<QPixmap> _hero_avatars_gray;
    std::vector<QPushButton*> _hero_buttons;
    std::vector<QLabel*> _hero_gold_labels;
    QLabel* _inventory_label;
    InventoryPane* _inventory_pane;
    EquipmentMarketPlacePane* _equipment_market_place_pane;
};

#endif
