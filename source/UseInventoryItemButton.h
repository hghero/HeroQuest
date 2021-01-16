#ifndef USE_INVENTORY_ITEM_BUTTON_H
#define USE_INVENTORY_ITEM_BUTTON_H

#include <QtWidgets/QPushButton>

class QString;
class InventoryItem;
class DialogInventory;

class UseInventoryItemButton : public QPushButton
{
    Q_OBJECT

public:
    UseInventoryItemButton(DialogInventory* dialog_inventory, QWidget* parent, const InventoryItem& inventory_item, const QString& text);
	virtual ~UseInventoryItemButton();

	const InventoryItem& getInventoryItem() const;

public slots:
	void use() const;

private:
	DialogInventory* _dialog_inventory;
	const InventoryItem& _inventory_item;
};

#endif
