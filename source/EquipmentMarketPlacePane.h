#ifndef EQUIPMENTMARKETPLACEPANE_H
#define EQUIPMENTMARKETPLACEPANE_H

#include <QtWidgets/QLabel>

class QDialog;
class QScrollArea;
class QBoxLayout;
class DialogBuyEquipment;
class Hero;

class EquipmentMarketPlacePane : public QLabel
{
    Q_OBJECT

public:
    //
    //  ------------------------- ---------
    // |                         |         |
    // | EquipmentCardPane       |  (Use)  |
    // |                         |(Discard)|
    //  ------------------------- ---------
    // |                         |         |
    // | EquipmentCardPane       |  (Use)  |
    // |                         |(Discard)|
    //  -------------------------

    static const QSize MINIMUM_BUTTON_SIZE;

    enum EnableUseButton
    {
        USE_BUTTON_ENABLE = true, USE_BUTTON_DISABLE = false
    };

    enum EnableDisregardButton
    {
        DISCARD_BUTTON_ENABLE = true, DISREGARD_BUTTON_DISABLE = false
    };

    EquipmentMarketPlacePane(DialogBuyEquipment* dialog_buy_equipment, Hero* hero);
    virtual ~EquipmentMarketPlacePane();

    void updateEquipmentMarketPlace(Hero* hero);

    private:
    void addEquipmentCards(Hero* hero, QScrollArea* scroll_area, QBoxLayout* vertical_box_layout,
            int& grid_layout_spacing,
            int* min_width,
            int min_height,
            int* max_height);

    DialogBuyEquipment* _dialog_buy_equipment;
    QScrollArea* _scroll_area;
};

#endif
