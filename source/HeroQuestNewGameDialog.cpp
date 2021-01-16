//#define DEBUG

#include "HeroQuestNewGameDialog.h"
#include "HeroQuestLevelWindow.h"

#include <QtCore/QCoreApplication>
#include <QtGui/QGuiApplication>
#include <QtGui/QScreen>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtGui/QPainter>

#include "Debug.h"

using namespace std;

HeroQuestNewGameDialog::HeroQuestNewGameDialog(
        DialogReturnValue* dialog_return_value,
        vector<QString>* hero_names,
        SpellCard::SpellFamily* alb_spell_family)
:
    QDialog((QDialog*)0),
    _dialog_return_value(dialog_return_value),
    _hero_names(hero_names),
    _alb_spell_family(alb_spell_family),

    _barbarian_check_box(0),
    _dwarf_check_box(0),
    _alb_check_box(0),
    _magician_check_box(0),

    _barbarian_avatar(0),
    _dwarf_avatar(0),
    _alb_avatar(0),
    _magician_avatar(0),
    _inactive_avatar(0),

    _barbarian_avatar_label(0),
    _dwarf_avatar_label(0),
    _alb_avatar_label(0),
    _alb_spell_pile_combo_box(0),
    _magician_avatar_label(0),

    _button_ids(),
    _buttons()
{
    loadAvatars();

    int screen_width = 1920;
    int screen_height = 1080;
    if (!QGuiApplication::screens().empty())
    {
        QScreen* first_screen = *(QGuiApplication::screens().begin());
        screen_width = first_screen->availableGeometry().width();
        screen_height = first_screen->availableGeometry().height();
    }

    setStyleSheet("background-color: black;");

    /*
     * vertical_layout:
     *
     *  -----------------------------------
     * |                                   |
     * | Choose Heroes                     |
     * |                                   |
     *  -----------------------------------
     * |                        |          |
     * | Cancel to Main Menu    |   Ok     |
     * |                        |          |
     *  -----------------------------------
     */

    QVBoxLayout* vertical_layout = new QVBoxLayout;
    setLayout(vertical_layout);

    // checkboxes with hero avatars
    QLabel* choose_heroes_label = new QLabel("Choose heroes", this);
    choose_heroes_label->setStyleSheet(HeroQuestLevelWindow::DIALOG_HEADER_STYLE);
    vertical_layout->addWidget(choose_heroes_label, 0, Qt::AlignLeft);

    QLabel* grid_label = new QLabel(this);
    vertical_layout->addWidget(grid_label, 0, Qt::AlignLeft);

    QGridLayout* grid_layout = new QGridLayout(); // per row: left = checkbox, right = hero avatar
    grid_layout->setHorizontalSpacing(50);
    grid_layout->setVerticalSpacing(10);
    grid_layout->setContentsMargins(5, 5, 5, 5);
    grid_label->setLayout(grid_layout);

    // barbarian
    _barbarian_check_box = new QCheckBox("Barbarian", this);
    _barbarian_check_box->setStyleSheet(HeroQuestLevelWindow::DIALOG_TEXT_STYLE);
    _barbarian_check_box->setChecked(true);
    grid_layout->addWidget(_barbarian_check_box, 0, 0, 1, 1, Qt::AlignVCenter | Qt::AlignLeft);

    _barbarian_avatar_label = new QLabel(this);
    _barbarian_avatar_label->setPixmap(*_barbarian_avatar);
    grid_layout->addWidget(_barbarian_avatar_label, 0, 1, 1, 1, Qt::AlignVCenter | Qt::AlignHCenter);

    // dwarf
    _dwarf_check_box = new QCheckBox("Dwarf", this);
    _dwarf_check_box->setStyleSheet(HeroQuestLevelWindow::DIALOG_TEXT_STYLE);
    _dwarf_check_box->setChecked(true);
    grid_layout->addWidget(_dwarf_check_box, 1, 0, 1, 1, Qt::AlignVCenter | Qt::AlignLeft);

    _dwarf_avatar_label = new QLabel(this);
    _dwarf_avatar_label->setPixmap(*_dwarf_avatar);
    grid_layout->addWidget(_dwarf_avatar_label, 1, 1, 1, 1, Qt::AlignVCenter | Qt::AlignHCenter);

    // alb
    _alb_check_box = new QCheckBox("Alb", this);
    _alb_check_box->setStyleSheet(HeroQuestLevelWindow::DIALOG_TEXT_STYLE);
    _alb_check_box->setChecked(true);
    grid_layout->addWidget(_alb_check_box, 2, 0, 1, 1, Qt::AlignVCenter | Qt::AlignLeft);

    _alb_avatar_label = new QLabel(this);
    _alb_avatar_label->setPixmap(*_alb_avatar);
    grid_layout->addWidget(_alb_avatar_label, 2, 1, 1, 1, Qt::AlignVCenter | Qt::AlignHCenter);

    _alb_spell_pile_combo_box = new QComboBox(this);
    _alb_spell_pile_combo_box->addItem("Air Spells");
    _alb_spell_pile_combo_box->addItem("Water Spells");
    _alb_spell_pile_combo_box->addItem("Earth Spells");
    _alb_spell_pile_combo_box->addItem("Fire Spells");
    _alb_spell_pile_combo_box->setCurrentIndex(0);
    _alb_spell_pile_combo_box->setStyleSheet(HeroQuestLevelWindow::DIALOG_COMBOBOX_STYLE);
    grid_layout->addWidget(_alb_spell_pile_combo_box, 2, 2, 1, 1, Qt::AlignVCenter | Qt::AlignHCenter);

    // magician
    _magician_check_box = new QCheckBox("Magician", this);
    _magician_check_box->setStyleSheet(HeroQuestLevelWindow::DIALOG_TEXT_STYLE);
    _magician_check_box->setChecked(true);
    grid_layout->addWidget(_magician_check_box, 3, 0, 1, 1, Qt::AlignVCenter | Qt::AlignLeft);

    _magician_avatar_label = new QLabel(this);
    _magician_avatar_label->setPixmap(*_magician_avatar);
    grid_layout->addWidget(_magician_avatar_label, 3, 1, 1, 1, Qt::AlignVCenter | Qt::AlignHCenter);

    grid_label->setFixedSize(grid_layout->sizeHint());

    connect(_barbarian_check_box, SIGNAL(stateChanged(int)), SLOT(updateButtons()));
    connect(_dwarf_check_box, SIGNAL(stateChanged(int)), SLOT(updateButtons()));
    connect(_alb_check_box, SIGNAL(stateChanged(int)), SLOT(updateButtons()));
    connect(_magician_check_box, SIGNAL(stateChanged(int)), SLOT(updateButtons()));

    // horizontal line
    QWidget* horizontal_line = new QWidget;
    horizontal_line->setFixedHeight(5);
    horizontal_line->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    horizontal_line->setStyleSheet(QString("background-color: #c0c0c0;"));
    vertical_layout->addWidget(horizontal_line, 0, Qt::AlignLeft);

    // buttons
    QLabel* button_label = new QLabel(this);
    vertical_layout->addWidget(button_label, 0, Qt::AlignLeft);

    QHBoxLayout* buttons_layout = new QHBoxLayout;
    buttons_layout->setContentsMargins(0, 0, 0, 0);
    button_label->setLayout(buttons_layout);

    _button_ids["Cancel to Main Menu"] = 0;
    _button_ids["Ok"] = 1;

    vector<QString> button_captions_ordered(_button_ids.size());
    for (map<QString, uint>::const_iterator it = _button_ids.begin(); it != _button_ids.end(); ++it)
    {
        button_captions_ordered[it->second] = it->first;
    }

    for (vector<QString>::const_iterator it = button_captions_ordered.begin(); it != button_captions_ordered.end(); ++it)
    {
        _buttons.push_back(new QPushButton(*it, this));
    }

    for (vector<QPushButton*>::iterator it = _buttons.begin(); it != _buttons.end(); ++it)
    {
        (*it)->setStyleSheet(HeroQuestLevelWindow::BUTTON_STYLE_ACTIVE);
        buttons_layout->addWidget(*it, 0, Qt::AlignBottom);
    }

    button_label->setFixedSize(buttons_layout->sizeHint());

    // connect buttons
    connect(getButton("Cancel to Main Menu"), SIGNAL(clicked()), this, SLOT(cancelButtonClicked()));
    connect(getButton("Ok"), SIGNAL(clicked()), this, SLOT(okButtonClicked()));
}

HeroQuestNewGameDialog::~HeroQuestNewGameDialog()
{
    // NIX
}

/*!
 * The avatars are directly scaled to HeroQuestLevelWindow::FIELD_SIZE.
 */
void HeroQuestNewGameDialog::loadAvatars()
{
    Barbarian tmp_barbarian(false);
    const QString& barbarian_filename = tmp_barbarian.getIconFilename();
    _barbarian_avatar = new QPixmap(barbarian_filename.toUtf8().constData());
    if (_barbarian_avatar == 0)
        DVX(("barbarian avatar could not be loaded from file \"%s\"", qPrintable(barbarian_filename)));
    *_barbarian_avatar = _barbarian_avatar->scaledToHeight(HeroQuestLevelWindow::FIELD_SIZE);

    Dwarf tmp_dwarf(false);
    const QString& dwarf_filename = tmp_dwarf.getIconFilename();
    _dwarf_avatar = new QPixmap(dwarf_filename.toUtf8().constData());
    if (_dwarf_avatar == 0)
        DVX(("dwarf avatar could not be loaded from file \"%s\"", qPrintable(dwarf_filename)));
    *_dwarf_avatar = _dwarf_avatar->scaledToHeight(HeroQuestLevelWindow::FIELD_SIZE);

    Alb tmp_alb(false);
    const QString& alb_filename = tmp_alb.getIconFilename();
    _alb_avatar = new QPixmap(alb_filename.toUtf8().constData());
    if (_barbarian_avatar == 0)
        DVX(("alb avatar could not be loaded from file \"%s\"", qPrintable(alb_filename)));
    *_alb_avatar = _alb_avatar->scaledToHeight(HeroQuestLevelWindow::FIELD_SIZE);

    Magician tmp_magician(false);
    const QString& magician_filename = tmp_magician.getIconFilename();
    _magician_avatar = new QPixmap(magician_filename.toUtf8().constData());
    if (_magician_avatar == 0)
        DVX(("magician avatar could not be loaded from file \"%s\"", qPrintable(magician_filename)));
    *_magician_avatar = _magician_avatar->scaledToHeight(HeroQuestLevelWindow::FIELD_SIZE);

    // generate _inactive_avatar
    _inactive_avatar = new QPixmap(HeroQuestLevelWindow::FIELD_SIZE, HeroQuestLevelWindow::FIELD_SIZE);

    QColor fill_color(70, 70, 70, 255);
    QColor rect_color(150, 150, 150, 255);

    QPainter* painter = new QPainter(_inactive_avatar);
    painter->fillRect(0, 0, HeroQuestLevelWindow::FIELD_SIZE-1, HeroQuestLevelWindow::FIELD_SIZE-1, fill_color);

    QPen pen(rect_color);
    pen.setWidth(1);
    painter->setPen(pen);
    painter->drawRect(0, 0, HeroQuestLevelWindow::FIELD_SIZE-1, HeroQuestLevelWindow::FIELD_SIZE-1);
}

QPushButton* HeroQuestNewGameDialog::getButton(const QString& str_id)
{
    return _buttons[_button_ids[str_id]];
}

void HeroQuestNewGameDialog::updateButtons()
{
    // at least one hero must be checked in order to enable the Ok button
    if (_barbarian_check_box->checkState() == Qt::Checked ||
        _dwarf_check_box->checkState() == Qt::Checked ||
        _alb_check_box->checkState() == Qt::Checked ||
        _magician_check_box->checkState() == Qt::Checked)
    {
        getButton("Ok")->setEnabled(true);
        getButton("Ok")->setStyleSheet(HeroQuestLevelWindow::BUTTON_STYLE_ACTIVE);
    }
    else
    {
        getButton("Ok")->setEnabled(false);
        getButton("Ok")->setStyleSheet(HeroQuestLevelWindow::BUTTON_STYLE_INACTIVE);
    }

    // show avatars only for checked heroes
    if (_barbarian_check_box->checkState() == Qt::Checked)
        _barbarian_avatar_label->setPixmap(*_barbarian_avatar);
    else
        _barbarian_avatar_label->setPixmap(*_inactive_avatar);

    if (_dwarf_check_box->checkState() == Qt::Checked)
        _dwarf_avatar_label->setPixmap(*_dwarf_avatar);
    else
        _dwarf_avatar_label->setPixmap(*_inactive_avatar);

    if (_alb_check_box->checkState() == Qt::Checked)
        _alb_avatar_label->setPixmap(*_alb_avatar);
    else
        _alb_avatar_label->setPixmap(*_inactive_avatar);

    if (_magician_check_box->checkState() == Qt::Checked)
        _magician_avatar_label->setPixmap(*_magician_avatar);
    else
        _magician_avatar_label->setPixmap(*_inactive_avatar);

    // show spell pile selection for Alb only if Alb has been chosen
    if (_alb_check_box->checkState() == Qt::Checked)
        _alb_spell_pile_combo_box->setEnabled(true);
    else
        _alb_spell_pile_combo_box->setEnabled(false);
}

void HeroQuestNewGameDialog::okButtonClicked()
{
    _hero_names->clear();

    if (_barbarian_check_box->checkState() == Qt::Checked)
        _hero_names->push_back("Barbarian");
    if (_dwarf_check_box->checkState() == Qt::Checked)
        _hero_names->push_back("Dwarf");
    if (_alb_check_box->checkState() == Qt::Checked)
        _hero_names->push_back("Alb");
    if (_magician_check_box->checkState() == Qt::Checked)
        _hero_names->push_back("Magician");

    switch (_alb_spell_pile_combo_box->currentIndex())
    {
        case 0:
            *_alb_spell_family = SpellCard::AIR;
            break;
        case 1:
            *_alb_spell_family = SpellCard::WATER;
            break;
        case 2:
            *_alb_spell_family = SpellCard::EARTH;
            break;
        case 3:
            *_alb_spell_family = SpellCard::FIRE;
            break;
        default:
            ProgError("spell family undefined");
            break;
    }

    *_dialog_return_value = OK;
    accept();
}

void HeroQuestNewGameDialog::cancelButtonClicked()
{
    *_dialog_return_value = QUIT;
    accept();
}
