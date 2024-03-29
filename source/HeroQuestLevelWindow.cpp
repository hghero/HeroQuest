//#define HG_DEBUG

#include "HeroQuestLevelWindow.h"

#include <iostream>
#include <fstream>
#include <windows.h>

#include <QtCore/QCoreApplication>
#include <QtGui/QResizeEvent>
#include <QtGui/QPainter>
#include <QtGui/QGuiApplication>
#include <QtGui/QScreen>
#include <QtGui/QCloseEvent>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QFileDialog>

#include "Preferences.h"
#include "Debug.h"
#include "SaveContext.h"
#include "LoadContext.h"
#include "Hero.h"
#include "HeroStatisticPane.h"
#include "DiceRollPane.h"
#include "DiceRollArea.h"
#include "ButtonPane.h"
#include "HintPane.h"
#include "Playground.h"
#include "Monster.h"
#include "DialogOkCancel.h"
#include "DialogOk.h"
#include "TreasureCard.h"
#include "DialogTreasureCard.h"
#include "DialogInventory.h"
#include "DialogOpenChest.h"
#include "InventoryItem.h"
#include "Inventory.h"
#include "StreamUtils.h"
#include "LayoutHelper.h"
#include "GameState.h"
#include "Command.h"
#include "Level01TheProbation.h"
#include "Level02SavingSirRagnar.h"
#include "HeroCamp.h"
#include "SpellCardStorage.h"
#include "TreasureCardStorage.h"
#include "TreasureDataTypes.h"
#include "ParameterStorage.h"
#include "EquipmentCardStorage.h"
#include "Trap.h"

#define DEBUG_PANE_SIZES

using namespace std;

HeroQuestLevelWindow* HeroQuestLevelWindow::_hero_quest = 0; // static instance pointer

// TODO: move these constants to a more global class
const QString HeroQuestLevelWindow::BUTTON_STYLE_ACTIVE =
        "QPushButton {border-style: outset; min-width: 5em; border-width: 1px; border-radius: 5px; border-color: rgb(255,200,70);  padding: 4px; background-color: rgb(50,50,50); color: rgb(255,200,70); font: bold 14px;}"
                                         "QPushButton:pressed {border-style: inset;  min-width: 5em; border-width: 1px; border-radius: 5px; border-color: rgb(255,200,70);  padding: 4px; background-color: rgb(150,150,150); color: rgb(255,200,70); font: bold 14px;}";
const QString HeroQuestLevelWindow::BUTTON_STYLE_INACTIVE = "QPushButton {border-style: outset; min-width: 5em; border-width: 1px; border-radius: 5px; border-color: rgb(150,150,150); padding: 4px; background-color: rgb(50,50,50); color: rgb(150,150,150); font: bold 14px;}";


const QString HeroQuestLevelWindow::INVENTORY_BUTTON_STYLE_ACTIVE   = "QPushButton {border-style: outset; min-width: 5em; border-width: 1px; border-radius: 5px; border-color: rgb(0,0,0);  padding: 4px; background-color: rgb(255,200,70); color: rgb(30,30,30); font: bold 14px;}"
                                                   "QPushButton:pressed {border-style: inset;  min-width: 5em; border-width: 1px; border-radius: 5px; border-color: rgb(155,155,155);  padding: 4px; background-color: rgb(255,200,70); color: rgb(30,30,30); font: bold 14px;}";
const QString HeroQuestLevelWindow::INVENTORY_BUTTON_STYLE_INACTIVE = "QPushButton {border-style: outset; min-width: 5em; border-width: 1px; border-radius: 5px; border-color: rgb(150,150,150); padding: 4px; background-color: rgb(50,50,50); color: rgb(150,150,150); font: bold 14px;}";

const QString HeroQuestLevelWindow::DIALOG_STYLE          = "background-color: black;";
const QString HeroQuestLevelWindow::DIALOG_HEADER_STYLE   = "color: rgb(220,220,220); font: bold 14px;";
const QString HeroQuestLevelWindow::DIALOG_HEADER_BIG_STYLE = "color: rgb(220,220,220); font: bold 30px;";
const QString HeroQuestLevelWindow::DIALOG_TEXT_STYLE     = "color: rgb(220,220,220); font: 14px;";
const QString HeroQuestLevelWindow::DIALOG_TEXT_BIG_STYLE = "color: rgb(220,220,220); font: 30px;";
const QString HeroQuestLevelWindow::DIALOG_COMBOBOX_STYLE = "color: rgb(220,220,220); font: 14px; background-color: rgb(100,100,100); selection-background-color: rgb(150,150,150);";

const QString HeroQuestLevelWindow::STATISTIC_HEADER_STYLE_ACTIVE   = "QLabel { background-color: transparent; color: black;            font: 12pt; font-weight: bold; }";
const QString HeroQuestLevelWindow::STATISTIC_HEADER_STYLE_INACTIVE = "QLabel { background-color: transparent; color: rgb(100,100,100); font: 12pt; font-weight: bold; }";
const QString HeroQuestLevelWindow::STATISTIC_BODY_STYLE_ACTIVE     = "QLabel { background-color: transparent; color: black;            font: 10pt;                    }";
const QString HeroQuestLevelWindow::STATISTIC_BODY_STYLE_INACTIVE   = "QLabel { background-color: transparent; color: rgb(100,100,100); font: 10pt;                    }";
const QString HeroQuestLevelWindow::STATISTIC_BODY_STYLE_WARNING    = "QLabel { background-color: transparent; color: red;              font: 10pt;                    }";

const QString HeroQuestLevelWindow::DICE_ROLL_PANE_STYLE = "QLabel { background-color: rgb(40,40,40);}";

bool HeroQuestLevelWindow::_debug_create_attacks_creature_command_running = false;

HeroQuestLevelWindow::HeroQuestLevelWindow(
        const QString& version,
        QApplication& app,
        const QString& filename,
        GameState* game_state,
        SpellCardStorage* spell_card_storage, //
        TreasureCardStorage* treasure_card_storage, //
        EquipmentCardStorage* equipment_card_storage, //
        HeroCamp& hero_camp)
:
    QMainWindow((QMainWindow*)0),
    _version(version),
    _app(app),
    _save_action(0),
    _game_state(game_state),
    _hero_camp(
                hero_camp),
    _spell_card_storage(spell_card_storage),
    _treasure_card_storage(treasure_card_storage),
    _equipment_card_storage(
                equipment_card_storage),
    _level(0),
    _heroes(),
    _action_pane(0),
    _dice_roll_pane(0),
    _button_pane(0),
    _playground(0),
    _info_pane_wrapper(0),
    _info_pane(0),
    _hero_statistic_panes(),
    _sound_manager(0), //
        _event_blocking_bitmask(EventBlockingContext::EVENT_BLOCKING_NONE), //
        _is_painting(false), //
        _user_interaction_mode(USER_INTERACTION_NORMAL), //
        _user_selected_node_id(-1, -1), //
        _user_selected_two_node_ids(make_pair(NodeID(-1, -1), NodeID(-1, -1))), //
        _user_abort(
                false)
{
    _hero_quest = this; // set static instance pointer

#if 0
    string str;
    cin >> str;
#endif
    create();

    // menu
    QMenu* file_menu = menuBar()->addMenu("&File");

    QAction* load_action = file_menu->addAction("&Load Game");
    load_action->
    connect(load_action, SIGNAL(triggered()), this, SLOT(loadGame()));

    _save_action = file_menu->addAction("&Save Game");
    connect(_save_action, SIGNAL(triggered()), this, SLOT(saveGameChooseFilename()));

    QAction* exit_action = file_menu->addAction("E&xit");
    connect(exit_action, SIGNAL(triggered()), this, SLOT(close()));

    // Exit on escape; block paintEvents during load; ...
    // Note that it is crucial to call installEventFilter() on the _app! It does NOT suffice to install it on this HeroQuestLeveWindow!
    _app.installEventFilter(this);

    hero_camp.getHeroes(&_heroes);

    createGUIElements();

    _sound_manager = new SoundManager;

    startGame(filename);

    showMaximized();
}

HeroQuestLevelWindow::~HeroQuestLevelWindow()
{
    delete _sound_manager;
}

std::vector<Hero*>& HeroQuestLevelWindow::getHeroes()
{
    return _heroes;
}

SpellCardStorage* HeroQuestLevelWindow::getSpellCardStorage()
{
    return _spell_card_storage;
}

void HeroQuestLevelWindow::createGUIElements()
{
    /*
     * layout:
     *
     *  -------------------------------------------------
     * |  action   |     playground      |     info      |
     * |           |                     |               |
     * |           |                     |               |
     * |           |                     |               |
     * |           |                     |               |
     * |           |                     |               |
     * |           |                     |               |
     *  -------------------------------------------------
     */

    // central widget
    QWidget* central_widget = new QWidget(this);
    setCentralWidget(central_widget);
    central_widget->setStyleSheet("background-color: black;");
    QHBoxLayout* central_widget_layout = new QHBoxLayout;
    central_widget_layout->setAlignment(Qt::AlignTop); // however, each horizontal box is aligned vertically-centered
    central_widget_layout->setSpacing(0);
    central_widget_layout->setContentsMargins(0, 0, 0, 0);
    central_widget->setLayout(central_widget_layout);
#if 0
    setStyleSheet("color: rgb(255,0,0) ; border: 1px solid #00ff00 ;");
#endif

    // the playground size determines the sizes of the surrounding dialog elements
    _playground = new Playground(central_widget);
    _playground->setFixedSize(
            QSize(ParameterStorage::instance->getPlaygroundWidth(), ParameterStorage::instance->getPlaygroundHeight()));
    _playground->create();
#if 0
    _playground_pane->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    _playground_pane->setStyleSheet("color: rgb(255,0,0) ; border: 1px solid #00ff00 ;");
#endif

    // stretch
    central_widget_layout->addStretch();

    // action pane (top: DiceRollPane; middle: ButtonPane; bottom: HintPane)
    _action_pane = new QLabel(central_widget);
    _action_pane->setFixedWidth(ParameterStorage::instance->getActionPaneWidth());
    central_widget_layout->addWidget(_action_pane);
#if 0
    _action_pane->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    _action_pane->setStyleSheet("color: rgb(255,0,0) ; border: 1px solid #ff0000 ;");
#endif
    QVBoxLayout* action_pane_layout = new QVBoxLayout;
    _action_pane->setLayout(action_pane_layout);
    action_pane_layout->setContentsMargins(0, 0, 0, 0);
    _dice_roll_pane = new DiceRollPane(ParameterStorage::instance->getActionPaneWidth(),
            ParameterStorage::instance->getFieldSize());
    action_pane_layout->addStretch(); // test
    action_pane_layout->addWidget(_dice_roll_pane);
    _button_pane = new ButtonPane();
    action_pane_layout->addWidget(_button_pane);
    action_pane_layout->addStretch();
    _hint_pane = new HintPane();
    action_pane_layout->addWidget(_hint_pane);
    action_pane_layout->addStretch();
    _action_pane->setMinimumHeight(action_pane_layout->sizeHint().height());
    DV(("Action pane height: %d", _action_pane->height()));

    // stretch
    central_widget_layout->addStretch();

    // playground
    QLabel* playground_pane = new QLabel(central_widget);
    playground_pane->setFixedWidth(_playground->width());
    QVBoxLayout* playground_layout = new QVBoxLayout;
    playground_layout->setAlignment(Qt::AlignVCenter);
    playground_pane->setLayout(playground_layout);
    playground_layout->setContentsMargins(0, 0, 0, 0);
    playground_layout->setSpacing(0);
    playground_layout->addStretch();
    playground_layout->addWidget(_playground);
    playground_layout->addStretch();
    playground_pane->setMinimumHeight(playground_layout->sizeHint().height());
    central_widget_layout->addWidget(playground_pane);
#if 0
    playground_pane->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    playground_pane->setStyleSheet("color: rgb(255,0,0) ; border: 1px solid #00ff00 ;");
#endif

    //image_label->setFrameStyle(QFrame::Panel | QFrame::Sunken);

    // stretch
    central_widget_layout->addStretch();

    addInfoPaneToCentralWidget();

    // stretch
    central_widget_layout->addStretch();

    adjustSize();

#ifdef DEBUG_PANE_SIZES
    DVX(("========================================="));
    DVX(("HeroQuestLevelWindow::createGuiElements()"));
    DVX(("-----------------------------------------"));
    DVX(("screen_width = %d", ParameterStorage::instance->getScreenWidth()));
    DVX(("action_pane_width = %d (actual %d)", ParameterStorage::instance->getActionPaneWidth(), _action_pane->width()));
    DVX(("playground_width = %d (actual %d)", ParameterStorage::instance->getPlaygroundWidth(), _playground->width()));
    DVX(("playground_height = %d", _playground->height()));
    DVX(("info_pane_wrapper width x height = %d x %d", _info_pane_wrapper->width(), _info_pane_wrapper->height()));
    DVX(("info_pane_width = %d (actual %d)", ParameterStorage::instance->getInfoPaneWidth(), _info_pane->width()));
    DVX(("_info_pane width x height = %d x %d", _info_pane->width(), _info_pane->height()));
#endif

    connectActionButtons();
}

void HeroQuestLevelWindow::addInfoPaneToCentralWidget()
{
    removeHeroStatisticPanes();
    delete _info_pane_wrapper;
    QWidget* central_widget = centralWidget();

    // =========
    // info pane
    // =========
    //
    //  -------- info_pane_wrapper                                             : fixed:info_pane_width x fixed:playground_height ------
    // |                                                                                                                               |
    // |   ----- scroll_area (parent: info_pane_wrapper)                       : fixed:info_pane_width x fixed:playground_height ---   |
    // |  |                                                                                                                         |  |
    // |  |   -- _info_pane (parent: info_pane_wrapper; widget of scroll_area) : variable size, depending on contained panes ----   |  |
    // |  |  |                                                                                                                   |  |  |
    // |  |  |                                                                                                                   |  |  |
    // |  |  |                                                                                                                   |  |  |
    // |  |  |                                                                                                                   |  |  |
    // |  |  |                                                                                                                   |  |  |
    // |  |  |                                                                                                                   |  |  |
    // |  |  |                                                                                                                   |  |  |
    // |  |  |                                                                                                                   |  |  |
    // |  |  |                                                                                                                   |  |  |
    // |  |   -------------------------------------------------------------------------------------------------------------------   |  |
    // |  |                                                                                                                         |  |
    // |   -------------------------------------------------------------------------------------------------------------------------   |
    // |                                                                                                                               |
    //  -------------------------------------------------------------------------------------------------------------------------------

    // info pane: info_pane_wrapper => scroll_area => _info_pane
    _info_pane_wrapper = new QLabel(central_widget);
    _info_pane = new QLabel(_info_pane_wrapper);
    QVBoxLayout* info_pane_layout = new QVBoxLayout;
    _info_pane->setLayout(info_pane_layout);
    info_pane_layout->setContentsMargins(0, 0, 0, 0);
#if 0
    _info_pane->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    _info_pane->setStyleSheet("color: rgb(255,0,0) ; border: 1px solid #ff0000 ;");
#endif
    _info_pane_wrapper->setFixedSize(ParameterStorage::instance->getInfoPaneWidth(),
            ParameterStorage::instance->getPlaygroundHeight());
    QScrollArea* scroll_area = new QScrollArea(_info_pane_wrapper);
    {
        scroll_area->setFixedSize(ParameterStorage::instance->getInfoPaneWidth(),
                ParameterStorage::instance->getPlaygroundHeight());
        scroll_area->setWidget(_info_pane);
    }
    central_widget->layout()->addWidget(_info_pane_wrapper);
}

/*!
 * Shows the avatar of the currently acting creature on the action pane.
 */
void HeroQuestLevelWindow::initActionPane()
{
    if (!currentThreadIsGUIThread())
    {
        QMetaObject::invokeMethod(this, "initActionPane", Qt::BlockingQueuedConnection);
        return;
    }

    Creature* creature = _level->getCurrentlyActingCreature();
    if (creature == 0)
        return;

    resetActionPane();

    // restore the movement dice roll area
    _dice_roll_pane->getMovementDiceRollArea()->setContent(creature->getAvatar());
    if (creature->isHero())
    {
        Hero* hero = dynamic_cast<Hero*>(creature);
        _dice_roll_pane->updateMovementDice(*hero, _level->getCurrentHeroActionStates()._dice_results);
    }
    _dice_roll_pane->update();
}

/*!
 * Removes the avatar and everything of the currently acting creature from the action pane.
 */
void HeroQuestLevelWindow::resetActionPane()
{
    if (!HeroQuestLevelWindow::currentThreadIsGUIThread())
    {
        cout << "HeroQuest::resetActionPane() called from non-GUI thread" << endl;
        return;
    }

    _dice_roll_pane->getMovementDiceRollArea()->clearContent();
    _dice_roll_pane->getAttackDiceRollArea()->clearContent();
    _dice_roll_pane->getDefendDiceRollArea()->clearContent();

    _dice_roll_pane->update();
}

void HeroQuestLevelWindow::updateButtons()
{
    DV(("HeroQuestLevelWindow::updateButtons BEGIN"));
    // execute in GUI thread
    if (!currentThreadIsGUIThread())
    {
        QMetaObject::invokeMethod(this, "updateButtons", Qt::BlockingQueuedConnection);
        return;
    }

    DV(("HeroQuestLevelWindow::updateButtons check heroesAreMoving"));
    if (_level->heroesAreMoving())
    {
        _button_pane->updateButtons(_level->getLevelState()->_hero_action_states[_level->getLevelState()->_current_hero_idx]);
    }
    else if (_level->monstersAreMoving())
    {
        _button_pane->setAllInactive();
    }
    DV(("HeroQuestLevelWindow::updateButtons END"));
}

void HeroQuestLevelWindow::updateHeroStatisticPanes()
{
    if (!currentThreadIsGUIThread())
    {
        QMetaObject::invokeMethod(this, "updateHeroStatisticPanes", Qt::QueuedConnection);
        return;
    }

    for (vector<HeroStatisticPane*>::iterator it = _hero_statistic_panes.begin(); it != _hero_statistic_panes.end(); ++it)
    {
        (*it)->update();
    }
}

void HeroQuestLevelWindow::updateHeroStatisticPaneOfCurrentHero()
{
    if (_level == 0)
        return;

    Creature* currentCreature = _level->getCurrentlyActingCreature();
    Hero* currentHero = dynamic_cast<Hero*>(currentCreature);
    if (currentHero == 0)
        return;

    for (vector<HeroStatisticPane*>::iterator it = _hero_statistic_panes.begin(); it != _hero_statistic_panes.end(); ++it)
    {
        if ((*it)->getHero() == currentHero)
            (*it)->update();
    }
}

void HeroQuestLevelWindow::updateHeroStatisticPane(const Hero* hero)
{
    for (vector<HeroStatisticPane*>::iterator it = _hero_statistic_panes.begin(); it != _hero_statistic_panes.end();
            ++it)
    {
        if ((*it)->getHero() == hero)
            (*it)->update();
    }
}

/*!
 * Waits for ms ms. While waiting, processing paint events is allowed, but input events are blocked (discarded).
 *
 * @param ms Wait time in ms.
 */
void HeroQuestLevelWindow::delay(uint ms)
{
    DelayCommand cmd(ms);
    {
        EventBlockingContext event_blocking_context(this, EventBlockingContext::EVENT_BLOCKING_INPUT_EVENTS);
        executeBackground(cmd);
    }
}

bool HeroQuestLevelWindow::execute(Command& cmd)
{
    cmd.run();
    return cmd.succeeded();
}

/*!
 * Commands run in background must not access GUI elements!
 */
bool HeroQuestLevelWindow::executeBackground(Command& cmd)
{
    DV(("running command %s in background", qPrintable(cmd.getName())));

    // setup internal event loop to dispatch Windows events
    QEventLoop loop;

    // execute cmd and wait until it finishes
    cmd.start();

    while (!cmd.isFinished())
    {
        loop.processEvents();
    }

    return cmd.succeeded();
}

EventBlockingContext::EventBlockingBitmask HeroQuestLevelWindow::getEventBlockingBitmask() const
{
    return _event_blocking_bitmask;
}

void HeroQuestLevelWindow::setEventBlockingBitmask(
        const EventBlockingContext::EventBlockingBitmask event_blocking_bitmask)
{
    _event_blocking_bitmask = event_blocking_bitmask;
}

bool HeroQuestLevelWindow::currentThreadIsGUIThread()
{
    return QThread::currentThread() == QCoreApplication::instance()->thread();
}

void HeroQuestLevelWindow::addMonsterToLevelAndPlayground(Monster* monster, const NodeID& node_id)
{
    _playground->addCreature(monster, node_id);
    _level->_level_state._monster_action_states.push_back(Level::MonsterActionStates());
}

QSize HeroQuestLevelWindow::getInfoPaneSize() const
{
    QSize current_info_pane_size(0, 0);
    if (_info_pane != 0)
        current_info_pane_size = _info_pane->size();
    return current_info_pane_size;
}

/*!
 * Plays the sound with id sound_id non-blocking in the GUI thread.
 */
void HeroQuestLevelWindow::playSoundOnce(QVariant sound_id)
{
    if (!currentThreadIsGUIThread())
    {
        QMetaObject::invokeMethod(this, "playSoundOnce", Qt::QueuedConnection, Q_ARG(QVariant, sound_id));
        return;
    }

    bool ok = false;
    uint sound_id_uint = sound_id.toUInt(&ok);
    if (!ok)
        return;

    _sound_manager->playSoundOnce(SoundManager::SoundID(sound_id_uint));
}

void HeroQuestLevelWindow::removeHeroStatisticPanes()
{
    for (vector<HeroStatisticPane*>::iterator it = _hero_statistic_panes.begin(); it != _hero_statistic_panes.end();
            ++it)
    {
        removeHeroStatisticPane((*it)->getHero());
    }
    _hero_statistic_panes.clear();
}

void HeroQuestLevelWindow::removeHeroStatisticPane(const Hero* hero)
{
    HeroStatisticPane* hero_statistic_pane = 0;
    vector<HeroStatisticPane*>::iterator it;
    size_t index = 0;
    for (it = _hero_statistic_panes.begin(); it != _hero_statistic_panes.end(); ++it)
    {
        if ((*it)->getHero() == hero)
        {
            hero_statistic_pane = *it;
            break;
        }
        ++index;
    }
    if (hero_statistic_pane == 0)
    {
        DVX(("Could not remove HeroStatisticPane"));
        return;
    }

#ifdef DEBUG_PANE_SIZES
    DVX(("================================================="));
    DVX(("removeHeroStatisticPane()"));
    DVX(("-------------------------------------------------"));
    DVX(("_info_pane->layout() content before remove = %d", LayoutHelper::getNumWidgets(_info_pane->layout())));
#endif

    size_t layout_index = index * 2;
    _info_pane->layout()->removeItem(_info_pane->layout()->itemAt(int(layout_index + 1))); // space below statistic pane
    _info_pane->layout()->removeItem(_info_pane->layout()->itemAt(int(layout_index))); // statistic pane

    _hero_statistic_panes.erase(it);
    delete hero_statistic_pane;

    adjustInfoPaneHeight();

#ifdef DEBUG_PANE_SIZES
    DVX(("_info_pane->layout() content after remove = %d", LayoutHelper::getNumWidgets(_info_pane->layout())));
#endif
}

void HeroQuestLevelWindow::startHeroTurn()
{
    if (!currentThreadIsGUIThread())
    {
        QMetaObject::invokeMethod(this, "startHeroTurn", Qt::QueuedConnection);
        return;
    }

    if (_level == 0)
        return;

    _level->startHeroTurn();
}

/*!
 * \return True if applying the spell to hero is successful.
 */
bool HeroQuestLevelWindow::checkApplySpell(Hero* hero)
{
    return checkApplySpell(QVariant::fromValue(hero));
}

/*!
 * \return True if applying the spell to hero is successful.
 */
bool HeroQuestLevelWindow::checkApplySpell(QVariant hero)
{
    if (!currentThreadIsGUIThread())
    {
        bool ret;
        QMetaObject::invokeMethod(this, "checkApplySpell", Qt::BlockingQueuedConnection, Q_RETURN_ARG(bool, ret), Q_ARG(QVariant, hero));
        return ret;
    }

    Hero* actual_hero = hero.value<Hero*>();
    if (actual_hero == 0)
    {
        cout << "HeroQuest::checkApplySpell: actual_hero == 0" << endl;
        return false;
    }

    if (!actual_hero->getInventory().hasResistancePotion())
    {
        return true;
    }

    // hero has resistance potion => ask him if he wants to use it
    QString text = actual_hero->getName() + ": " + "Would you like to use your resistance potion to render the spell which has been cast on you ineffective?";
    bool use_resistance_potion = (execDialogOkCancel("Use potion?", text) == QDialog::Accepted);

    if (use_resistance_potion)
    {
        const TreasureCard* resistance_potion_p = actual_hero->getInventory().getTreasureCard(
                TreasureDataTypes::TREASURE_CARD_RESISTANCE_POTION);
        TreasureCard resistance_potion = *resistance_potion_p;

        // hero uses the resistance potion => remove it from his inventory
        actual_hero->getInventory().removeTreasureCard(*resistance_potion_p);

        // put treasure card back into the depositions
        _level->_treasure_card_deposition.push_back(resistance_potion);
    }

    return use_resistance_potion;
}

/*!
 * \return Always true.
 */
bool HeroQuestLevelWindow::checkDefend(Hero* hero)
{
    return checkDefend(QVariant::fromValue(hero));
}

/*!
 * \return True, if the hero really dies, false otherwise.
 */
bool HeroQuestLevelWindow::checkDeath(Hero* hero)
{
    return checkDeath(QVariant::fromValue(hero));
}

bool HeroQuestLevelWindow::save(SaveContext& save_context) const
{
    SaveContext::OpenChapter open_chapter(save_context, "HeroQuestLevelWindow");

    _game_state->save(save_context);
    _spell_card_storage->save(save_context);
    _treasure_card_storage->save(save_context);
    _equipment_card_storage->save(save_context);
    _hero_camp.save(save_context);

    _level->save(save_context);
    _playground->save(save_context);

    return !save_context.fail();
}

bool HeroQuestLevelWindow::load(LoadContext& load_context)
{
    LoadContext::OpenChapter open_chapter(load_context, "HeroQuestLevelWindow");

    // Note that _game_state, ..., _hero_camp have been loaded already before.

    DV(("loading level..."));
    _level->load(load_context);

    DV(("loading playground..."));
    _playground->load(load_context);

    _user_interaction_mode = USER_INTERACTION_NORMAL;
    _user_selected_node_id = NodeID(-1, -1);
    _user_selected_two_node_ids = make_pair(NodeID(-1, -1), NodeID(-1, -1));

    return !load_context.fail();
}

bool HeroQuestLevelWindow::loadImagesAndAdjustPointers()
{
    _playground->loadImagesAndAdjustPointers();

    // Update _treasure_image in all TreasureDescription instances!
    //
    // Level:
    // - std::list<TreasureCard> _treasure_card_stock;
    // - std::list<TreasureCard> _treasure_card_deposition;
    _level->updateTreasureImages();
    //
    // Heroes' inventories:
    // - std::set<TreasureCard> _treasure_cards;
    for (uint i = 0; i < _heroes.size(); ++i)
    {
        _heroes[i]->updateTreasureImages();
    }

    return true;
}

/*!
 * Checks things which must be clarified before hero defends himself.
 *
 * \return True, if hero can defend himself.
 */
bool HeroQuestLevelWindow::checkDefend(QVariant hero)
{
    if (!currentThreadIsGUIThread())
    {
        bool ret;
        QMetaObject::invokeMethod(this, "checkDefend", Qt::BlockingQueuedConnection, Q_RETURN_ARG(bool, ret), Q_ARG(QVariant, hero));
        return ret;
    }

    Hero* actual_hero = hero.value<Hero*>();
    if (actual_hero == 0)
    {
        cout << "HeroQuest::checkDefend: actual_hero == 0" << endl;
        return false;
    }

    if (!actual_hero->getInventory().hasImmunizationPotion())
    {
        return true;
    }

    // hero has immunization potion => ask him if he wants to use it
    QString text = actual_hero->getName() + ": " + "Would you like to use your immunization potion, so that you can throw two additional combat dice during your next defence?";
    bool use_immunization_potion = (execDialogOkCancel("Use potion?", text) == QDialog::Accepted);

    if (use_immunization_potion)
    {
        const TreasureCard* immunization_potion_p = actual_hero->getInventory().getTreasureCard(
                TreasureDataTypes::TREASURE_CARD_IMMUNIZATION_POTION);
        TreasureCard immunization_potion = *immunization_potion_p;
        immunization_potion.executeInventoryActions(actual_hero);

        // hero uses the immunization potion => remove it from his inventory
        actual_hero->getInventory().removeTreasureCard(*immunization_potion_p);

        // put treasure card back into the deposition
        _level->_treasure_card_deposition.push_back(immunization_potion);
    }

    return true;
}

/*!
 * Checks things which must be clarified before hero dies.
 *
 * \return True, if hero will really die, false otherwise.
 */
bool HeroQuestLevelWindow::checkDeath(QVariant hero)
{
    if (!currentThreadIsGUIThread())
    {
        bool ret;
        QMetaObject::invokeMethod(this, "checkDeath", Qt::BlockingQueuedConnection, Q_RETURN_ARG(bool, ret), Q_ARG(QVariant, hero));
        return ret;
    }

    Hero* actual_hero = hero.value<Hero*>();
    if (actual_hero == 0)
    {
        cout << "HeroQuest::checkDeath: actual_hero == 0" << endl;
        return false;
    }

    if (!actual_hero->getInventory().hasHealingPotion())
    {
        return true;
    }

    // hero has healing potion => ask him if he wants to use it
    QString text = actual_hero->getName() + ": " + "Would you like to use your healing potion, so that you regain up to 4 life points?";
    bool use_healing_potion = (execDialogOkCancel("Use potion?", text) == QDialog::Accepted);

    if (use_healing_potion)
    {
        const TreasureCard* healing_potion_p = actual_hero->getInventory().getTreasureCard(
                TreasureDataTypes::TREASURE_CARD_HEALING_POTION);
        TreasureCard healing_potion = *healing_potion_p;
        healing_potion.executeInventoryActions(actual_hero);

        // hero uses the immunization potion => remove it from his inventory
        actual_hero->getInventory().removeTreasureCard(*healing_potion_p);

        // put treasure card back into the deposition
        _level->_treasure_card_deposition.push_back(healing_potion);
    }

    return false;
}

bool HeroQuestLevelWindow::chooseLoadGameFilename(QWidget* parent, QString* filename)
{
    QFileDialog load_file_dialog(parent);
    load_file_dialog.setNameFilter("HeroQuest Saved Games (*.hqsg)");
    load_file_dialog.setAcceptMode(QFileDialog::AcceptOpen);
    load_file_dialog.setDefaultSuffix("hqsg");
    load_file_dialog.setFileMode(QFileDialog::ExistingFile);

    QStringList filenames;
    if (load_file_dialog.exec())
        filenames = load_file_dialog.selectedFiles();

    if (filenames.size() != 1)
    {
        DVX(("Not exactly one file selected as load game file."));
        return false;
    }

    *filename = filenames[0];
    return true;
}

bool HeroQuestLevelWindow::chooseSaveGameFilename(QWidget* parent, QString* filename)
{
    QFileDialog save_file_dialog(parent);
    save_file_dialog.setNameFilter("HeroQuest Saved Games (*.hqsg)");
    save_file_dialog.setAcceptMode(QFileDialog::AcceptSave);
    save_file_dialog.setDefaultSuffix("hqsg");

    QStringList filenames;
    if (save_file_dialog.exec())
        filenames = save_file_dialog.selectedFiles();

    if (filenames.size() != 1)
    {
        DVX(("Not exactly one file selected as save game file."));
        return false;
    }

    *filename = filenames[0];
    return true;
}

void HeroQuestLevelWindow::paintEvent(QPaintEvent* event)
{
    DV(("HeroQuestLevelWindow::paintEvent BEGIN"));
    _is_painting = true;
    QMainWindow::paintEvent(event);
    _is_painting = false;
    DV(("HeroQuestLevelWindow::paintEvent END"));
}

/*!
 * Called only via menu.
 */
bool HeroQuestLevelWindow::loadGame()
{
    return loadGame("");
}

/*!
 * Loads the game from a file. If filename is empty, the user chooses the file.
 *
 * @param filename Name of file to load from.
 *
 * \return True, if loading succeeded.
 */
bool HeroQuestLevelWindow::loadGame(const QString& filename)
{
    DVX(("loadGame %s", qPrintable(filename)));

    QString filename_local = filename;
    if (filename_local.length() == 0)
    {
        if (!chooseLoadGameFilename(this, &filename_local))
            return false;
    }

    LoadContext load_context(filename_local,
            Preferences::ENABLE_SAVE_LOAD_LOG ? LoadContext::ENABLE_LOG : LoadContext::DISABLE_LOG);
    if (load_context.fail())
        return false;

    {
        DV(
                ("HeroQuestLevelWindow::loadGame: suppress paint events while load game"));
        EventBlockingContext load_game_context(this); // makes sure that no paint event is running now

        if (!_game_state->load(load_context))
        {
            return false;
        }
        if (!_spell_card_storage->load(load_context))
        {
            return false;
        }
        if (!_treasure_card_storage->load(load_context))
        {
            return false;
        }
        if (!_equipment_card_storage->load(load_context))
        {
            return false;
        }

        // level
        createLevel();

        DV(("loading hero_camp..."));
        if (!_hero_camp.load(load_context, *_game_state))
        {
            return false;
        }

        // update heroes from camp
        _hero_camp.getHeroes(&_heroes);

        if (!load(load_context)) // this is required to set level->_acting_heroes correctly before creating the statistic panes
        {
            DVX(("Could not load game from file \"%s\".", qPrintable(filename_local)));
            return false;
        }

        // hero static panes
        addInfoPaneToCentralWidget();
        addHeroStatisticPanes();

        DV(("HeroQuestLevelWindow::loadGame: loadImagesAndAdjustPointers"));
        if (!loadImagesAndAdjustPointers())
        {
            DVX(
                    ("Could not adjust pointers after load from file \"%s\".", qPrintable(filename_local)));
            return false;
        }
        DV(
                ("HeroQuestLevelWindow::loadGame: loadImagesAndAdjustPointers FINISHED"));

        // update graphical user interface
        initActionPane();

        updateButtons();
        updateHeroStatisticPanes();
    }

    // trigger repaint
    update();

    return true;
}

Level* HeroQuestLevelWindow::createLevelFromLevelID(GameState::LevelID level_id, size_t num_heroes)
{
    Level* level = 0;

    if (level_id == GameState::LEVEL_THE_PROBATION)
    {
        level = new Level01TheProbation(num_heroes);
    }
    else if (level_id == GameState::LEVEL_SAVING_SIR_RAGNAR)
    {
        level = new Level02SavingSirRagnar(num_heroes);
    }
    else
    {
        DVX(("Unknown level ID \"%d\"!", (uint)level_id));
    }

    return level;
}

bool HeroQuestLevelWindow::createLevel()
{
    delete _level;
    _level = createLevelFromLevelID(_game_state->_current_level, _game_state->_hero_names.size());
    return _level != 0;
}

/*!
 * Saves the game in a file on hard disk (which the user has to choose).
 *
 * \return True, saving succeeded.
 */
bool HeroQuestLevelWindow::saveGameChooseFilename()
{
    QString filename;
    if (!chooseSaveGameFilename(this, &filename))
        return false;

    SaveContext save_context(filename,
            Preferences::ENABLE_SAVE_LOAD_LOG ? SaveContext::ENABLE_LOG : SaveContext::DISABLE_LOG);
    if (save_context.fail())
        return false;

    if (!save(save_context))
    {
        DVX(("Could not save game to file \"%s\".", qPrintable(filename)));
        return false;
    }

    return true;
}

bool HeroQuestLevelWindow::useInventoryItem(const InventoryItem& inventory_item)
{
    Hero* currentlyActingHero = _level->getCurrentlyActingHero();

    if (inventory_item.isTreasureCard())
    {
        const TreasureCard& treasure_card = dynamic_cast<const TreasureCard&>(inventory_item);
        bool treasure_card_used = useTreasureCard(treasure_card);
        if (treasure_card_used)
        {
            // remove from inventory
            currentlyActingHero->getInventory().removeTreasureCard(treasure_card);
        }
        return treasure_card_used;
    }

    if (inventory_item.isSpellCard())
    {
        const SpellCard& spell_card = dynamic_cast<const SpellCard&>(inventory_item);
        bool spell_card_used = useSpellCard(spell_card);
        if (spell_card_used)
        {
            // remove from hero's inventory
            currentlyActingHero->getInventory().removeSpellCard(spell_card);
        }
        return spell_card_used;
    }

    if (inventory_item.isEquipmentCard())
    {
        const EquipmentCard& equipment_card = dynamic_cast<const EquipmentCard&>(inventory_item);
        bool equipment_card_used = useEquipmentCard(equipment_card);
        if (equipment_card_used)
        {
            EquipmentCard equipment_card_copy(equipment_card);

            // remove from hero's inventory
            currentlyActingHero->getInventory().removeEquipmentCard(equipment_card);
            // add back to equipment card stock
            EquipmentCardStorage::instance->addEquipmentCardToStock(equipment_card_copy);
        }
        return equipment_card_used;
    }

    DVX(("HeroQuest::useInventoryItem: Case not yet implemented"));
    return false;
}

bool HeroQuestLevelWindow::useTreasureCard(const TreasureCard& treasure_card)
{
    return treasure_card.executeInventoryActions();
}

bool HeroQuestLevelWindow::useSpellCard(const SpellCard& spell_card)
{
    bool spell_card_used = spell_card.execute();

    if (spell_card_used)
    {
        // action states: hero has cast a spell
        _level->getCurrentHeroActionStates()._has_cast_spell = true;

        updateButtons();
    }

    return spell_card_used;
}

bool HeroQuestLevelWindow::useEquipmentCard(const EquipmentCard& equipment_card)
{
    bool equipment_card_used = equipment_card.execute();

    if (equipment_card_used)
    {
        // action states: hero has attacked or done sth. equivalent
        --(_level->getCurrentHeroActionStates()._num_attacks_left);

        updateButtons();
    }

    return equipment_card_used;
}

/*!
 * @param creature Creature who tries to wake up by throwing a movement die.
 * @return True if thrown a 6; false otherwise.
 */
bool HeroQuestLevelWindow::creatureTriesToWakeUp(Creature& creature)
{
    // throw a movement die; waking up when thrown a 6
    vector<uint> dice_results;
    _dice_roll_pane->throwMovementDice(creature, 1, &dice_results);
    delay(Level::ATTACK_DELAY);
    if (dice_results[0] == 6)
    {
        playSoundOnce(SoundManager::SOUND_ATTACK_DEFENDED);
        return true;
    }
    return false;
}

HeroQuestLevelWindow::UserInteractionMode HeroQuestLevelWindow::getUserInteractionMode() const
{
    return _user_interaction_mode;
}

/*!
 * @param related_hero is used for USER_INTERACTION_SELECT_CREATURE_IN_HORIZONTAL_OR_VERTICAL_LINE_OF_SIGHT
 * or for USER_INTERACTION_SELECT_ADJACENT_PIT_TRAP
 */
void HeroQuestLevelWindow::setUserInteractionMode(UserInteractionMode user_interaction_mode, const Hero* related_hero)
{
    _user_interaction_mode = user_interaction_mode;

    switch (_user_interaction_mode)
    {
        case USER_INTERACTION_NORMAL:
        {
            // make buttons active
            updateButtons();
            updateHeroStatisticPanes();
            // enable save
            _save_action->setEnabled(true);
            // playground
            _playground->setActionMode(Playground::ACTION_MODE_MOVE_OR_ATTACK_OR_OPEN_DOOR);
            // hint
            _hint_pane->reset();
        }
            break;

        case USER_INTERACTION_SELECT_HERO:
        {
            // make buttons inactive
            updateButtons();
            updateHeroStatisticPanes();
            // forbid save
            _save_action->setEnabled(false);
            // playground
            _playground->setActionMode(Playground::ACTION_MODE_SELECT_FIELD_OR_DOOR);
            // hint
            _hint_pane->setHint("Select a hero (RMB)!", HintPane::ALLOW_ABORT);

            // init result
            _user_selected_node_id = NodeID(-1, -1);
        }
            break;

        case USER_INTERACTION_SELECT_CREATURE:
        {
            // make buttons inactive
            updateButtons();
            updateHeroStatisticPanes();
            // forbid save
            _save_action->setEnabled(false);
            // playground
            _playground->setActionMode(Playground::ACTION_MODE_SELECT_FIELD_OR_DOOR);
            // hint
            _hint_pane->setHint("Select a creature (RMB)!", HintPane::ALLOW_ABORT);

            // init result
            _user_selected_node_id = NodeID(-1, -1);
        }
            break;

        case USER_INTERACTION_SELECT_CREATURE_OR_DOOR:
        {
            // make buttons inactive
            updateButtons();
            updateHeroStatisticPanes();
            // forbid save
            _save_action->setEnabled(false);
            // playground
            _playground->setActionMode(Playground::ACTION_MODE_SELECT_FIELD_OR_DOOR);
            // hint
            _hint_pane->setHint("Select a creature (RMB) or a door (LMB)!", HintPane::ALLOW_ABORT);

            // init result
            _user_selected_node_id = NodeID(-1, -1);
            _user_selected_two_node_ids = make_pair(NodeID(-1, -1), NodeID(-1, -1));
        }
            break;

        case USER_INTERACTION_SELECT_CREATURE_IN_HORIZONTAL_OR_VERTICAL_LINE_OF_SIGHT:
        {
            // make buttons inactive
            updateButtons();
            updateHeroStatisticPanes();
            // forbid save
            _save_action->setEnabled(false);
            // playground
            _playground->setActionMode(Playground::ACTION_MODE_SELECT_FIELD_IN_VISUAL_LINE_OF_SIGHT, related_hero);
            // hint
            _hint_pane->setHint("Select a creature (RMB)!", HintPane::ALLOW_ABORT);

            // init result
            _user_selected_node_id = NodeID(-1, -1);
        }
            break;

        case USER_INTERACTION_SELECT_ADJACENT_PIT_TRAP:
        {
            // make buttons inactive
            updateButtons();
            updateHeroStatisticPanes();
            // forbid save
            _save_action->setEnabled(false);
            // playground
            _playground->setActionMode(Playground::ACTION_MODE_SELECT_ADJACENT_FIELD, related_hero);
            // hint
            _hint_pane->setHint("Select a pit trap (LMB)!", HintPane::ALLOW_ABORT);

            // init result
            _user_selected_node_id = NodeID(-1, -1);
        }
            break;

        default:
            DVX(("HeroQuestLevelWindow::setUserInteractionMode: user_interaction_mode NYI!"))
            ;
            break;
    }

    _playground->update();
}

void HeroQuestLevelWindow::setUserSelectedNodeID(const NodeID& node_id)
{
    _user_selected_node_id = node_id;
}

void HeroQuestLevelWindow::setUserSelectedTwoNodeIDs(const pair<NodeID, NodeID>& node_ids)
{
    _user_selected_two_node_ids = node_ids;
}

Hero* HeroQuestLevelWindow::userSelectsHero()
{
    // switch to hero selection mode
    HeroQuestLevelWindow::_hero_quest->setUserInteractionMode(HeroQuestLevelWindow::USER_INTERACTION_SELECT_HERO);

    // setup internal event loop to let the user select a field on the board
    Hero* hero = 0;
    QEventLoop loop;
    bool hero_chosen = false;
    _user_abort = false;
    while (!(hero_chosen || _user_abort))
    {
        loop.processEvents();

        // node_id chosen which holds a hero?
        Creature* creature = _playground->getCreature(_user_selected_node_id);
        if (creature != 0 && creature->isHero())
        {
            hero = dynamic_cast<Hero*>(creature);
            hero_chosen = true;
        }
    }

    // switch back to normal mode
    HeroQuestLevelWindow::_hero_quest->setUserInteractionMode(HeroQuestLevelWindow::USER_INTERACTION_NORMAL);

    return hero;
}

Creature* HeroQuestLevelWindow::userSelectsCreature()
{
    // switch to creature selection mode
    HeroQuestLevelWindow::_hero_quest->setUserInteractionMode(HeroQuestLevelWindow::USER_INTERACTION_SELECT_CREATURE);

    // setup internal event loop to let the user select a creature on the board
    Creature* creature = 0;
    QEventLoop loop;
    bool creature_chosen = false;
    _user_abort = false;
    while (!(creature_chosen || _user_abort))
    {
        loop.processEvents();

        // node_id chosen which holds a creature?
        creature = _playground->getCreature(_user_selected_node_id);
        if (creature != 0)
        {
            creature_chosen = true;
        }
    }

    // switch back to normal mode
    HeroQuestLevelWindow::_hero_quest->setUserInteractionMode(HeroQuestLevelWindow::USER_INTERACTION_NORMAL);

    return creature;
}

Creature* HeroQuestLevelWindow::userSelectsCreatureInHorizontalOrVerticalLineOfSightOf(const Hero& attacker)
{
    // switch to creature selection mode in horizontal or vertical line of sight
    HeroQuestLevelWindow::_hero_quest->setUserInteractionMode(
            HeroQuestLevelWindow::USER_INTERACTION_SELECT_CREATURE_IN_HORIZONTAL_OR_VERTICAL_LINE_OF_SIGHT, &attacker);

    // setup internal event loop to let the user select a creature on the board
    Creature* creature = 0;
    QEventLoop loop;
    bool creature_chosen = false;
    _user_abort = false;
    while (!(creature_chosen || _user_abort))
    {
        loop.processEvents();

        // node_id chosen which holds a creature in horizontal or vertical line of sight of attacker
        // (otherwise the creature returned by "getCreature" would be 0 in this mode)?
        creature = _playground->getCreature(_user_selected_node_id);
        if (creature != 0)
        {
            creature_chosen = true;
        }
    }

    // switch back to normal mode
    HeroQuestLevelWindow::_hero_quest->setUserInteractionMode(HeroQuestLevelWindow::USER_INTERACTION_NORMAL);

    return creature;
}

PitTrap* HeroQuestLevelWindow::userSelectsAdjacentPitTrap(const Hero& hero)
{
    // switch to adjacent pit trap selection mode
    HeroQuestLevelWindow::_hero_quest->setUserInteractionMode(
            HeroQuestLevelWindow::USER_INTERACTION_SELECT_ADJACENT_PIT_TRAP, &hero);

    // setup internal event loop to let the user select a pit trap on the board
    PitTrap* pit_trap = 0;
    QEventLoop loop;
    bool pit_trap_chosen = false;
    _user_abort = false;
    while (!(pit_trap_chosen || _user_abort))
    {
        loop.processEvents();

        // node_id chosen which holds a pit trap adjacent to hero
        // (otherwise the pit trap returned by "getPitTrap" would be 0 in this mode)?
        pit_trap = dynamic_cast<PitTrap*>(_playground->getTrap(_user_selected_node_id));
        if (pit_trap != 0)
        {
            pit_trap_chosen = true;
        }
    }

    // switch back to normal mode
    HeroQuestLevelWindow::_hero_quest->setUserInteractionMode(HeroQuestLevelWindow::USER_INTERACTION_NORMAL);

    return pit_trap;
}

void HeroQuestLevelWindow::userSelectsCreatureOrDoor(Creature** creature, Door** door)
{
    // switch to creature selection mode
    HeroQuestLevelWindow::_hero_quest->setUserInteractionMode(
            HeroQuestLevelWindow::USER_INTERACTION_SELECT_CREATURE_OR_DOOR);

    // setup internal event loop to let the user select a creature on the board
    Creature* creature_local = 0;
    const Door* door_local = 0;
    QEventLoop loop;
    bool creature_or_door_chosen = false;
    _user_abort = false;
    while (!(creature_or_door_chosen || _user_abort))
    {
        loop.processEvents();

        // node_id chosen which holds a hero?
        creature_local = _playground->getCreature(_user_selected_node_id);
        if (creature_local != 0)
        {
            creature_or_door_chosen = true;
            *creature = creature_local;
        }

        // two nodes chosen which represent a door?
        door_local = _level->getDoor(_user_selected_two_node_ids);
        if (door_local != 0)
        {
            creature_or_door_chosen = true;
            *door = const_cast<Door*>(door_local);
        }
    }

    // switch back to normal mode
    HeroQuestLevelWindow::_hero_quest->setUserInteractionMode(HeroQuestLevelWindow::USER_INTERACTION_NORMAL);
}

void HeroQuestLevelWindow::throwAttackDice(Creature& attacker, std::vector<DiceRollPane::AttackDiceResult>* dice_results)
{
    throwAttackDice(QVariant::fromValue(&attacker), QVariant::fromValue(dice_results));
}

void HeroQuestLevelWindow::throwAttackDice(Creature& attacker, uint num_dice, std::vector<DiceRollPane::AttackDiceResult>* dice_results)
{
    throwAttackDice(QVariant::fromValue(&attacker), QVariant::fromValue(num_dice), QVariant::fromValue(dice_results));
}

void HeroQuestLevelWindow::throwAttackDice(QVariant attacker, QVariant dice_results)
{
    if (!currentThreadIsGUIThread())
    {
        QMetaObject::invokeMethod(this, "throwAttackDice", Qt::BlockingQueuedConnection, Q_ARG(QVariant, attacker), Q_ARG(QVariant, dice_results));
        return;
    }

    Creature* actual_attacker = attacker.value<Creature*>();
    if (actual_attacker == 0)
    {
        cout << "HeroQuest::throwAttackDice: actual_attacker == 0" << endl;
        return;
    }

    std::vector<DiceRollPane::AttackDiceResult>* actual_dice_results = dice_results.value<std::vector<DiceRollPane::AttackDiceResult>*>();
    if (actual_dice_results == 0)
    {
        cout << "HeroQuest::throwAttackDice: actual_dice_results == 0" << endl;
        return;
    }

    _dice_roll_pane->throwAttackDice(*actual_attacker, actual_dice_results);
    update();
}

void HeroQuestLevelWindow::throwAttackDice(QVariant attacker, QVariant num_dice, QVariant dice_results)
{
    if (!currentThreadIsGUIThread())
    {
        QMetaObject::invokeMethod(this, "throwAttackDice", Qt::BlockingQueuedConnection, Q_ARG(QVariant, attacker), Q_ARG(QVariant, num_dice), Q_ARG(QVariant, dice_results));
        return;
    }

    Creature* actual_attacker = attacker.value<Creature*>();
    if (actual_attacker == 0)
    {
        cout << "HeroQuest::throwAttackDice: actual_attacker == 0" << endl;
        return;
    }

    uint actual_num_dice = num_dice.value<uint>();

    std::vector<DiceRollPane::AttackDiceResult>* actual_dice_results = dice_results.value<std::vector<DiceRollPane::AttackDiceResult>*>();
    if (actual_dice_results == 0)
    {
        cout << "HeroQuest::throwAttackDice: actual_dice_results == 0" << endl;
        return;
    }

    _dice_roll_pane->throwAttackDice(*actual_attacker, actual_num_dice, actual_dice_results);
}

void HeroQuestLevelWindow::throwDefendDice(Creature& defender, std::vector<DiceRollPane::AttackDiceResult>* dice_results)
{
    throwDefendDice(QVariant::fromValue(&defender), QVariant::fromValue(dice_results));
}

void HeroQuestLevelWindow::throwDefendDice(Creature& defender, uint num_dice,
        std::vector<DiceRollPane::AttackDiceResult>* dice_results)
{
    throwDefendDice(QVariant::fromValue(&defender), QVariant::fromValue(num_dice), QVariant::fromValue(dice_results));
}

void HeroQuestLevelWindow::throwDefendDice(QVariant defender, QVariant dice_results)
{
    if (!currentThreadIsGUIThread())
    {
        QMetaObject::invokeMethod(this, "throwDefendDice", Qt::BlockingQueuedConnection, Q_ARG(QVariant, defender), Q_ARG(QVariant, dice_results));
        return;
    }

    Creature* actual_defender = defender.value<Creature*>();
    if (actual_defender == 0)
    {
        cout << "HeroQuest::throwDefendDice: actual_defender == 0" << endl;
        return;
    }

    std::vector<DiceRollPane::AttackDiceResult>* actual_dice_results = dice_results.value<std::vector<DiceRollPane::AttackDiceResult>*>();
    if (actual_dice_results == 0)
    {
        cout << "HeroQuest::throwDefendDice: actual_dice_results == 0" << endl;
        return;
    }

    _dice_roll_pane->throwDefendDice(*actual_defender, actual_dice_results);
    update();
}

void HeroQuestLevelWindow::throwDefendDice(QVariant defender, QVariant num_dice, QVariant dice_results)
{
    if (!currentThreadIsGUIThread())
    {
        QMetaObject::invokeMethod(this, "throwDefendDice", Qt::BlockingQueuedConnection, Q_ARG(QVariant, defender),
                Q_ARG(QVariant, dice_results));
        return;
    }

    Creature* actual_defender = defender.value<Creature*>();
    if (actual_defender == 0)
    {
        cout << "HeroQuest::throwDefendDice: actual_defender == 0" << endl;
        return;
    }

    uint actual_num_dice = num_dice.value<uint>();

    std::vector<DiceRollPane::AttackDiceResult>* actual_dice_results = dice_results.value<
            std::vector<DiceRollPane::AttackDiceResult>*>();
    if (actual_dice_results == 0)
    {
        cout << "HeroQuest::throwDefendDice: actual_dice_results == 0" << endl;
        return;
    }

    _dice_roll_pane->throwDefendDice(*actual_defender, actual_num_dice, actual_dice_results);
    update();
}

void HeroQuestLevelWindow::execDialogOk(QVariant title, QVariant text)
{
    if (!currentThreadIsGUIThread())
    {
        int ret_val;
        QMetaObject::invokeMethod(this, "execDialogOk", Qt::BlockingQueuedConnection, Q_RETURN_ARG(int, ret_val),
                Q_ARG(QVariant, title), Q_ARG(QVariant, text));
        return;
    }

    QString actual_title = title.value<QString>();
    QString actual_text = text.value<QString>();

    DialogOk* dialog_ok = new DialogOk(actual_title, actual_text);
    dialog_ok->exec();
    delete dialog_ok;
}

int HeroQuestLevelWindow::execDialogOkCancel(QVariant title, QVariant text)
{
    if (!currentThreadIsGUIThread())
    {
        int ret_val;
        QMetaObject::invokeMethod(this, "execDialogOkCancel", Qt::BlockingQueuedConnection, Q_RETURN_ARG(int, ret_val),
                Q_ARG(QVariant, title), Q_ARG(QVariant, text));
        return ret_val;
    }

    QString actual_title = title.value<QString>();
    QString actual_text = text.value<QString>();

    DialogOkCancel* dialog_ok_cancel = new DialogOkCancel(actual_title, actual_text);
    int ret_val = dialog_ok_cancel->exec();
    delete dialog_ok_cancel;

    return ret_val;
}

void HeroQuestLevelWindow::execDialogOpenChest(QVariant treasure_description)
{
    if (!currentThreadIsGUIThread())
    {
        QMetaObject::invokeMethod(this, "execDialogOpenChest", Qt::BlockingQueuedConnection,
                Q_ARG(QVariant, treasure_description));
        return;
    }

    TreasureDescription* actual_treasure_description = treasure_description.value<TreasureDescription*>();
    if (actual_treasure_description == 0)
        return;

    DialogOpenChest* dialog_open_chest = new DialogOpenChest(*actual_treasure_description);
    dialog_open_chest->exec();
    delete dialog_open_chest;
}

void HeroQuestLevelWindow::execDialogTreasureCard(QVariant treasure_card)
{
    if (!currentThreadIsGUIThread())
    {
        QMetaObject::invokeMethod(this, "execDialogTreasureCard", Qt::BlockingQueuedConnection, Q_ARG(QVariant, treasure_card));
        return;
    }

    TreasureCard* actual_treasure_card = treasure_card.value<TreasureCard*>();
    if (actual_treasure_card == 0)
        return;

    DialogTreasureCard* dialog_treasure_card = new DialogTreasureCard(*actual_treasure_card);
    dialog_treasure_card->exec();
    delete dialog_treasure_card;
}

void HeroQuestLevelWindow::execDialogInventory()
{
    if (!currentThreadIsGUIThread())
    {
        QMetaObject::invokeMethod(this, "execDialogInventory", Qt::BlockingQueuedConnection);
        return;
    }

    DialogInventory* dialog_inventory = new DialogInventory();
    dialog_inventory->exec();
    delete dialog_inventory;
}

Playground* HeroQuestLevelWindow::getPlayground()
{
    return _playground;
}

Level* HeroQuestLevelWindow::getLevel()
{
    return _level;
}

DiceRollPane* HeroQuestLevelWindow::getDiceRollPane()
{
    return _dice_roll_pane;
}

ButtonPane* HeroQuestLevelWindow::getButtonPane()
{
    return _button_pane;
}

vector<HeroStatisticPane*>& HeroQuestLevelWindow::getHeroStatisticPanes()
{
    return _hero_statistic_panes;
}

Creature* HeroQuestLevelWindow::getCreature(uint referencing_id)
{
    Creature* creature = 0;

    // Here we don't know if the queried Creature is a Hero or an Monster,
    // so we have to iterate over all of them.

    // check Hero
    for (vector<Hero*>::iterator it = _heroes.begin(); it != _heroes.end(); ++it)
    {
        if ((*it)->getReferencingID() == referencing_id)
            return *it;
    }

    // check Monster
    creature = _level->getMonster(referencing_id);

    if (creature != 0)
        return creature;

    DVX(("Did not find queried creature with ref ID %d", referencing_id));
    return 0;
}

Hero* HeroQuestLevelWindow::getHero(uint referencing_id)
{
    Creature* creature = getCreature(referencing_id);
    return dynamic_cast<Hero*>(creature);
}

bool HeroQuestLevelWindow::eventFilter(QObject* /*object*/, QEvent* event)
{
    // ============= event blocking =============
    // block input events?
    if ((dynamic_cast<QInputEvent*>(event) != 0)
            && ((_event_blocking_bitmask & EventBlockingContext::EVENT_BLOCKING_INPUT_EVENTS)
                    != EventBlockingContext::EVENT_BLOCKING_NONE))
    {
        // block
        DVX(("input event blocked"));
        return true;
    }

    // block paint events?
    if ((event->type() == QEvent::Paint)
            && ((_event_blocking_bitmask & EventBlockingContext::EVENT_BLOCKING_PAINT_EVENTS)
                    != EventBlockingContext::EVENT_BLOCKING_NONE))
    {
        // block
        return true;
    }
    
    // ============= event handling =============
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent* ke = static_cast<QKeyEvent*>(event);
        if (ke->key() == Qt::Key_Escape)
        {
            if (_playground != 0
                    && (_playground->getActionMode() == Playground::ACTION_MODE_SELECT_FIELD_OR_DOOR
                            || _playground->getActionMode()
                                    == Playground::ACTION_MODE_SELECT_FIELD_IN_VISUAL_LINE_OF_SIGHT
                            || _playground->getActionMode() == Playground::ACTION_MODE_SELECT_ADJACENT_FIELD))
            {
                // cancel selecting a field or door
                _user_abort = true;
            }
            else
            {
                // quit this level as failed
                close();
            }
            return true;
        }
    }

    // event is not handled here => forward the event to follow-up handlers
    return false;
}

void HeroQuestLevelWindow::closeEvent(QCloseEvent* event)
{
    // exit level as failed when window is simply closed
    bool really_exit_as_failed = (execDialogOkCancel("Really exit level as failed?",
            "Press Ok to exit this level and lose the game.") == QDialog::Accepted);
    if (really_exit_as_failed)
    {
        exitLost();
    }

    // note that we never call QMainWindow::closeEvent();
    // we use app.exit() instead
    event->ignore();
}

/*!
 * If filename is not empty, load level and heroes, and continue playing.
 * Otherwise, create level, add heroes, and initialize the hero action states, starting the first round.
 * As a prerequisite to this, _game_state must be available.
 * TODO: instead of using the _game_state, explicitly give the required parameters.
 *
 * @param filename
 */
void HeroQuestLevelWindow::startGame(const QString& filename)
{
    DV(("HeroQuestLevelWindow::startGame"));

    if (filename.length() != 0)
    {
        loadGame(filename);
    }
    else
    {
        createLevel();
        _level->setActingHeroes(getHeroes());
        addHeroStatisticPanes(); // assumes that _acting_heroes have already been set!
        _level->create();
        _level->startRound();
    }
}

/*!
 * Adds hero statistic panes to the info area.
 *
 * Requires that _level->_acting_heroes is set already.
 */
void HeroQuestLevelWindow::addHeroStatisticPanes()
{
#ifdef DEBUG_PANE_SIZES
    DVX(
            ("addHeroStatisticPanes _info_pane->layout() content before clear = %d", LayoutHelper::getNumWidgets(_info_pane->layout())));
#endif
    
    // clean internal data structures and GUI
    QVBoxLayout* info_pane_layout = dynamic_cast<QVBoxLayout*>(_info_pane->layout());
    while (info_pane_layout->itemAt(0) != 0)
    {
        info_pane_layout->removeItem(info_pane_layout->itemAt(0));
    }
    for (vector<HeroStatisticPane*>::iterator it = _hero_statistic_panes.begin(); it != _hero_statistic_panes.end();
            ++it)
    {
        delete (*it);
    }
    _hero_statistic_panes.clear();

#ifdef DEBUG_PANE_SIZES
    DVX(
            ("addHeroStatisticPanes _info_pane->layout() content after clear = %d", LayoutHelper::getNumWidgets(_info_pane->layout())));
#endif

    // --- add statistic panes to the info area ---
    for (vector<Hero*>::const_iterator it_hero = _level->getActingHeroes().begin(); it_hero != _level->getActingHeroes().end(); ++it_hero)
    {
        addHeroStatisticPane(*it_hero);
    }

    adjustInfoPaneHeight();

#ifdef DEBUG_PANE_SIZES
    DVX(
            ("addHeroStatisticPanes _info_pane->layout() content after add heroes = %d", LayoutHelper::getNumWidgets(_info_pane->layout())));
#endif
}

void HeroQuestLevelWindow::connectActionButtons()
{
    connect(HeroQuestLevelWindow::_hero_quest->getButtonPane()->getButton("Search Secret Doors / Traps"), SIGNAL(clicked()), this, SLOT(searchTrapsButtonClicked()));
    connect(HeroQuestLevelWindow::_hero_quest->getButtonPane()->getButton("Search Secret Doors / Traps"),
            SIGNAL(pressed()), this, SLOT(searchTrapsButtonPressed()));
    connect(HeroQuestLevelWindow::_hero_quest->getButtonPane()->getButton("Search Secret Doors / Traps"),
            SIGNAL(released()), this, SLOT(searchTrapsButtonReleased()));
    connect(HeroQuestLevelWindow::_hero_quest->getButtonPane()->getButton("Search Treasures"), SIGNAL(clicked()), this, SLOT(searchTreasuresButtonClicked()));
    connect(HeroQuestLevelWindow::_hero_quest->getButtonPane()->getButton("Delay Turn"), SIGNAL(clicked()), this, SLOT(delayTurnButtonClicked()));
    connect(HeroQuestLevelWindow::_hero_quest->getButtonPane()->getButton("End Turn"), SIGNAL(clicked()), this, SLOT(endTurnButtonClicked()));
}

/*!
 * Sets fixed height of _info_pane according to the height of the contained HeroStatisticPanes.
 */
void HeroQuestLevelWindow::adjustInfoPaneHeight(QSize fixed_size)
{
    if (fixed_size.width() == 0 || fixed_size.height() == 0)
    {
        fixed_size = _info_pane->layout()->sizeHint();
    }

    _info_pane->setFixedSize(fixed_size);

#ifdef DEBUG_PANE_SIZES
    DVX(("============================================"));
    DVX(
            ("adjustInfoPaneHeight: _info_pane->layout()->sizeHint: %d x %d", _info_pane->layout()->sizeHint().width(), _info_pane->layout()->sizeHint().height()));
#endif
}

void HeroQuestLevelWindow::exitLevelFinished()
{
    DVX(("Level finished successfully!"));
    _level->obtainLevelReward();
    _level->cleanupGameMaterialOnLevelFinish();
    _app.exit(EXIT_CODE_LEVEL_FINISHED);
}

void HeroQuestLevelWindow::exitLost()
{
    DVX(("Level lost :-("));
    _level->cleanupGameMaterialOnLevelFinish();
    _app.exit(EXIT_CODE_LOST);
}

/*!
 * Throws movement dice, computes the reachable area, and sets the current hero's action states
 * to MOVE_OR_ATTACK. This is executed on startTurn of each hero.
 *
 * Can be called by any thread.
 */
void HeroQuestLevelWindow::initCurrentHeroActionStateToMoveOrAttack()
{
    if (!currentThreadIsGUIThread())
    {
        QMetaObject::invokeMethod(this, "initCurrentHeroActionStateToMoveOrAttack", Qt::BlockingQueuedConnection);
        return;
    }

    _level->initCurrentHeroActionStateToMoveOrAttack();
}

void HeroQuestLevelWindow::searchTrapsButtonClicked()
{
    _level->searchTrapsButtonClicked();
}

void HeroQuestLevelWindow::searchTrapsButtonPressed()
{
    _level->searchTrapsButtonPressed();
}

void HeroQuestLevelWindow::searchTrapsButtonReleased()
{
    _level->searchTrapsButtonReleased();
}

void HeroQuestLevelWindow::searchTreasuresButtonClicked()
{
    _level->searchTreasuresButtonClicked();
}

void HeroQuestLevelWindow::delayTurnButtonClicked()
{
    if (!currentThreadIsGUIThread())
    {
        QMetaObject::invokeMethod(this, "delayTurnButtonClicked", Qt::BlockingQueuedConnection);
        return;
    }

    _level->delayTurnButtonClicked();
}

void HeroQuestLevelWindow::endTurnButtonClicked()
{
    _level->endTurnButtonClicked();
}

/*!
 * Shows the inventory of the currently acting hero.
 * It is then of course possible to use items stored in the inventory.
 */
void HeroQuestLevelWindow::inventoryButtonClicked()
{
    execDialogInventory();
}

/*!
 * Adds a new statistic pane for hero at the end.
 *
 * @param hero
 */
void HeroQuestLevelWindow::addHeroStatisticPane(Hero* hero)
{
    size_t index = _hero_statistic_panes.size();
    insertHeroStatisticPane(hero, index);
}

/*!
 * Inserts a new HeroStatisticPane for hero at logical position index. (There are 4 logical positions in the case of 4 heroes.)
 *
 * @param hero
 * @param index index of _hero_statistic_panes
 * @param extrapolate_info_pane_size if true, the fixed size of the _info_pane will be extrapolated, because the layout's sizeHint is wrong :-(
 */
void HeroQuestLevelWindow::insertHeroStatisticPane(Hero* hero, size_t index, bool extrapolate_info_pane_size)
{
    DVX(("insertHeroStatisticPane for %s at index %zd", qPrintable(hero->getName()), index));

    QSize fixed_size(0, 0);
    if (extrapolate_info_pane_size)
    {
        // bad workaround, needed because layout->sizeHint is wrong after having added sir ragnar in level 2 :-(
        QSize current_size_hint = _info_pane->layout()->sizeHint();
        uint current_num_statistic_panes = uint(_hero_statistic_panes.size());
        fixed_size = QSize(current_size_hint.width(),
                current_size_hint.height() / current_num_statistic_panes * (current_num_statistic_panes + 1));
    }

    // insert into _hero_statistic_panes
    vector<HeroStatisticPane*>::iterator it_hero_statistic_pane = _hero_statistic_panes.begin();
    for (size_t i = 0; i < index; ++i)
    {
        ++it_hero_statistic_pane;
    }
    HeroStatisticPane* hero_statistic_pane = new HeroStatisticPane(hero);
    _hero_statistic_panes.insert(it_hero_statistic_pane, hero_statistic_pane);

    // insert into layout
    QVBoxLayout* info_pane_layout = dynamic_cast<QVBoxLayout*>(_info_pane->layout());
    int layout_index = 2 * (int) index; // [hero0, hero1, ...] => [hero0Pane, space0, hero1Pane, space1, ...]

    // insert hero pane
    info_pane_layout->insertWidget(layout_index, hero_statistic_pane);
    connect(hero_statistic_pane->getInventoryButton(), SIGNAL(clicked()), this, SLOT(inventoryButtonClicked()));

    // insert spacing
    info_pane_layout->insertSpacing(layout_index + 1, 10);

    adjustInfoPaneHeight(fixed_size);

#ifdef DEBUG_PANE_SIZES
    DVX(("========================================================"));
    DVX(("HeroQuestLevelWindow::addHeroStatisticPane (%s)", qPrintable(hero->getName())));
    DVX(("--------------------------------------------------------"));
    DVX(("hero_statistic_pane width x height = %d x %d", hero_statistic_pane->width(), hero_statistic_pane->height()));
    DVX(
            ("_info_pane->layout() sizeHint = %d x %d", _info_pane->layout()->sizeHint().width(), _info_pane->layout()->sizeHint().height()));
#endif
}

bool HeroQuestLevelWindow::isPainting() const
{
    return _is_painting;
}

