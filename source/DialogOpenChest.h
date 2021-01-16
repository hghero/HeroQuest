#ifndef DIALOG_OPEN_CHEST_H
#define DIALOG_OPEN_CHEST_H

#include <QtWidgets/QDialog>

class TreasureDescription;

class DialogOpenChest : public QDialog
{
    Q_OBJECT

    public:
    DialogOpenChest(const TreasureDescription& treasure_description);
    virtual ~DialogOpenChest();
};

#endif
