#include "HintPane.h"

#include <QtWidgets/QLayout>

#include "Debug.h"
#include "HeroQuestLevelWindow.h"
#include "Level.h"
#include "Playground.h"
#include "QuestBoard.h"

using namespace std;

HintPane::HintPane()
: QLabel(),
  _hint(), _hint_label(0), _abort_label(0)
{
    QVBoxLayout* vertical_layout = new QVBoxLayout;
	setLayout(vertical_layout);
    vertical_layout->setAlignment(Qt::AlignCenter);
	vertical_layout->setMargin(0);
    vertical_layout->setSpacing(3);

    _hint_label = new QLabel();
    _hint_label->setStyleSheet(HeroQuestLevelWindow::DIALOG_TEXT_STYLE);
    _hint_label->setAlignment(Qt::AlignCenter);
    vertical_layout->addWidget(_hint_label);

    _abort_label = new QLabel();
    _abort_label->setStyleSheet(HeroQuestLevelWindow::DIALOG_TEXT_STYLE);
    _abort_label->setAlignment(Qt::AlignCenter);
    vertical_layout->addWidget(_abort_label);

	setMinimumSize(vertical_layout->sizeHint());
}

HintPane::~HintPane()
{
    // NIX
}

void HintPane::reset()
{
    _hint = "";
    _hint_label->setText(_hint);
    _abort_label->setText("");
    update();
}

void HintPane::setHint(const QString& hint, AbortBehaviour abort_behaviour)
{
    _hint = hint;
    _hint_label->setText(_hint);

    if (abort_behaviour == ALLOW_ABORT)
    {
        _abort_label->setText("Press <esc> to abort.");
    }
    else
    {
        _abort_label->setText("");
    }

    update();
}
