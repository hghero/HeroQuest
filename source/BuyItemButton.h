#ifndef BUY_ITEM_BUTTON_H
#define BUY_ITEM_BUTTON_H

#include <QtWidgets/QPushButton>

class QString;
class EquipmentCard;
class Inventory;
class DialogBuyEquipment;

class BuyItemButton : public QPushButton
{
    Q_OBJECT

public:
    BuyItemButton(DialogBuyEquipment* dialog_buy_equipment, QWidget* parent, const EquipmentCard& equipment_card,
            Inventory* inventory, const QString& text);
    virtual ~BuyItemButton();

public slots:
    void buy(); //! buy the equipment card

private:
    DialogBuyEquipment* _dialog_buy_equipment;
    const EquipmentCard& _equipment_card;
    Inventory* _inventory; //! the inventory to which the equipment will be added when bought
};

#endif
