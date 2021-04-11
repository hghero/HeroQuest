#include "Level01TheProbation.h"

#include "Playground.h"
#include "NonWalkableDecoration.h"
#include "WalkableDecoration.h"

using namespace std;


Level01TheProbation::Level01TheProbation(uint num_heroes)
:
        Level(num_heroes)
{
	// NIX
}

Level01TheProbation::~Level01TheProbation()
{
	// NIX
}

QString Level01TheProbation::getName() const
{
    return prettyClassName();
}

QString Level01TheProbation::prettyClassName()
{
    return "The Probation";
}

void Level01TheProbation::placeHeroes()
{
	const NodeID hero_start_nodes[4] = {NodeID(1, 14), NodeID(2, 14), NodeID(1, 15), NodeID(2, 15)}; // positions of the stairs

    for (unsigned int i = 0; i < HeroQuestLevelWindow::_hero_quest->getLevel()->getActingHeroes().size(); ++i)
	{
        HeroQuestLevelWindow::_hero_quest->getPlayground()->addCreature(
                HeroQuestLevelWindow::_hero_quest->getLevel()->getActingHeroes()[i], hero_start_nodes[i]);
	}
}

/*!
 * Create monsters, and place them on the board.
 */
void Level01TheProbation::createMonsters()
{
	createMonster<Skeleton>(NodeID(2, 2));
	createMonster<Skeleton>(NodeID(3, 2));
	
	createMonster<Zombie>(NodeID(6, 1));
	// special mummy which attacks with 4 dice instead of 3
	Mummy* guarding_mummy = createMonster<Mummy>(NodeID(6, 2));
	guarding_mummy->setNumDiceAttack(guarding_mummy->getNumDiceAttack() + 1);
	createMonster<Zombie>(NodeID(6, 3));

	createMonster<Skeleton>(NodeID(9, 3));
	createMonster<Mummy>(NodeID(9, 4));
	createMonster<Skeleton>(NodeID(10, 4));

	createMonster<Goblin>(NodeID(3, 6));

	createMonster<Goblin>(NodeID(6, 7));
	createMonster<Goblin>(NodeID(7, 7));

	createMonster<Gargoyle>(NodeID(12, 8));
	createMonster<Fimir>(NodeID(14, 8));
	createMonster<ChaosWarrior>(NodeID(11, 11));
	createMonster<ChaosWarrior>(NodeID(14, 11));

	createMonster<Goblin>(NodeID(0, 13));
	createMonster<Goblin>(NodeID(2, 11));
	createMonster<Orc>(NodeID(2, 12));

	createMonster<Orc>(NodeID(7, 14));
	createMonster<Orc>(NodeID(8, 15));

	createMonster<Goblin>(NodeID(10, 14));
	createMonster<Fimir>(NodeID(10, 16));

	createMonster<Orc>(NodeID(12, 18));

	createMonster<ChaosWarrior>(NodeID(15, 16));
	createMonster<ChaosWarrior>(NodeID(16, 15));
	createMonster<Fimir>(NodeID(17, 14));
}

/*!
 * Create doors and secret doors on the board.
 */
void Level01TheProbation::createDoors()
{
	// normal doors
	vector<pair<NodeID, NodeID> > door_nodes;
	door_nodes.reserve(12);
	door_nodes.push_back(make_pair(NodeID(4, 2), NodeID(5, 2)));
	door_nodes.push_back(make_pair(NodeID(8, 2), NodeID(9, 2)));
	door_nodes.push_back(make_pair(NodeID(3, 3), NodeID(3, 4)));
	door_nodes.push_back(make_pair(NodeID(3, 8), NodeID(3, 9)));
	door_nodes.push_back(make_pair(NodeID(7, 8), NodeID(7, 9)));
	door_nodes.push_back(make_pair(NodeID(15, 9), NodeID(16, 9)));
	door_nodes.push_back(make_pair(NodeID(0, 11), NodeID(1, 11)));
	door_nodes.push_back(make_pair(NodeID(3, 17), NodeID(3, 18)));
	door_nodes.push_back(make_pair(NodeID(7, 17), NodeID(7, 18)));
	door_nodes.push_back(make_pair(NodeID(8, 15), NodeID(9, 15)));
	door_nodes.push_back(make_pair(NodeID(10, 12), NodeID(10, 13)));
	door_nodes.push_back(make_pair(NodeID(13, 15), NodeID(14, 15)));

	for (unsigned int i = 0; i < door_nodes.size(); ++i)
	{
		Door* door = new Door(door_nodes[i].first, door_nodes[i].second);
		_doors.push_back(door);
	}

#if 0 // no secret doors in this level
	// secret doors
	vector<pair<pair<NodeID, NodeID>, SecretDoor::DrawPos> > secret_door_nodes;
	secret_door_nodes.reserve(20);
	secret_door_nodes.push_back(make_pair(make_pair(NodeID(3, 0), NodeID(3, 1)), SecretDoor::DRAW_POS_F1));

	for (unsigned int i = 0; i < secret_door_nodes.size(); ++i)
	{
		SecretDoor* secret_door = new SecretDoor(secret_door_nodes[i].first.first, secret_door_nodes[i].first.second, secret_door_nodes[i].second);
		_doors.push_back(secret_door);
	}
#endif
}

/*!
 * Create traps on the board.
 */
void Level01TheProbation::createTraps()
{
    // (no traps in this level)

	// pit traps
    //_traps.push_back(new PitTrap(NodeID(2, 17)));

	// spear traps
    //_traps.push_back(new SpearTrap(NodeID(0, 1)));
    //_traps.push_back(new SpearTrap(NodeID(0, 7)));
    //_traps.push_back(new SpearTrap(NodeID(2, 16)));
}

/*!
 * Create walkable and non-walkable decoration on the board.
 */
void Level01TheProbation::createDecoration()
{
	_decoration.push_back(new Chest(NodeID(1, 1), Decoration::TURN_0));
	_decoration.push_back(new Chest(NodeID(10, 5), Decoration::TURN_0));
	_decoration.push_back(new Chest(NodeID(9, 17), Decoration::TURN_0));
	_decoration.push_back(new AlchemistTable(NodeID(5, 15), Decoration::TURN_90));
	_decoration.push_back(new Armory(NodeID(11, 15), Decoration::TURN_90));
	_decoration.push_back(new Cupboard(NodeID(5, 13), Decoration::TURN_0));
	_decoration.push_back(new Cupboard(NodeID(15, 13), Decoration::TURN_0));
	_decoration.push_back(new Cupboard(NodeID(15, 17), Decoration::TURN_0));
	_decoration.push_back(new Fireplace(NodeID(12, 7), Decoration::TURN_180));
	_decoration.push_back(new Sarcophagus(NodeID(10, 1), Decoration::TURN_0));
	_decoration.push_back(new Table(NodeID(11, 9), Decoration::TURN_0));
	_decoration.push_back(new Table(NodeID(6, 5), Decoration::TURN_0));
	_decoration.push_back(new Throne(NodeID(10, 8), Decoration::TURN_180));
	_decoration.push_back(new TortureRack(NodeID(3, 10), Decoration::TURN_180));
	_decoration.push_back(new WarlockTable(NodeID(1, 5), Decoration::TURN_270));
	_decoration.push_back(new Debris(NodeID(0, 1), Decoration::TURN_0));
	_decoration.push_back(new Debris(NodeID(12, 5), Decoration::TURN_0));
	_decoration.push_back(new Debris(NodeID(13, 5), Decoration::TURN_0));
	_decoration.push_back(new Debris(NodeID(17, 9), Decoration::TURN_0));
	_decoration.push_back(new Debris(NodeID(14, 18), Decoration::TURN_0));
	_decoration.push_back(new Stairs(NodeID(1, 14), Decoration::TURN_0));
}

void Level01TheProbation::createChestTreasureDescriptions()
{
    // chest A
    TreasureDescription chest_a;
    chest_a.setID("chest_A");
    chest_a.setAmount(100);
    chest_a.setText("In the chest you find 100 gold coins!");
    // no actions
    _chest_pos_to_treasure_description[NodeID(1, 1)] = chest_a;

    // chest C
    TreasureDescription chest_c;
    chest_c.setID("chest_C");
    chest_c.setAmount(150);
    chest_c.setText("In the chest you find 150 gold coins!");
    // no actions
    _chest_pos_to_treasure_description[NodeID(10, 5)] = chest_c;

    // chest D
    TreasureDescription chest_d;
    chest_d.setID("chest_D");
    chest_d.setAmount(0);
    chest_d.setText("This chest is empty.");
    // no actions
    _chest_pos_to_treasure_description[NodeID(9, 17)] = chest_d;
}

QString Level01TheProbation::getLevelBriefing() const
{
    return "<title>The Probation</title>"
            "<description>You have learned well, my friends. Now, time has come to face your first challenge. You shall "
            "infiltrade the catacombs of Verag, an insidious Gargoyle. In these catacombs, the deadly remains of "
            "Fellmarg have been entombed. Your task is not simple, and some of you might not return from it. "
            "The survivors will continue their training afterwards. Consider is as a first step towards being a hero. "
            "Be careful, friends.</description>";
}

/*!
 * After removing the monster, checks if Verag has just been killed. If yes,
 * displays an informational dialog.
 *
 * @param monster
 * @return true iff monster was successfully removed
 */
bool Level01TheProbation::removeMonster(Monster* monster)
{
    bool verag_killed = monster->getName(monster).compare("Gargoyle") == 0;
    bool result = Level::removeMonster(monster);
    if (verag_killed)
    {
        HeroQuestLevelWindow::_hero_quest->execDialogOk("Task Successful",
                "Verag has been killed! You can now exit the level by going back to the stairway.");

        _level_state._heroes_may_exit_level_via_stairway = true;
    }
    return result;
}

/*!
 * \return A reference to a dummy instance of the roaming monster in this level.
 */
const Monster* Level01TheProbation::getRoamingMonsterType() const
{
	// There is no roaming monster defined for this level in the original rule book. => Simple pick the Orc.
	return _roaming_monster_templates.find("Orc")->second;
}
