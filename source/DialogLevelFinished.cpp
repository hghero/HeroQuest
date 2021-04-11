#include "DialogLevelFinished.h"

#include <QtWidgets/QLabel>
#include <QtWidgets/QLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QDialogButtonBox>

#include "HeroQuestLevelWindow.h"

using namespace std;

DialogLevelFinished::DialogLevelFinished()
: QDialog(HeroQuestLevelWindow::_hero_quest)
{
    setWindowTitle("Level finished");
    setStyleSheet(HeroQuestLevelWindow::DIALOG_STYLE);

    QBoxLayout* vertical_box_layout = new QBoxLayout(QBoxLayout::TopToBottom);
    vertical_box_layout->setAlignment(Qt::AlignHCenter);
    setLayout(vertical_box_layout);

    QLabel* text_label = new QLabel("Level finished!");
    text_label->setStyleSheet(HeroQuestLevelWindow::DIALOG_HEADER_STYLE);
    text_label->setMinimumHeight(100);
    text_label->setAlignment(Qt::AlignCenter);
    vertical_box_layout->addWidget(text_label);

    QLabel* space_label = new QLabel();
    space_label->setFixedHeight(10);
    vertical_box_layout->addWidget(space_label);

    QDialogButtonBox* button_box = new QDialogButtonBox(QDialogButtonBox::Ok);
    button_box->setStyleSheet(HeroQuestLevelWindow::DIALOG_STYLE);
    button_box->button(QDialogButtonBox::Ok)->setStyleSheet(HeroQuestLevelWindow::BUTTON_STYLE_ACTIVE);
    connect(button_box, SIGNAL(accepted()), this, SLOT(accept()));
    vertical_box_layout->addWidget(button_box);

    // center the dialog relative to the HeroQuestLevelWindow
    const QRect& parent_rect = HeroQuestLevelWindow::_hero_quest->geometry();
    move(parent_rect.center() - rect().center());
}

DialogLevelFinished::~DialogLevelFinished()
{
	// NIX
}
