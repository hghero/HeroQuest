#include "Level02SavingSirRagnar.h"

#include "Playground.h"
#include "NonWalkableDecoration.h"
#include "WalkableDecoration.h"
#include "BoardGraph.h"
#include "QuestBoard.h"
#include "Debug.h"
#include "StreamUtils.h"

using namespace std;

// TODO: The HeroStatisticPanes are too narrow - the overall container size must somehow be enlarged

Level02SavingSirRagnar::Level02SavingSirRagnar(uint num_heroes)
:
        Level(num_heroes), _level_completion_state(LEVEL_COMPLETION_STATE_SEARCHING_SIR_RAGNAR)
{
	// NIX
}

Level02SavingSirRagnar::~Level02SavingSirRagnar()
{
	// NIX
}

QString Level02SavingSirRagnar::getName() const
{
    return prettyClassName();
}

QString Level02SavingSirRagnar::prettyClassName()
{
    return "Saving Sir Ragnar";
}

void Level02SavingSirRagnar::placeHeroes()
{
    const NodeID hero_start_nodes[4] = {NodeID(10, 7), NodeID(11, 7), NodeID(10, 8), NodeID(11, 8)}; // positions of the stairs

    for (unsigned int i = 0; i < HeroQuestLevelWindow::_hero_quest->getLevel()->getActingHeroes().size(); ++i)
	{
        HeroQuestLevelWindow::_hero_quest->getPlayground()->addCreature(
                HeroQuestLevelWindow::_hero_quest->getLevel()->getActingHeroes()[i], hero_start_nodes[i]);
	}
}

/*!
 * Create monsters, and place them on the board.
 */
void Level02SavingSirRagnar::createMonsters()
{
    createMonster<Goblin>(NodeID(3, 9));

    createMonster<Orc>(NodeID(6, 14));
    createMonster<Orc>(NodeID(7, 15));

    createMonster<Fimir>(NodeID(8, 11));

    createMonster<Goblin>(NodeID(16, 12));
    createMonster<Goblin>(NodeID(16, 6));
    createMonster<Goblin>(NodeID(14, 6));

    createMonster<Orc>(NodeID(14, 4));
    createMonster<Orc>(NodeID(15, 3));

    createMonster<Orc>(NodeID(19, 3));

    createMonster<Orc>(NodeID(23, 6));
    createMonster<Orc>(NodeID(24, 7));

    createMonster<Goblin>(NodeID(23, 9));
}

/*!
 * Create doors and secret doors on the board.
 */
void Level02SavingSirRagnar::createDoors()
{
	// normal doors
	vector<pair<NodeID, NodeID> > door_nodes;
    door_nodes.reserve(10);
    door_nodes.push_back(make_pair(NodeID(2, 9), NodeID(2, 10)));
    door_nodes.push_back(make_pair(NodeID(6, 11), NodeID(7, 11)));
    door_nodes.push_back(make_pair(NodeID(8, 15), NodeID(9, 15)));
    door_nodes.push_back(make_pair(NodeID(11, 17), NodeID(11, 18)));
    door_nodes.push_back(make_pair(NodeID(12, 11), NodeID(12, 12)));
    door_nodes.push_back(make_pair(NodeID(15, 6), NodeID(15, 5)));
    door_nodes.push_back(make_pair(NodeID(16, 3), NodeID(17, 3)));
    door_nodes.push_back(make_pair(NodeID(19, 4), NodeID(19, 5)));
    door_nodes.push_back(make_pair(NodeID(20, 6), NodeID(21, 6)));
    door_nodes.push_back(make_pair(NodeID(23, 8), NodeID(23, 9)));

	for (unsigned int i = 0; i < door_nodes.size(); ++i)
	{
		Door* door = new Door(door_nodes[i].first, door_nodes[i].second);
		_doors.push_back(door);
	}

	// secret doors
	vector<pair<pair<NodeID, NodeID>, SecretDoor::DrawPos> > secret_door_nodes;
    secret_door_nodes.reserve(1);
    secret_door_nodes.push_back(make_pair(make_pair(NodeID(8, 12), NodeID(8, 13)), SecretDoor::DRAW_POS_F2));

	for (unsigned int i = 0; i < secret_door_nodes.size(); ++i)
	{
		SecretDoor* secret_door = new SecretDoor(secret_door_nodes[i].first.first, secret_door_nodes[i].first.second, secret_door_nodes[i].second);
		_doors.push_back(secret_door);
	}
}

/*!
 * Create traps on the board.
 */
void Level02SavingSirRagnar::createTraps()
{
    // (no traps in this level)

	// pit traps
    //_traps.push_back(new PitTrap(NodeID(2, 17)));

	// spear traps
    //_traps.push_back(new SpearTrap(NodeID(0, 1)));
}

/*!
 * Create walkable and non-walkable decoration on the board.
 */
void Level02SavingSirRagnar::createDecoration()
{
    _decoration.push_back(new Chest(NodeID(2, 13), Decoration::TURN_0)); // chest A
    _decoration.push_back(new Chest(NodeID(19, 8), Decoration::TURN_0)); // chest B
    _decoration.push_back(new Cupboard(NodeID(17, 6), Decoration::TURN_90));
    _decoration.push_back(new Table(NodeID(5, 15), Decoration::TURN_90));
    _decoration.push_back(new Table(NodeID(18, 1), Decoration::TURN_0));
    _decoration.push_back(new Debris(NodeID(1, 9), Decoration::TURN_0));
    _decoration.push_back(new Debris(NodeID(9, 18), Decoration::TURN_0));
    _decoration.push_back(new Debris(NodeID(14, 18), Decoration::TURN_0));
	_decoration.push_back(new Debris(NodeID(12, 5), Decoration::TURN_0));
	_decoration.push_back(new Debris(NodeID(13, 5), Decoration::TURN_0));
    _decoration.push_back(new Debris(NodeID(24, 9), Decoration::TURN_0));
    _decoration.push_back(new Stairs(NodeID(10, 7), Decoration::TURN_0));
}

void Level02SavingSirRagnar::createChestTreasureDescriptions()
{
    // chest A
    TreasureDescription chest_a;
    chest_a.setID("chest_A");
    chest_a.setAmount(100);
    chest_a.setText("This chest contains a trap only.");
    chest_a.addActionString("LIFE_POINTS <-1>");
    _chest_pos_to_treasure_description[NodeID(2, 13)] = chest_a;

    // chest B
    TreasureDescription chest_b;
    chest_b.setID("chest_B");
    chest_b.setAmount(50);
    chest_b.setText(
            "This chest contains 50 gold pieces and a healing potion which instantly recovers up to 4 life points.");
    chest_b.addActionString("GOLD <50>");
    chest_b.addActionString("LIFE_POINTS <4>");
    _chest_pos_to_treasure_description[NodeID(19, 8)] = chest_b;
}

QString Level02SavingSirRagnar::getLevelBriefing() const
{
    return "<title>Saving Sir Ragnar</title>"
            "<description>Sir Ragnar, a mighty baron, has been kidnapped. He is being detained by Ulag, the "
            "general of the Orcs. You shall find Sir Ragnar and save him. Those who succeed will receive a total reward "
            "of 100 gold pieces, promised by Prince Magnus. The reward won't be paid if Sir Ragnar dies during "
            "the escape.</description>";
}

bool Level02SavingSirRagnar::save(ostream& stream) const
{
    Level::save(stream);

    StreamUtils::writeUInt(stream, (uint) _level_completion_state);

    return !stream.fail();
}

bool Level02SavingSirRagnar::load(istream& stream)
{
    Level::load(stream);

    // _level_completion_state
    uint level_completion_state_uint;
    StreamUtils::readUInt(stream, &level_completion_state_uint);
    _level_completion_state = (LevelCompletionState) level_completion_state_uint;

    return !stream.fail();
}


/*!
 * \return A reference to a dummy instance of the roaming monster in this level.
 */
const Monster* Level02SavingSirRagnar::getRoamingMonsterType() const
{
	return _roaming_monster_templates.find("Orc")->second;
}

/*!
 * Is called when creature has been moved onto node.
 * Checks if Sir Ragnar has been found and handles stuff accordingly.
 *
 * @param creature
 * @param node
 */
void Level02SavingSirRagnar::fieldHasBeenReachedBy(const Creature* creature, const NodeID& node)
{
    if (_level_completion_state != LEVEL_COMPLETION_STATE_SEARCHING_SIR_RAGNAR)
        return;

    // check if node is inside Sir Ragnar's prison room
    if (HeroQuestLevelWindow::_hero_quest->getPlayground()->getQuestBoard()->getRoomID(node)
            != HeroQuestLevelWindow::_hero_quest->getPlayground()->getQuestBoard()->getRoomID(NodeID(5, 10)))
        return;

    handleSirRagnarFound();
}

/*!
 * Is called when creature has opened door.
 * Checks if Sir Ragnar has been found and handles stuff accordingly.
 *
 * @param creature
 * @param door
 */
void Level02SavingSirRagnar::doorHasBeenOpenedBy(const Creature* creature, const Door* door)
{
    if (_level_completion_state != LEVEL_COMPLETION_STATE_SEARCHING_SIR_RAGNAR)
        return;

    // check if door leads to Sir Ragnar's prison room
    if (door->getF1() != NodeID(6, 11) && door->getF2() != NodeID(6, 11))
        return;

    handleSirRagnarFound();
}

/*!
 * Shows all secret doors and pit traps which are in line of sight from the current hero's position.
 * Disarms traps in chests if possible.
 */
void Level02SavingSirRagnar::searchTrapsButtonClicked()
{
    // --- show all secret doors and pit traps which are in line of sight from the current hero's position ---
    Level::searchTrapsButtonClicked();

    // --- disarm traps in all chests in the current room ---
    // 1. get room id of currently acting creature
    Creature* current_creature = getCurrentlyActingCreature();
    if (!current_creature->isHero())
        return;
    Hero* current_hero = dynamic_cast<Hero*>(current_creature);
    const NodeID* pos_current_hero = HeroQuestLevelWindow::_hero_quest->getPlayground()->getCreaturePos(current_hero);
    if (pos_current_hero == 0)
        DVX(("Internal program error: Level02SavingSirRagnar::searchTrapsButtonClicked()"));
    uint hero_room_id = HeroQuestLevelWindow::_hero_quest->getPlayground()->getQuestBoard()->getRoomID(
            *pos_current_hero);

    // 2. disarm all traps inside all chests in the room with id room_id
    for (map<NodeID, TreasureDescription>::iterator it_chests = _chest_pos_to_treasure_description.begin();
            it_chests != _chest_pos_to_treasure_description.end(); ++it_chests)
    {
        const NodeID& chest_node_id = it_chests->first;
        uint chest_room_id = HeroQuestLevelWindow::_hero_quest->getPlayground()->getQuestBoard()->getRoomID(
                chest_node_id);
        if (chest_room_id != hero_room_id)
            continue;

        TreasureDescription& treasure_description = it_chests->second;
        treasure_description.disarmTraps();
    }
}

/*!
 * Called when the level was finished successfully. Here, Heroes will obtain their level reward.
 * In this level, the reward is 100 gold pieces, devided on all heroes who are still alive.
 */
void Level02SavingSirRagnar::obtainLevelReward()
{
    vector<Hero*>& heroes = HeroQuestLevelWindow::_hero_quest->getHeroes();
    uint num_heroes_survived = 0;
    for (size_t i = 0; i < heroes.size(); ++i)
    {
        if (heroes[i]->getLifePoints() > 0)
            ++num_heroes_survived;
    }

    uint total_reward = 100;
    vector<uint> gold_per_hero(num_heroes_survived, total_reward / num_heroes_survived);

    // distribute the remainder
    uint remainder = total_reward % num_heroes_survived;
    size_t i_gold = 0;
    while (remainder > 0)
    {
        ++(gold_per_hero[i_gold]);
        --remainder;

        ++i_gold;
    }

    // actually assign the gold pieces
    i_gold = 0;
    for (size_t i_hero = 0; i_hero < heroes.size(); ++i_hero)
    {
        if (heroes[i_hero]->getLifePoints() > 0)
        {
            heroes[i_hero]->getInventory().addGold(gold_per_hero[i_gold]);
            ++i_gold;
        }
    }
}

/*!
 * - play a siren sound
 * - display an informative dialog; after click ok:
 * - add SirRagnar to the active heroes
 * - open all doors on the board
 * - make everything (which is reachable) visible on the board
 */
void Level02SavingSirRagnar::handleSirRagnarFound()
{
    _level_completion_state = LEVEL_COMPLETION_STATE_FOUND_SIR_RAGNAR;

    _level_state._heroes_may_exit_level_via_stairway = true;

    HeroQuestLevelWindow::_hero_quest->playSoundOnce(SoundManager::SOUND_SIREN);

    HeroQuestLevelWindow::_hero_quest->execDialogOk("Found Sir Ragnar",
            "You found Sir Ragnar! Now lead him back to the stairs in order to fulfill your mission!");

    Hero* sir_ragnar = new SirRagnar();
    sir_ragnar->create();
    size_t hero_index = _acting_heroes.size();
    _acting_heroes.push_back(sir_ragnar);
    _level_state._hero_action_states.push_back(Level::HeroActionStates());
    _level_state._num_turns_left.push_back(1);
    HeroQuestLevelWindow::_hero_quest->insertHeroStatisticPane(sir_ragnar, hero_index, true);

    // place Sir Ragnar on the board at (5, 11)
    HeroQuestLevelWindow::_hero_quest->getPlayground()->addCreature(sir_ragnar, NodeID(5, 11));

    // open all doors on the board
    BoardGraph* board_graph = HeroQuestLevelWindow::_hero_quest->getPlayground()->getQuestBoard()->getBoardGraph();
    for (vector<Door*>::iterator it = _doors.begin(); it != _doors.end(); ++it)
    {
        Door* door = *it;
        if (door->getOpen())
            continue;

        board_graph->getNode(door->getF1()).addNeighborAbsolute(door->getF2());
        board_graph->getNode(door->getF2()).addNeighborAbsolute(door->getF1());

        door->setOpen(true);
    }

    // make everything (which is reachable) visible on the board
    HeroQuestLevelWindow::_hero_quest->getPlayground()->makeAllRelevantNodesVisible();

    // update the reachable area
    HeroQuestLevelWindow::_hero_quest->getPlayground()->getQuestBoard()->computeReachableArea();

    // redraw
    HeroQuestLevelWindow::_hero_quest->getPlayground()->update();

    // update button visibility
    HeroQuestLevelWindow::_hero_quest->updateButtons();
}
