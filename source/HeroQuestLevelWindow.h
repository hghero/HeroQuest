#ifndef HERO_QUEST_LEVEL_WINDOW_H
#define HERO_QUEST_LEVEL_WINDOW_H

//#define HG_DEBUG

#include <vector>

#include <QtCore/QString>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtGui/QBackingStore>
#include <QtCore/QString>
#include <QtCore/QMutex>

#include "Hero.h"
#include "SoundManager.h"
#include "DiceRollPane.h"
#include "EventBlockingContext.h"
#include "GameState.h"
#include "Door.h"

class QMenuBar;
class QLabel;
class Level;
class HeroStatisticPane;
class ButtonPane;
class HintPane;
class Playground;
class Monster;
class NodeID;
class InventoryItem;
class SpellCardStorage;
class Command;

class HeroQuestLevelWindow : public QMainWindow
{
    friend class Hero;
    friend class PaintAutoMutex;

  Q_OBJECT

  public:
    static HeroQuestLevelWindow* _hero_quest;

    static int FIELD_SIZE;

    static const QString BUTTON_STYLE_ACTIVE;
    static const QString BUTTON_STYLE_INACTIVE;
    static const QString INVENTORY_BUTTON_STYLE_ACTIVE;
    static const QString INVENTORY_BUTTON_STYLE_INACTIVE;
    static const QString DIALOG_STYLE;
    static const QString DIALOG_HEADER_STYLE;
    static const QString DIALOG_HEADER_BIG_STYLE;
    static const QString DIALOG_TEXT_STYLE;
    static const QString DIALOG_TEXT_BIG_STYLE;
    static const QString DIALOG_COMBOBOX_STYLE;
    static const QString STATISTIC_HEADER_STYLE_ACTIVE;
    static const QString STATISTIC_HEADER_STYLE_INACTIVE;
    static const QString STATISTIC_BODY_STYLE_ACTIVE;
    static const QString STATISTIC_BODY_STYLE_INACTIVE;
    static const QString STATISTIC_BODY_STYLE_WARNING;
    static const QString DICE_ROLL_PANE_STYLE;

    static bool _debug_create_attacks_creature_command_running;

    enum ExitCode
    {
        EXIT_CODE_LEVEL_FINISHED = 0, EXIT_CODE_LOST,
    } ExitCodeEnum;

    enum UserInteractionMode
    {
        USER_INTERACTION_NORMAL = 0, //
        USER_INTERACTION_SELECT_HERO, //
        USER_INTERACTION_SELECT_CREATURE, //
        USER_INTERACTION_SELECT_CREATURE_OR_DOOR
    } UserInteractionModeEnum;

    HeroQuestLevelWindow(
            const QString& version,
            QApplication& app,
            const QString& filename,
            GameState* game_state,
            SpellCardStorage* spell_card_storage);
    ~HeroQuestLevelWindow();

    std::vector<Hero*>& getHeroes();
    SpellCardStorage* getSpellCardStorage();
    Playground* getPlayground();
    Level* getLevel();
    DiceRollPane* getDiceRollPane();
    ButtonPane* getButtonPane();
    std::vector<HeroStatisticPane*>& getHeroStatisticPanes();

    Creature* getCreature(uint referencing_id);

    void resetActionPane();

    void throwAttackDice(Creature& attacker, std::vector<DiceRollPane::AttackDiceResult>* dice_results);
    void throwAttackDice(Creature& attacker, uint num_dice, std::vector<DiceRollPane::AttackDiceResult>* dice_results);
    void throwDefendDice(Creature& defender, std::vector<DiceRollPane::AttackDiceResult>* dice_results);
    void throwDefendDice(Creature& defender, uint num_dice, std::vector<DiceRollPane::AttackDiceResult>* dice_results);

    bool useInventoryItem(const InventoryItem& inventory_item);
    bool useTreasureCard(const TreasureCard& treasure_card);
    bool useSpellCard(const SpellCard& spell_card);

    bool creatureTriesToWakeUp(Creature& creature);

    UserInteractionMode getUserInteractionMode() const;
    void setUserInteractionMode(UserInteractionMode user_interaction_mode);
    void setUserSelectedNodeID(const NodeID& node_id);
    void setUserSelectedTwoNodeIDs(const std::pair<NodeID, NodeID>& node_ids);
    Hero* userSelectsHero();
    Creature* userSelectsCreature();
    void userSelectsCreatureOrDoor(Creature** creature, Door** door);

    bool checkApplySpell(Hero* hero);
    bool checkDefend(Hero* hero);
    bool checkDeath(Hero* hero);

    virtual bool save(std::ostream& stream) const;
    virtual bool load(std::istream& stream);

    bool loadImagesAndAdjustPointers();

    bool isPainting() const;

    public slots:
    void initCurrentHeroActionStateToMoveOrAttack();
    void initActionPane();

    void updateButtons();
    void updateHeroStatisticPanes();

    void playSoundOnce(QVariant sound_id);
    void removeHeroStatisticPane(Hero* hero);
    void startHeroTurn();

    int execDialogOkCancel(QVariant title, QVariant text);
    void execDialogOpenChest(QVariant treasure_description);
    void execDialogTreasureCard(QVariant treasure_card);
    void execDialogInventory();

    static bool chooseLoadGameFilename(QWidget* parent, QString* filename);
    static bool chooseSaveGameFilename(QWidget* parent, QString* filename);

    bool execute(Command& cmd);
    bool executeBackground(Command& cmd);

    EventBlockingContext::EventBlockingBitmask getEventBlockingBitmask() const;
    void setEventBlockingBitmask(EventBlockingContext::EventBlockingBitmask);

    private slots:
    void throwAttackDice(QVariant attacker, QVariant dice_results);
    void throwAttackDice(QVariant attacker, QVariant num_dice, QVariant dice_results);
    void throwDefendDice(QVariant defender, QVariant dice_results);
    void throwDefendDice(QVariant defender, QVariant num_dice, QVariant dice_results);
    bool checkApplySpell(QVariant hero);
    bool checkDefend(QVariant hero);
    bool checkDeath(QVariant hero);
    bool loadGame();
    bool loadGame(const QString& filename);
    bool saveGameChooseFilename();

    public:
    static Level* createLevelFromLevelID(GameState::LevelID level_id, uint num_heroes);

    void updateHeroStatisticPaneOfCurrentHero();
    void updateHeroStatisticPane(const Hero* hero);

    void delay(uint ms);

    static bool currentThreadIsGUIThread();

    void addMonsterToLevelAndPlayground(Monster* monster, const NodeID& node_id);

    QSize getInfoPaneSize() const;

    void exitLost();

    protected:
    virtual void paintEvent(QPaintEvent* event);

    private:
    virtual bool eventFilter(QObject* object, QEvent* event);

    bool createLevel();

    void createGUIElements();

    void startGame(const QString& filename);

    void addHeroesAccordingToGameState();
    template <class HeroType>
    Hero* addHero();

    void addHeroStatisticPane(Hero* hero);

    void connectActionButtons();

  private slots:
    void searchTrapsButtonClicked();
    void searchTreasuresButtonClicked();
    void delayTurnButtonClicked();
    void endTurnButtonClicked();

    void inventoryButtonClicked();

  private:
    QString _version;
    QApplication& _app;
    QAction* _save_action;
    GameState* _game_state;
    SpellCardStorage* _spell_card_storage;
    Level* _level;
    std::vector<Hero*> _heroes;

    /*
     * layout:
     *
     *  -------------------------------------------------
     * |           |                     |               |
     * |           |                     |               |
     * |           |                     |               |
     * | action    |     playground      |     info      |
     * |  pane     |       pane          |     pane      |
     * |           |                     |               |
     * |           |                     |               |
     *  -------------------------------------------------
     */
    QLabel* _action_pane;
    DiceRollPane* _dice_roll_pane;
    ButtonPane* _button_pane;
    HintPane* _hint_pane;
    Playground* _playground;
    QLabel* _info_pane;

    std::vector<HeroStatisticPane*> _hero_statistic_panes; // shown in the _info_pane

    SoundManager* _sound_manager;

    EventBlockingContext::EventBlockingBitmask _event_blocking_bitmask;
    bool _is_painting;

    UserInteractionMode _user_interaction_mode;
    NodeID _user_selected_node_id;
    std::pair<NodeID, NodeID> _user_selected_two_node_ids;
    bool _user_abort;
};
Q_DECLARE_METATYPE(Creature*);
Q_DECLARE_METATYPE(Hero*);
Q_DECLARE_METATYPE(std::vector<DiceRollPane::AttackDiceResult>*);
Q_DECLARE_METATYPE(TreasureCard*);
Q_DECLARE_METATYPE(TreasureDescription*);

/*!
 * Adds a hero of type HeroType, including statistic panel, to the game, and returns the added hero.
 */
template <class HeroType>
Hero* HeroQuestLevelWindow::addHero()
{
    Hero* hero = new HeroType();
    hero->create();
    _heroes.push_back(hero);
    return _heroes.back();
}

#endif