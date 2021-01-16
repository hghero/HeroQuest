#include "DialogOpenChest.h"

#include <iostream>

#include <QtWidgets/QLabel>
#include <QtWidgets/QLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QDialogButtonBox>

#include "HeroQuestLevelWindow.h"
#include "TreasureDescription.h"
#include "TreasureDescriptionPane.h"

using namespace std;

DialogOpenChest::DialogOpenChest(const TreasureDescription& treasure_description)
: QDialog(HeroQuestLevelWindow::_hero_quest)
{
    //
    // TreasureDescriptionPane
    //
    //                               OK button
    //

    setWindowTitle("Open Chest");
    setStyleSheet(HeroQuestLevelWindow::DIALOG_STYLE);

    QBoxLayout* vertical_box_layout = new QBoxLayout(QBoxLayout::TopToBottom);
    vertical_box_layout->setAlignment(Qt::AlignHCenter);
    setLayout(vertical_box_layout);

    TreasureDescriptionPane* treasure_description_pane = new TreasureDescriptionPane(this, treasure_description);
    vertical_box_layout->addWidget(treasure_description_pane);

    QLabel* space_label = new QLabel();
    space_label->setFixedHeight(10);
    vertical_box_layout->addWidget(space_label);

    QDialogButtonBox* button_box = new QDialogButtonBox(QDialogButtonBox::Ok);
    button_box->setStyleSheet(HeroQuestLevelWindow::DIALOG_STYLE);
    button_box->button(QDialogButtonBox::Ok)->setStyleSheet(HeroQuestLevelWindow::BUTTON_STYLE_ACTIVE);
    connect(button_box, SIGNAL(accepted()), this, SLOT(accept()));
    vertical_box_layout->addWidget(button_box);
}

DialogOpenChest::~DialogOpenChest()
{
	// NIX
}
