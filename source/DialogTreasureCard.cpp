#include "DialogTreasureCard.h"

#include <iostream>

#include <QtWidgets/QLabel>
#include <QtWidgets/QLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QDialogButtonBox>

#include "HeroQuestLevelWindow.h"
#include "TreasureCard.h"
#include "TreasureCardPane.h"

using namespace std;

DialogTreasureCard::DialogTreasureCard(const TreasureCard& treasure_card)
: QDialog(HeroQuestLevelWindow::_hero_quest)
{
    setWindowTitle("Drawn Treasure Card");
    setStyleSheet(HeroQuestLevelWindow::DIALOG_STYLE);

    QBoxLayout* vertical_box_layout = new QBoxLayout(QBoxLayout::TopToBottom);
    vertical_box_layout->setAlignment(Qt::AlignHCenter);
    setLayout(vertical_box_layout);

    TreasureCardPane* treasure_card_pane = new TreasureCardPane(this, treasure_card);
    vertical_box_layout->addWidget(treasure_card_pane);

    QLabel* space_label = new QLabel();
    space_label->setFixedHeight(10);
    vertical_box_layout->addWidget(space_label);

    QDialogButtonBox* button_box = new QDialogButtonBox(QDialogButtonBox::Ok);
    button_box->setStyleSheet(HeroQuestLevelWindow::DIALOG_STYLE);
    button_box->button(QDialogButtonBox::Ok)->setStyleSheet(HeroQuestLevelWindow::BUTTON_STYLE_ACTIVE);
    connect(button_box, SIGNAL(accepted()), this, SLOT(accept()));
    vertical_box_layout->addWidget(button_box);
}

DialogTreasureCard::~DialogTreasureCard()
{
	// NIX
}
