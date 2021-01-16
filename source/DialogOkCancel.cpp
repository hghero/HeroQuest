#include "DialogOkCancel.h"

#include <QtWidgets/QLabel>
#include <QtWidgets/QLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QDialogButtonBox>
#include "HeroQuestLevelWindow.h"

DialogOkCancel::DialogOkCancel(const QString& title, const QString& text)
: QDialog(HeroQuestLevelWindow::_hero_quest)
{
    setWindowTitle(title);
    setStyleSheet(HeroQuestLevelWindow::DIALOG_STYLE);

    /*
     *  ----------------------------------
     * |                                  |
     * |              text                |
     * |                                  |
     *  ----------------------------------
     * |      OK        |      Cancel     |
     *  ----------------------------------
     */
    QBoxLayout* vertical_box_layout = new QBoxLayout(QBoxLayout::TopToBottom);
    vertical_box_layout->setAlignment(Qt::AlignHCenter);
    setLayout(vertical_box_layout);

    QLabel* text_label = new QLabel(text);
    text_label->setStyleSheet(HeroQuestLevelWindow::DIALOG_TEXT_STYLE);
    vertical_box_layout->addWidget(text_label);

    QLabel* space_label = new QLabel();
    space_label->setFixedHeight(10);
    vertical_box_layout->addWidget(space_label);

    QDialogButtonBox* button_box = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    button_box->setStyleSheet(HeroQuestLevelWindow::DIALOG_STYLE);
    button_box->button(QDialogButtonBox::Ok)->setStyleSheet(HeroQuestLevelWindow::BUTTON_STYLE_ACTIVE);
    button_box->button(QDialogButtonBox::Cancel)->setStyleSheet(HeroQuestLevelWindow::BUTTON_STYLE_ACTIVE);
    connect(button_box, SIGNAL(accepted()), this, SLOT(accept()));
    connect(button_box, SIGNAL(rejected()), this, SLOT(reject()));
    vertical_box_layout->addWidget(button_box);
}

DialogOkCancel::~DialogOkCancel()
{
    // NIX
}
