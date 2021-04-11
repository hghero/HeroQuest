#ifndef USE_INVENTORY_ITEM_BUTTON_H
#define USE_INVENTORY_ITEM_BUTTON_H

#include <QtWidgets/QPushButton>

class QString;
class InventoryItem;
class QDialog;
class Inventory;

class UseInventoryItemButton : public QPushButton
{
    Q_OBJECT

public:
    UseInventoryItemButton(QDialog* dialog_inventory, QWidget* parent, const InventoryItem& inventory_item,
            Inventory* inventory, const QString& text);
	virtual ~UseInventoryItemButton();

	const InventoryItem& getInventoryItem() const;

public slots:
    void use() const; //! use a spell or equipment card
    void discard(); //! discard an equipment card

private:
    QDialog* _parent_dialog;
    const InventoryItem& _inventory_item; //! the item which can be used (spell, equipemnt) or discarded (equipment)
    Inventory* _inventory; //! the inventory from which the equipment will be discarded
};

#endif
