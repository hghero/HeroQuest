#ifndef DIALOG_TREASURE_CARD_H
#define DIALOG_TREASURE_CARD_H

#include <QtWidgets/QDialog>

class TreasureCard;

class DialogTreasureCard : public QDialog
{
    Q_OBJECT

    public:
    DialogTreasureCard(const TreasureCard& treasure_card);
    virtual ~DialogTreasureCard();
};

#endif
