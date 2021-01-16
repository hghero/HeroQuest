//#define DEBUG

#include "HeroQuestStartDialog.h"
#include "HeroQuestLevelWindow.h"

#include <QtCore/QCoreApplication>
#include <QtGui/QGuiApplication>
#include <QtGui/QScreen>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLayout>
#include <QtWidgets/QPushButton>

#include "Debug.h"

using namespace std;

HeroQuestStartDialog::HeroQuestStartDialog(DialogReturnValue* dialog_return_value)
:
    QDialog((QDialog*)0),
    _dialog_return_value(dialog_return_value),
    _button_ids(),
    _buttons()
{
    int screen_width = 1920;
    int screen_height = 1080;
    if (!QGuiApplication::screens().empty())
    {
        QScreen* first_screen = *(QGuiApplication::screens().begin());
        screen_width = first_screen->availableGeometry().width();
        screen_height = first_screen->availableGeometry().height();
    }

    // size of window: width 50 %, height 50 %
    QSize dialog_size(screen_width / 2, screen_height / 2);
    setFixedSize(dialog_size);

    setStyleSheet("background-color: black;");

    /*
     * layout:
     *
     *  ---------------------------
     * |           |               |
     * | New Game  |  Quick Game   |
     * |           |               |
     *  ---------------------------
     * |           |               |
     * | Load Game |     Quit      |
     * |           |               |
     *  ---------------------------
     */

    QGridLayout* grid_layout = new QGridLayout;
    setLayout(grid_layout);

    _button_ids["New Game"] = 0;
    _button_ids["Quick Game"] = 1;
    _button_ids["Load Game"] = 2;
    _button_ids["Quit"] = 3;

    vector<QString> button_captions_ordered(_button_ids.size());
    for (map<QString, uint>::const_iterator it = _button_ids.begin(); it != _button_ids.end(); ++it)
    {
        button_captions_ordered[it->second] = it->first;
    }

    for (vector<QString>::const_iterator it = button_captions_ordered.begin(); it != button_captions_ordered.end(); ++it)
    {
        _buttons.push_back(new QPushButton(*it, this));
    }

    uint num_rows_cols = 1;
    while (num_rows_cols * num_rows_cols < button_captions_ordered.size())
    {
        ++num_rows_cols;
    }

    uint row = 0;
    uint col = 0;
    for (vector<QPushButton*>::iterator it = _buttons.begin(); it != _buttons.end(); ++it)
    {
        (*it)->setStyleSheet(HeroQuestLevelWindow::BUTTON_STYLE_ACTIVE);

        (*it)->setFixedWidth(uint(dialog_size.width() / 3));
        (*it)->setFixedHeight(uint(dialog_size.height() / 3));

        grid_layout->addWidget(*it, row, col, 1, 1);

        // next cell
        ++col;
        if (col == num_rows_cols)
        {
            ++row;
            col = 0;
        }
    }

    // connect buttons
    connect(getButton("New Game"), SIGNAL(clicked()), this, SLOT(newGameButtonClicked()));
    connect(getButton("Quick Game"), SIGNAL(clicked()), this, SLOT(quickGameButtonClicked()));
    connect(getButton("Load Game"), SIGNAL(clicked()), this, SLOT(loadGameButtonClicked()));
    connect(getButton("Quit"), SIGNAL(clicked()), this, SLOT(quitButtonClicked()));
}

HeroQuestStartDialog::~HeroQuestStartDialog()
{
    // NIX
}

QPushButton* HeroQuestStartDialog::getButton(const QString& str_id)
{
    return _buttons[_button_ids[str_id]];
}

void HeroQuestStartDialog::quickGameButtonClicked()
{
    *_dialog_return_value = QUICK_GAME;
    accept();
}

void HeroQuestStartDialog::newGameButtonClicked()
{
    *_dialog_return_value = NEW_GAME;
    accept();
}

void HeroQuestStartDialog::loadGameButtonClicked()
{
    *_dialog_return_value = LOAD_GAME;
    accept();
}

void HeroQuestStartDialog::quitButtonClicked()
{
    *_dialog_return_value = QUIT;
    accept();
}
