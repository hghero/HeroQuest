//#define HG_DEBUG

#include "Playground.h"

#include <iostream>

#include <QtGui/QPainter>
#include <QtGui/QMouseEvent>

#include "QuestBoard.h"
#include "Monster.h"
#include "Hero.h"
#include "Math.h"
#include "HeroQuestLevelWindow.h"
#include "Trap.h"
#include "SecretDoor.h"
#include "WalkableDecoration.h"
#include "NonWalkableDecoration.h"
#include "ThreadUtils.h"
#include "StreamUtils.h"
#include "Debug.h"
#include "Level.h"
#include "PainterContext.h"
#include "ParameterStorage.h"
#include "GraphBase.h"


using namespace std;


//static NodeID _check_field(0, 0);

// decroation image filenames
const QString Playground::SECRET_DOOR_FILENAME = ":/graphics/secret_door_top.jpg";
const QString Playground::PIT_TRAP_FILENAME = ":/graphics/pit_trap.jpg";
const QString Playground::SPEAR_TRAP_FILENAME = ":/graphics/spear_trap.tga";
const QString Playground::DEBRIS_FILENAME = ":/graphics/debris.jpg";

// furniture image filenames
const QString Playground::ALCHEMIST_TABLE_FILENAME = ":/graphics/alchemist_table.jpg";
const QString Playground::ARMORY_FILENAME = ":/graphics/armory.tga";
const QString Playground::CHEST_FILENAME = ":/graphics/chest.tga";
const QString Playground::CUPBOARD_FILENAME = ":/graphics/cupboard.jpg";
const QString Playground::FIREPLACE_FILENAME = ":/graphics/fireplace.tga";
const QString Playground::SARCOPHAGUS_FILENAME = ":/graphics/sarcophagus.jpg";
const QString Playground::STAIRS_FILENAME = ":/graphics/stairs.jpg";
const QString Playground::TABLE_FILENAME = ":/graphics/table.tga";
const QString Playground::THRONE_FILENAME = ":/graphics/throne.tga";
const QString Playground::TORTURE_RACK_FILENAME = ":/graphics/torture_rack.tga";
const QString Playground::WARLOCK_TABLE_FILENAME = ":/graphics/warlock_table.jpg";

// treasure image filenames

Playground::Playground(QWidget* parent)
: QLabel(parent),
  _quest_board(0),
  _creatures(),
  _nodes(),
  _nodes_to_decoration(),
  _node_visibility(QuestBoard::FIELDS_X, QuestBoard::FIELDS_Y, false),
  _action_mode(ACTION_MODE_NONE),
  _action_mode_related_hero(
                0),
  _secret_door_image(0), // set in ::create
  _pit_trap_image(0),
  _spear_trap_image(0),
  _debris_image(0),
  _alchemist_table_image(0),
  _armory_image(0),
  _chest_image(0),
  _cupboard_image(0),
  _fireplace_image(0),
  _sarcophagus_image(0),
  _stairs_image(0),
  _table_image(0),
  _throne_image(0),
  _torture_rack_image(0),
  _warlock_table_image(0)
{
    DV(("Created new Playground 0x%x", (unsigned int)this));
}

Playground::~Playground()
{
    delete _quest_board;

	delete(_secret_door_image);
	delete(_pit_trap_image);
	delete(_spear_trap_image);
	delete(_debris_image);
	delete(_alchemist_table_image);
	delete(_armory_image);
	delete(_chest_image);
	delete(_cupboard_image);
	delete(_fireplace_image);
	delete(_sarcophagus_image);
	delete(_stairs_image);
	delete(_table_image);
	delete(_throne_image);
	delete(_torture_rack_image);
	delete(_warlock_table_image);

    // heroes are deleted by HeroQuest;
	// monsters are deleted by Level

    DV(("Deleted Playground 0x%x", (unsigned int)this));
}

bool Playground::create(bool only_load_images)
{
    if (!only_load_images)
    {
        // quest board
        _quest_board = new QuestBoard();
        if (!_quest_board->create())
            return false;
    }

	// === load further images ===

	// secret_door
    if (_secret_door_image == 0)
    {
        _secret_door_image = new QPixmap(SECRET_DOOR_FILENAME);
        if (_secret_door_image == 0)
        {
            cout << qPrintable(SECRET_DOOR_FILENAME)
                    << " not found or failed to load" << endl;
            return false;
        }
    }

    // pit_trap
    if (_pit_trap_image == 0) {
        _pit_trap_image = new QPixmap(PIT_TRAP_FILENAME);
        if (_pit_trap_image == 0)
        {
            cout << qPrintable(PIT_TRAP_FILENAME)
                    << " not found or failed to load" << endl;
            return false;
        }
    }

	// spear trap
    if (_spear_trap_image == 0)
    {
        _spear_trap_image = new QPixmap(SPEAR_TRAP_FILENAME);
        if (_spear_trap_image == 0)
        {
            cout << qPrintable(SPEAR_TRAP_FILENAME)
                    << " not found or failed to load" << endl;
            return false;
        }
    }

	// debris
    if (_debris_image == 0)
    {
        _debris_image = new QPixmap(DEBRIS_FILENAME);
        if (_debris_image == 0)
        {
            cout << qPrintable(DEBRIS_FILENAME)
                    << " not found or failed to load" << endl;
            return false;
        }
    }

	// alchemist table
    if (_alchemist_table_image == 0)
    {
        _alchemist_table_image = new QPixmap(ALCHEMIST_TABLE_FILENAME);
        if (_alchemist_table_image == 0)
        {
            cout << qPrintable(ALCHEMIST_TABLE_FILENAME)
                    << " not found or failed to load" << endl;
            return false;
        }
    }

	// armory
    if (_armory_image == 0)
    {
        _armory_image = new QPixmap(ARMORY_FILENAME);
        if (_armory_image == 0)
        {
            cout << qPrintable(ARMORY_FILENAME)
                    << " not found or failed to load" << endl;
            return false;
        }
    }

	// chest
    if (_chest_image == 0) {
        _chest_image = new QPixmap(CHEST_FILENAME);
        if (_chest_image == 0)
        {
            cout << qPrintable(CHEST_FILENAME) << " not found or failed to load"
                    << endl;
            return false;
        }
    }

	// cupboard
    if (_cupboard_image == 0) {
        _cupboard_image = new QPixmap(CUPBOARD_FILENAME);
        if (_cupboard_image == 0)
        {
            cout << qPrintable(CUPBOARD_FILENAME)
                    << " not found or failed to load" << endl;
            return false;
        }
    }

	// fireplace
    if (_fireplace_image == 0)
    {
        _fireplace_image = new QPixmap(FIREPLACE_FILENAME);
        if (_fireplace_image == 0)
        {
            cout << qPrintable(FIREPLACE_FILENAME)
                    << " not found or failed to load" << endl;
            return false;
        }
    }

	// sarcophagus
    if (_sarcophagus_image == 0)
    {
        _sarcophagus_image = new QPixmap(SARCOPHAGUS_FILENAME);
        if (_sarcophagus_image == 0)
        {
            cout << qPrintable(SARCOPHAGUS_FILENAME)
                    << " not found or failed to load" << endl;
            return false;
        }
    }

	// stairs
    if (_stairs_image == 0)
    {
        _stairs_image = new QPixmap(STAIRS_FILENAME);
        if (_stairs_image == 0)
        {
            cout << qPrintable(STAIRS_FILENAME)
                    << " not found or failed to load" << endl;
            return false;
        }
    }

	// table
    if (_table_image == 0) {
        _table_image = new QPixmap(TABLE_FILENAME);
        if (_table_image == 0)
        {
            cout << qPrintable(TABLE_FILENAME) << " not found or failed to load"
                    << endl;
            return false;
        }
    }

	// throne
    if (_throne_image == 0)
    {
        _throne_image = new QPixmap(THRONE_FILENAME);
        if (_throne_image == 0)
        {
            cout << qPrintable(THRONE_FILENAME)
                    << " not found or failed to load" << endl;
            return false;
        }
    }

	// torture rack
    if (_torture_rack_image == 0)
    {
        _torture_rack_image = new QPixmap(TORTURE_RACK_FILENAME);
        if (_torture_rack_image == 0)
        {
            cout << qPrintable(TORTURE_RACK_FILENAME)
                    << " not found or failed to load" << endl;
            return false;
        }
    }

	// warlock table
    if (_warlock_table_image == 0)
    {
        _warlock_table_image = new QPixmap(WARLOCK_TABLE_FILENAME);
        if (_warlock_table_image == 0)
        {
            cout << qPrintable(WARLOCK_TABLE_FILENAME)
                    << " not found or failed to load" << endl;
            return false;
        }
    }

	return true;
}

QuestBoard* Playground::getQuestBoard()
{
	return _quest_board;
};

bool Playground::addCreature(Creature* creature, const NodeID& node_id)
{
	_creatures[creature] = node_id;
	_nodes[node_id] = creature;

	return true;
}

/*!
 * Updates the _nodes_to_decoration map and the neighborhood info.
 */
void Playground::addDecoration(Decoration* decoration)
{
	// _nodes_to_decoration
	vector<NodeID> decoration_nodes;
	decoration->getNodes(&decoration_nodes);
	for (vector<NodeID>::const_iterator it = decoration_nodes.begin(); it != decoration_nodes.end(); ++it)
	{
		_nodes_to_decoration[*it] = decoration;
	}

	// neighborhood info for NonWalkableDecorations
	if (decoration->isWalkable())
		return;
	BoardGraph* board_graph = _quest_board->getBoardGraph();
	for (vector<NodeID>::const_iterator it = decoration_nodes.begin(); it != decoration_nodes.end(); ++it)
	{
		const NodeID& current_decoration_node = *it;
		const vector<NodeID>& neighbors = board_graph->getNode(*it).getNeighbors();
		for (vector<NodeID>::const_iterator it_neighbors = neighbors.begin(); it_neighbors != neighbors.end(); ++it_neighbors)
		{
			const NodeID& neighbor_node = *it_neighbors;
			board_graph->getNode(neighbor_node).removeNeighborAbsolute(current_decoration_node);
		}
		board_graph->getNode(current_decoration_node).clearNeighbors();
	}
}

/*!
 * @param node_id
 * @return True if node_id is walkable; false otherwise.
 */
bool Playground::isWalkable(const NodeID& node_id) const
{
    // check for non-walkable decoration
    map<NodeID, Decoration*>::const_iterator it = _nodes_to_decoration.find(node_id);
    if (it != _nodes_to_decoration.end() && it->second != 0 && !it->second->isWalkable())
        return false;

    return true;
}

/*!
 * \return True, if node_id is currently occupied by a creature; false otherwise.
 */
bool Playground::isFieldOccupiedByCreature(const NodeID& node_id) const
{
	map<NodeID, Creature*>::const_iterator it = _nodes.find(node_id);
	if (it != _nodes.end() && it->second != 0)
	{
		//cout << "There is a creature at " << node_id << endl;
		return true;
	}

	//cout << "There is currently no creature at " << node_id << endl;
	return false;
}

/*!
 * \return True, if node_id is currently occupied by a piece of non-walkable decoration; false otherwise.
 */
bool Playground::isFieldOccupiedByDecoration(const NodeID& node_id) const
{
    map<NodeID, Decoration*>::const_iterator it = _nodes_to_decoration.find(node_id);
    if (it != _nodes_to_decoration.end() && it->second != 0)
    {
        Decoration* decoration = it->second;
        if (!decoration->isWalkable())
        {
            //cout << "There is a piece of non-walkable decoration at " << node_id << endl;
            return true;
        }
    }

    //cout << "There is currently no non-walkable piece of decoration at " << node_id << endl;
    return false;
}

/*!
 * \return True, if node_id is currently occupied by a creature or a piece of non-walkable decoration; false otherwise.
 */
bool Playground::isFieldOccupied(const NodeID& node_id) const
{
    return isFieldOccupiedByCreature(node_id) || isFieldOccupiedByDecoration(node_id);
}

bool Playground::containsTrap(const NodeID& node_id) const
{
	// is any trap positioned on node_id?
	const vector<Trap*>& traps = HeroQuestLevelWindow::_hero_quest->getLevel()->getTraps();
	for (unsigned int i = 0; i < traps.size(); ++i)
	{
		if (traps[i]->getNodeID() == node_id)
			return true;
	}

	return false;
}

Trap* Playground::getTrap(const NodeID& node_id)
{
	vector<Trap*>& traps = HeroQuestLevelWindow::_hero_quest->getLevel()->getTraps();
	for (unsigned int i = 0; i < traps.size(); ++i)
	{
		if (traps[i]->getNodeID() == node_id)
			return traps[i];
	}

	return 0;
}

const map<Creature*, NodeID>& Playground::getCreatures() const
{
	return _creatures;
}

bool Playground::moveCreature(Creature* creature, const NodeID& node_id)
{
    if (_nodes[node_id] != 0)
    {
        DVX(
                ("ERROR: Playground::moveCreature: cannot place creature \"%s\" at node (%d, %d): already occupied!", qPrintable(creature->getName()), node_id._ix, node_id._iy));
        return false;
    }

    map<Creature*, NodeID>::const_iterator it_creature_to_node_id = _creatures.find(creature);
    if (it_creature_to_node_id == _creatures.end())
    {
        DVX(
                ("ERROR: Playground::moveCreature: creature \"%s\" not found in _creatures!", qPrintable(creature->getName())));
        return false;
    }
    NodeID old_node_id = it_creature_to_node_id->second;

	_nodes.erase(old_node_id);

	_creatures[creature] = node_id;
	_nodes[node_id] = creature;	

	return true;
}

void Playground::removeCreatureFromMaps(Creature* creature)
{
	map<Creature*, NodeID>::iterator it = _creatures.find(creature);
	if (it == _creatures.end())
	{
		cout << "Playground::removeCreature: Warning: creature already removed" << endl;
		return;
	}

	NodeID current_id = it->second;
	
	_creatures.erase(it);

	_nodes.erase(current_id);
}
#if 0
void Playground::removeCreatures()
{
	for (map<Creature*, NodeID>::iterator it = _creatures.begin(); it != _creatures.end(); ++it)
	{
		Creature* current_creature = it->first;

		if (current_creature != 0)
		{
			delete current_creature;
		}
		else
		{
			cout << "Playground::removeCreatures: Warning: creature == 0" << endl;
		}
	}

	_nodes.clear();
}

int Playground::getFieldSize() const
{
	return _quest_board->getFieldWidth();
}
#endif
void Playground::setActionMode(ActionMode action_mode, const Hero* related_hero)
{
	_action_mode = action_mode;
    _action_mode_related_hero = related_hero;
}

Playground::ActionMode Playground::getActionMode() const
{
	return _action_mode;
}

const Hero* Playground::getActionModeRelatedHero() const
{
    return _action_mode_related_hero;
}

#if 0
bool Playground::contains(const Vec2i& pixel) const
{
	if (_quest_board == 0)
		return false;

	return _quest_board->contains(pixel);
}
#endif
bool Playground::contains(const NodeID& node_id) const
{
	if (_quest_board == 0)
		return false;

	return _quest_board->contains(node_id);
}

const NodeID* Playground::getCreaturePos(Creature* creature) const
{
	map<Creature*, NodeID>::const_iterator it = _creatures.find(creature);
	if (it == _creatures.end())
		return 0;
	return &(it->second);
}

const NodeID* Playground::getCreaturePos(const Creature& creature) const
{
	map<Creature*, NodeID>::const_iterator it = _creatures.find(&const_cast<Creature&>(creature));
	if (it == _creatures.end())
		return 0;
	return &(it->second);
}

const NodeID* Playground::getCurrentlyActingCreaturePos() const
{
    Creature* current_creature = HeroQuestLevelWindow::_hero_quest->getLevel()->getCurrentlyActingCreature();
    if (current_creature == 0)
        return 0;

    return getCreaturePos(current_creature);
}

/*!
 * param node_id
 * @return true iff node_id is on a stairway
 */
bool Playground::nodeIsOnStairway(const NodeID& node_id) const
{
    map<NodeID, Decoration*>::const_iterator it = _nodes_to_decoration.find(node_id);
    if (it == _nodes_to_decoration.end())
        return false;

    const Decoration* decoration = it->second;
    return dynamic_cast<const Stairs*>(decoration) != 0;
}

Creature* Playground::getCreature(const NodeID& node_id)
{
	Creature* creature = 0;

	std::map<NodeID, Creature*>::iterator it = _nodes.find(node_id);
	if (it != _nodes.end())
		creature = it->second;

	return creature;
}

Monster* Playground::getMonster(const NodeID& node_id)
{
    Creature* creature = 0;

    std::map<NodeID, Creature*>::iterator it = _nodes.find(node_id);
    if (it != _nodes.end())
        creature = it->second;

    return dynamic_cast<Monster*>(creature);
}

/*!
 * If respect_target_node_border is false, then the borders of target fields occupied by creatures
 * are not respected during the sectioning algorithm, i.e. then the target node is seen although
 * there is a creature standing on it.
 */
void Playground::computeViewableNodes(const NodeID& node_id, bool respect_target_node_border, set<NodeID>* viewable_nodes) const
{
	viewable_nodes->clear();
	viewable_nodes->insert(node_id);

	for (unsigned int y = 0; y < (unsigned int)(QuestBoard::FIELDS_Y); ++y)
		for (unsigned int x = 0; x < (unsigned int)(QuestBoard::FIELDS_X); ++x)
		{
			const NodeID check_node_id(x, y);
			
			if (check_node_id == node_id)
				continue;

			if (_quest_board->fieldCanBeViewedFromField(node_id, check_node_id, respect_target_node_border))
				viewable_nodes->insert(check_node_id);
		}
}

/*!
 * Stores the secret doors which are viewable from node_id in viewable_secret_doors.
 */
void Playground::computeViewableSecretDoors(const set<NodeID>& viewable_nodes, vector<SecretDoor*>* viewable_secret_doors) const
{
	viewable_secret_doors->clear();

	vector<Door*>& doors = HeroQuestLevelWindow::_hero_quest->getLevel()->getDoors();
	for (unsigned int i = 0; i < doors.size(); ++i)
	{
		const NodeID& field1 = doors[i]->getF1();
		const NodeID& field2 = doors[i]->getF2();

		if (viewable_nodes.find(field1) != viewable_nodes.end() || viewable_nodes.find(field2) != viewable_nodes.end())
		{
			// doors[i] is viewable from node_id
			if (typeid(*(doors[i])) == typeid(SecretDoor))
			{
				SecretDoor* secret_door = dynamic_cast<SecretDoor*>(doors[i]);
				viewable_secret_doors->push_back(secret_door);
			}
		}
	}
}

void Playground::computeViewableTraps(const set<NodeID>& viewable_nodes, vector<Trap*>* viewable_traps) const
{
	vector<Trap*>& traps = HeroQuestLevelWindow::_hero_quest->getLevel()->getTraps();
	for (unsigned int i = 0; i < traps.size(); ++i)
	{
		const NodeID& trap_node_id = traps[i]->getNodeID();

		if (viewable_nodes.find(trap_node_id) != viewable_nodes.end())
		{
			// traps[i] is viewable from node_id
			if (typeid(*(traps[i])) == typeid(PitTrap))
			{
				PitTrap* pit_trap = dynamic_cast<PitTrap*>(traps[i]);
				viewable_traps->push_back(pit_trap);
			}
		}
	}
}

void Playground::computeIncidentCreatures(const NodeID& node_id, vector<Creature*>* incident_creatures) const
{
    Playground* playground = HeroQuestLevelWindow::_hero_quest->getPlayground();

	incident_creatures->clear();
	
	const vector<NodeID>& neighbors = _quest_board->getBoardGraph()->getNode(node_id).getNeighbors();
	for (vector<NodeID>::const_iterator it = neighbors.begin(); it != neighbors.end(); ++it)
	{
		const NodeID& current_neighbor = *it;

		// is current_neighbor_node occupied by a creatures?
        Creature* neighbor_creature = playground->getCreature(current_neighbor);
		if (neighbor_creature != 0)
		{
			incident_creatures->push_back(neighbor_creature);
		}
	}
}

void Playground::computeAdjacentHeroes(Monster* monster, vector<Hero*>* adjacent_heroes) const
{
    Playground* playground = HeroQuestLevelWindow::_hero_quest->getPlayground();

	adjacent_heroes->clear();
	const NodeID* monster_node_id = getCreaturePos(monster);

	const vector<NodeID>& neighbors = _quest_board->getBoardGraph()->getNode(*monster_node_id).getNeighbors();
	for (vector<NodeID>::const_iterator it = neighbors.begin(); it != neighbors.end(); ++it)
	{
		const NodeID& current_neighbor = *it;

		// is current_neighbor_node occupied by a hero?
        Creature* neighbor_creature = playground->getCreature(current_neighbor);
		if ((neighbor_creature != 0) && neighbor_creature->isHero())
		{
			adjacent_heroes->push_back(dynamic_cast<Hero*>(neighbor_creature));
		}
	}
}

/*!
 * Clears nearest_heroes. Then, adds those heroes to nearest_heroes, which are nearest reachable from monster.
 */
void Playground::computeNearestHeroes(Monster* monster, vector<Hero*>* nearest_heroes)
{
    computeNearestHeroes(monster, HeroQuestLevelWindow::_hero_quest->getLevel()->getActingHeroes(), nearest_heroes);
}

/*!
 * Clears nearest_heroes. Then, adds those heroes to nearest_heroes, which are in selected_heroes, and
 * are nearest reachable from monster.
 */
void Playground::computeNearestHeroes(Monster* monster, const vector<Hero*>& selected_heroes, vector<Hero*>* nearest_heroes)
{
	nearest_heroes->clear();

	// get monster's node_id
    map<Creature*, NodeID>::const_iterator monster_node_id_it = _creatures.find(monster);
    if (monster_node_id_it == _creatures.end())
    {
        DVX(
                ("ERROR: Playground::computeNearestHeroes monster %s not found in _creatures!", qPrintable(Monster::getName(monster))));
        return;
    }
    const NodeID& monster_node_id = monster_node_id_it->second;

	// get the shortest path from monster to each selected hero
	map<Hero*, int> hero_path_lengths;
	int shortest_path_length = INT_MAX;
	for (vector<Hero*>::const_iterator it = selected_heroes.begin(); it != selected_heroes.end(); ++it)
	{
		Hero* current_hero = *it;
        map<Creature*, NodeID>::const_iterator creature_to_node_id_it = _creatures.find(current_hero);
        if (creature_to_node_id_it == _creatures.end())
        {
            DVX(
                    ("ERROR: Playground::computeNearestHeroes selected hero %s not found in _creatures!", qPrintable(current_hero->getName())));
            continue;
        }
        const NodeID& current_hero_node_id = creature_to_node_id_it->second;
		vector<NodeID> path_to_current_hero;
        _quest_board->getBoardGraph()->getShortestPath(monster_node_id, current_hero_node_id, false, true,
                current_hero->getCanMoveThroughWalls(), UINT_MAX, &path_to_current_hero);

		int path_length = int(path_to_current_hero.size());
		if (path_length > 0)
		{
			hero_path_lengths[current_hero] = path_length;

			if (path_length < shortest_path_length)
				shortest_path_length = path_length;
		}
	}

	// add those heroes, who have the minimum path length, to nearest_heroes
	for (map<Hero*, int>::iterator it = hero_path_lengths.begin(); it != hero_path_lengths.end(); ++it)
	{
		Hero* hero = it->first;
		int path_length = it->second;

		if (path_length == shortest_path_length)
			nearest_heroes->push_back(hero);
	}
}

/*!
 * Clears reachable_heroes. Then, adds those heroes to reachable_heroes, which are in reachable (in 1 turn) from
 * from monster, assuming that the monster has not moved yet.
 */
void Playground::computeReachableHeroes(Monster* monster, bool one_turn_policy, vector<Hero*>* reachable_heroes)
{
    computeReachableHeroes(monster, HeroQuestLevelWindow::_hero_quest->getLevel()->getActingHeroes(), one_turn_policy,
            reachable_heroes);
}

/*!
 * Clears reachable_heroes. Then, adds those heroes among selected_heros to reachable_heroes, which are reachable
 * from monster, assuming that the monster has not moved yet.
 *
 * \param one_turn_policy If true, only heroes reachable in 1 turn are added to reachable_heroes.
 */
void Playground::computeReachableHeroes(Monster* monster, const vector<Hero*>& selected_heroes, bool one_turn_policy, vector<Hero*>* reachable_heroes)
{
	reachable_heroes->clear();

	// get monster's node_id
	const NodeID& monster_node_id = _creatures[monster];

	unsigned int maximum_path_length = (unsigned int)(monster->getNumMovePoints());

	// see if there is a shortest path from monster to each hero;
	// if there is no path to a hero, then that hero is not reachable
	for (vector<Hero*>::const_iterator it = selected_heroes.begin(); it != selected_heroes.end(); ++it)
	{
		Hero* current_hero = *it;
        map<Creature*, NodeID>::const_iterator it_creature_to_node_id = _creatures.find(current_hero);
        if (it_creature_to_node_id == _creatures.end())
        {
            DVX(
                    ("ERROR: Playground::computeReachableHeroes: hero \"%s\" not found in _creatures!", qPrintable(current_hero->getName())));
            continue;
        }
        const NodeID& current_hero_node_id = it_creature_to_node_id->second;
		vector<NodeID> path_to_current_hero;
        _quest_board->getBoardGraph()->getShortestPath(monster_node_id, current_hero_node_id, false, true,
                current_hero->getCanMoveThroughWalls(), UINT_MAX, &path_to_current_hero);

		if (path_to_current_hero.size() > 0)
		{
			if (path_to_current_hero.size() <= maximum_path_length || !one_turn_policy)
			{
				// current_hero is reachable
				reachable_heroes->push_back(current_hero);
			}
		}
	}
}

/*!
 * Computes a shortest path from creature_from to creature_to and stores it in path_next_to_creature_to.
 * If the path exceeds max_path_length, the path will be truncated accordingly.
 * path_next_to_creature_to will not contain the node IDs of creature_from and creature_to. However,
 * max_path_length is the maximum path length from creature_from to creature_to, including node creature_to.
 */
void Playground::computeShortestPathNextTo(Creature* creature_from, Creature* creature_to, bool pass_heroes,
        bool pass_monsters, bool through_walls, unsigned int max_path_length,
        vector<NodeID>* path_next_to_creature_to) const
{
	path_next_to_creature_to->clear();

	map<Creature*, NodeID>::const_iterator it_from = _creatures.find(creature_from);
	if (it_from == _creatures.end())
		return;
	const NodeID& node_from = it_from->second;

	map<Creature*, NodeID>::const_iterator it_to = _creatures.find(creature_to);
	if (it_to == _creatures.end())
		return;
	const NodeID& node_to = it_to->second;

	// compute the shortest path including node_to
    _quest_board->getBoardGraph()->getShortestPath(node_from, node_to, pass_heroes, pass_monsters,
            through_walls,
            max_path_length, path_next_to_creature_to);
	
	// remove node_to from the path (if contained)
	if (path_next_to_creature_to->size() != 0 && path_next_to_creature_to->back() == node_to)
		path_next_to_creature_to->pop_back();
}

/*!
 * Computes a position for a roaming monster which should attack the hero who stands at target_node.
 *
 * \param target_node The position of the hero.
 * \param roaming_monster_pos Is set to the determined position.
 * \return True, if a position for the roaming monster could be determined.
 */
bool Playground::computeRoamingMonsterPos(const NodeID& target_node, NodeID* roaming_monster_pos) const
{
	// 1) Can we put the monster on a field directly adjacent to target_node?
	if (_quest_board->getBoardGraph()->getNearestNeighborNode(target_node, roaming_monster_pos))
		return true;

	// 2) All neighbors are occupied => choose a field in the same room.
	if (_quest_board->getRoomID(target_node) != 0)
	{
		const list<NodeID>& room_fields = _quest_board->getRoomFields(_quest_board->getRoomID(target_node));
		for (list<NodeID>::const_iterator it = room_fields.begin(); it != room_fields.end(); ++it)
		{
            if (!isFieldOccupied(*it))
			{
				*roaming_monster_pos = *it;
				return true;
			}
		}
	}

	// 3) Find a field in the "same" hallway, i.e. an unoccupied field which is visible from target_node.
	set<NodeID> viewable_nodes;
	computeViewableNodes(target_node, false, &viewable_nodes);
	for (set<NodeID>::const_iterator it = viewable_nodes.begin(); it != viewable_nodes.end(); ++it)
	{
        if (!isFieldOccupied(*it))
		{
			*roaming_monster_pos = *it;
			return true;
		}
	}

	// no such field available
	return false;
}

/*!
 * \return True, if some monster can be viewed from the hero's position, false otherwise.
 */
bool Playground::heroSeesMonster(Hero* hero) const
{
	for (map<Creature*, NodeID>::const_iterator it = _creatures.begin(); it != _creatures.end(); ++it)
	{
		const Creature* creature = it->first;
		if (creature->isMonster())
		{
			const NodeID& monster_node_id = it->second;
			const NodeID& hero_node_id = _creatures.find(hero)->second;
			// is monster_node viewed from hero_node?
			if (_quest_board->fieldCanBeViewedFromField(monster_node_id, hero_node_id, false))
				return true;
		}
	}

	return false;
}

/*!
 * @param creature
 * @param hero
 * @return true, iff creature is in horizontal or vertical line of sight of hero
 */
bool Playground::creatureIsInHorizontalOrVerticalLineOfSightOfHero(const Creature* creature, const Hero* hero) const
{
    const NodeID* creature_pos = getCreaturePos(*creature);
    const NodeID* hero_pos = getCreaturePos(*hero);

    // check if creature and hero are in same column or in same row
    if (creature_pos->_ix != hero_pos->_ix && creature_pos->_iy != hero_pos->_iy)
    {
        // neither same column nor same row
        return false;
    }

    // check if creature is visible by hero
    if (!_quest_board->fieldCanBeViewedFromField(*creature_pos, *hero_pos, false))
    {
        // not viewed
        return false;
    }

    return true;
}

QPixmap* Playground::getSecretDoorImage() const
{
	return _secret_door_image;
}

QPixmap* Playground::getPitTrapImage() const
{
	return _pit_trap_image;
}

QPixmap* Playground::getSpearTrapImage() const
{
	return _spear_trap_image;
}

QPixmap* Playground::getDebrisImage() const
{
	return _debris_image;
}

QPixmap* Playground::getAlchemistTableImage() const
{
	return _alchemist_table_image;
}

QPixmap* Playground::getArmoryImage() const
{
	return _armory_image;
}

QPixmap* Playground::getChestImage() const
{
	return _chest_image;
}

QPixmap* Playground::getCupboardImage() const
{
	return _cupboard_image;
}

QPixmap* Playground::getFireplaceImage() const
{
	return _fireplace_image;
}

QPixmap* Playground::getSarcophagusImage() const
{
	return _sarcophagus_image;
}

QPixmap* Playground::getStairsImage() const
{
	return _stairs_image;
}

QPixmap* Playground::getTableImage() const
{
	return _table_image;
}

QPixmap* Playground::getThroneImage() const
{
	return _throne_image;
}

QPixmap* Playground::getTortureRackImage() const
{
	return _torture_rack_image;
}

QPixmap* Playground::getWarlockTableImage() const
{
	return _warlock_table_image;
}

void Playground::setNodeVisibility(const NodeID& node_id, bool visible)
{
	_node_visibility[node_id._iy][node_id._ix] = visible;
}

bool Playground::getNodeVisibility(const NodeID& node_id)
{
	return _node_visibility[node_id._iy][node_id._ix];
}

void Playground::setNodeVisibility(const set<NodeID>& nodes, bool visible)
{
	for (set<NodeID>::const_iterator it = nodes.begin(); it != nodes.end(); ++it)
	{
		setNodeVisibility(*it, visible);
	}
}

void Playground::setNodeVisibility(const list<NodeID>& nodes, bool visible)
{
    for (list<NodeID>::const_iterator it = nodes.begin(); it != nodes.end(); ++it)
    {
        setNodeVisibility(*it, visible);
    }
}

/*!
 * Sets all nodes which are viewable from node_id visible.
 *
 * @param node_id
 */
void Playground::setNodesViewableFromNodeVisible(const NodeID& node_id)
{
    set<NodeID> viewable_nodes;
    computeViewableNodes(node_id, false, &viewable_nodes);
    setNodeVisibility(viewable_nodes, true);
}

/*!
 * Sets all nodes of the given room visible.
 *
 * @param door
 */
void Playground::setRoomVisible(const Door* door)
{
    unsigned int room_id = _quest_board->getRoomID(door->getF1());
    if (room_id == 0)
        room_id = _quest_board->getRoomID(door->getF2());

    const list<NodeID>& room_fields = _quest_board->getRoomFields(room_id);
    setNodeVisibility(room_fields, true);
}

/*!
 * Sets all fields which can be seen from creature visible.
 */
void Playground::updateFieldVisibility(const Creature& creature)
{
    setNodesViewableFromNodeVisible(*getCreaturePos(creature));
}

/*!
 * Sets all fields which can be seen any hero to visible.
 */
void Playground::updateFieldVisibility()
{
	for (map<Creature*, NodeID>::const_iterator it = _creatures.begin(); it != _creatures.end(); ++it)
	{
		const Creature* creature = it->first;
		if (creature->isHero())
			updateFieldVisibility(*creature);
	}
}

bool Playground::save(ostream& stream) const
{
    //! Map of heroes (barbarian, dwarf, alb, magician) and monsters, mapped to Playground positions (NodeID)
    StreamUtils::writeUInt(stream, _creatures.size());
    for (map<Creature*, NodeID>::const_iterator it = _creatures.begin(); it != _creatures.end(); ++it)
    {
        StreamUtils::writeUInt(stream, it->first->getReferencingID());
        DV(("Saving creature referencing id %d", it->first->getReferencingID()));
        it->second.save(stream);
    }

    //! Mapping of nodes to decoration
    StreamUtils::writeUInt(stream, _nodes_to_decoration.size());
    for (map<NodeID, Decoration*>::const_iterator it = _nodes_to_decoration.begin(); it != _nodes_to_decoration.end(); ++it)
    {
        it->first.save(stream);
        StreamUtils::writeUInt(stream, it->second->getReferencingID());
    }

    // _node_visibility
    uint node_visibility_width = _node_visibility.getWidth();
    uint node_visibility_height = _node_visibility.getHeight();
    StreamUtils::writeUInt(stream, node_visibility_width);
    StreamUtils::writeUInt(stream, node_visibility_height);
    for (uint y = 0; y < node_visibility_height; ++y)
        for (uint x = 0; x < node_visibility_width; ++x)
        {
            StreamUtils::writeBool(stream, _node_visibility[y][x]);
        }

    // _action_mode
    StreamUtils::writeUInt(stream, uint(_action_mode));
    // _action_mode_related_hero
    uint related_hero_referencing_id =
            _action_mode_related_hero != 0 ? _action_mode_related_hero->getReferencingID() : UINT_MAX;
    StreamUtils::writeUInt(stream, related_hero_referencing_id);

    _quest_board->save(stream);

    return !stream.fail();
}

bool Playground::load(istream& stream)
{
    _creatures.clear();
    _nodes.clear();
    uint num_creatures;
    StreamUtils::readUInt(stream, &num_creatures);
    for (uint i = 0; i < num_creatures; ++i)
    {
        uint ref_id;
        StreamUtils::readUInt(stream, &ref_id);
        NodeID node_id;
        node_id.load(stream);

        Creature* creature = HeroQuestLevelWindow::_hero_quest->getCreature(ref_id);

        _creatures[creature] = node_id;
        _nodes[node_id] = creature;
    }
    DV(("Playground::load: %d creatures", num_creatures));

    _nodes_to_decoration.clear();
    uint num_decoration;
    StreamUtils::readUInt(stream, &num_decoration);
    for (uint i = 0; i < num_decoration; ++i)
    {
        NodeID node_id;
        node_id.load(stream);

        uint ref_id;
        StreamUtils::readUInt(stream, &ref_id);

        Decoration* decoration = HeroQuestLevelWindow::_hero_quest->getLevel()->getDecoration(ref_id);

        _nodes_to_decoration[node_id] = decoration;
    }
    DV(("Playground::load: %d decoration", num_decoration));

    // _node_visibility
    uint node_visibility_width;
    uint node_visibility_height;
    StreamUtils::readUInt(stream, &node_visibility_width);
    StreamUtils::readUInt(stream, &node_visibility_height);
    _node_visibility.resize(node_visibility_width, node_visibility_height);
    for (uint y = 0; y < node_visibility_height; ++y)
        for (uint x = 0; x < node_visibility_width; ++x)
        {
            bool value;
            StreamUtils::readBool(stream, &value);

            _node_visibility[y][x] = value;
        }
    DV(("Playground::load: node_visibility finished"));

    // _action_mode
    uint action_mode_uint;
    StreamUtils::readUInt(stream, &action_mode_uint);
    _action_mode = ActionMode(action_mode_uint);
    DV(("Playground::load: action_mode %d", action_mode_uint));

    // _action_mode_related_hero
    uint ref_id;
    StreamUtils::readUInt(stream, &ref_id);
    _action_mode_related_hero = 0;
    if (ref_id != UINT_MAX)
    {
        _action_mode_related_hero = HeroQuestLevelWindow::_hero_quest->getHero(ref_id);
    }

    if (_quest_board != 0)
    {
        delete _quest_board;
        _quest_board = new QuestBoard();
        _quest_board->load(stream);
    }
    DV(("Playground::load: QuestBoard loaded"));

    return !stream.fail();
}

bool Playground::loadImagesAndAdjustPointers()
{
    return create(true /*only load images*/); // images, and _treasure_card_images map
}

bool Playground::event(QEvent* event)
{
    if (_quest_board != 0 &&
            (event->type() == QEvent::MouseButtonPress ||
                    event->type() == QEvent::MouseButtonRelease ||
                    event->type() == QEvent::MouseMove))
    {
        _quest_board->mouseEvent(dynamic_cast<QMouseEvent*>(event));
        return true;
    }

    QWidget::event(event);
    return false;
}

void Playground::paintEvent(QPaintEvent* event)
{
    //cout << "paint " << event->rect().x() << " " << event->rect().y() << " " << event->rect().width() << " " << event->rect().height() << endl;
    QPainter painter(this);

#if 0
    QPen pen;
    pen.setColor(QColor(255,0,0,255));
    painter.setPen(pen);

    painter.drawRect(QRect(0, 0, width() - 1, height() - 1));
#endif
    _quest_board->redraw(painter);

    // decoration
    vector<Decoration*>& decoration = HeroQuestLevelWindow::_hero_quest->getLevel()->getDecoration();
    for (uint i = 0; i < decoration.size(); ++i)
    {
        Decoration* current_decoration = decoration[i];
        current_decoration->redraw(painter);
    }

    // doors
    vector<Door*>& doors = HeroQuestLevelWindow::_hero_quest->getLevel()->getDoors();
    for (uint i = 0; i < doors.size(); ++i)
    {
        Door* current_door = doors[i];
        current_door->redraw(painter, _quest_board->isDestinationDoor(*(doors[i])));
    }

    // traps
    {
        AutoMutex am(HeroQuestLevelWindow::_hero_quest->getLevel()->getTrapsMutex());

        vector<Trap*>& traps = HeroQuestLevelWindow::_hero_quest->getLevel()->getTraps();
        for (uint i = 0; i < traps.size(); ++i)
        {
            Trap* current_trap = traps[i];

            //ACTION_MODE_SELECT_ADJACENT_FIELD
            // pit trap selection, visible, and adjacent to hero?
            bool highlight = HeroQuestLevelWindow::_hero_quest->getUserInteractionMode()
                    == HeroQuestLevelWindow::USER_INTERACTION_SELECT_ADJACENT_PIT_TRAP && current_trap->getVisible()
                    && _quest_board->getBoardGraph()->getNode(current_trap->getNodeID()).isNeighbor(
                            *(getCreaturePos(*getActionModeRelatedHero())));
            Qt::GlobalColor highlight_color = Qt::blue;

            if (!current_trap->isSpearTrap()) // spear trap is drawn over creatures (=> see further below)
                current_trap->redraw(painter, highlight, highlight_color);
        }
    }

    // creatures
    int field_size = ParameterStorage::instance->getFieldSize();
    for (map<Creature*, NodeID>::iterator it = _creatures.begin(); it != _creatures.end(); ++it)
    {
        Creature* creature = it->first;
        if (creature == 0)
        {
            cout << "Creature is 0" << endl;
            return;
        }
        const NodeID& node_id = it->second;

        int max_draw_size = field_size - (0.15 * field_size);

        bool highlight = (HeroQuestLevelWindow::_hero_quest->getLevel()->getCurrentlyActingCreature() == creature)
                || (HeroQuestLevelWindow::_hero_quest->getUserInteractionMode()
                        == HeroQuestLevelWindow::USER_INTERACTION_SELECT_CREATURE)
                || (HeroQuestLevelWindow::_hero_quest->getUserInteractionMode()
                        == HeroQuestLevelWindow::USER_INTERACTION_SELECT_CREATURE_OR_DOOR)
                || (HeroQuestLevelWindow::_hero_quest->getUserInteractionMode()
                                == HeroQuestLevelWindow::USER_INTERACTION_SELECT_HERO && creature->isHero())
                        || (HeroQuestLevelWindow::_hero_quest->getUserInteractionMode()
                                == HeroQuestLevelWindow::USER_INTERACTION_SELECT_CREATURE_IN_HORIZONTAL_OR_VERTICAL_LINE_OF_SIGHT
                                && creatureIsInHorizontalOrVerticalLineOfSightOfHero(creature,
                                        getActionModeRelatedHero()));
        Qt::GlobalColor highlight_color =
                (_action_mode == ACTION_MODE_SELECT_FIELD_OR_DOOR)
                        || (_action_mode == ACTION_MODE_SELECT_FIELD_IN_VISUAL_LINE_OF_SIGHT
                                && creature != getActionModeRelatedHero()) ? Qt::blue : Qt::red;

        creature->redrawCentered(
                painter,
                _quest_board->getFieldCenter(node_id),
            max_draw_size,
            max_draw_size,
                highlight, highlight_color);
    }

    // spear traps (drawn over creatures)
    {
        AutoMutex am(HeroQuestLevelWindow::_hero_quest->getLevel()->getTrapsMutex());

        vector<Trap*>& traps = HeroQuestLevelWindow::_hero_quest->getLevel()->getTraps();
        for (unsigned int i = 0; i < traps.size(); ++i)
        {
            Trap* current_trap = traps[i];
            if (current_trap->isSpearTrap())
                current_trap->redraw(painter, false, Qt::black /*dummy*/);
        }
    }

#if 1
    // blackening of QuestBoard fields
    painter.save();
    QBrush brush(Qt::SolidPattern);
    brush.setColor(QColor(0, 0, 0, 255));
    for (unsigned int y = 0; y < (unsigned int)(QuestBoard::FIELDS_Y); ++y)
        for (unsigned int x = 0; x < (unsigned int)(QuestBoard::FIELDS_X); ++x)
        {
            const NodeID node(x, y);
            if (!getNodeVisibility(node))
            {
                Vec2i p0(_quest_board->getFieldCorner0(node));
                Vec2i p3(_quest_board->getFieldCorner3(node));
                painter.fillRect(p0.x, p0.y, p3.x - p0.x, p3.y - p0.y, brush);
            }
        }
    painter.restore();
#endif

    if (_action_mode == Playground::ACTION_MODE_MOVE_OR_ATTACK_OR_OPEN_DOOR)
    {
        // QuestBoard: movement path (moved here because must be done after drawing creatures)
        {
            PainterContext painter_context(painter);

            QColor color;
            color.setRgb(40, 150, 40, 120);
            QBrush brush(color);
            painter.setBrush(brush);

            for (vector<NodeID>::const_iterator it = _quest_board->getMovementPath().begin();
                    it != _quest_board->getMovementPath().end(); ++it)
            {
                const NodeID node_id = *it;
                Vec2i field_center(_quest_board->getFieldCenter(node_id));
                int field_width = _quest_board->getFieldCorner3(node_id).x - _quest_board->getFieldCorner0(node_id).x;
                int field_height = _quest_board->getFieldCorner3(node_id).y - _quest_board->getFieldCorner0(node_id).y;
                int min_field_size = min(field_width, field_height);
                int circle_radius = int(min_field_size / 2 * 0.7);

                painter.drawEllipse(QPoint(field_center.x, field_center.y), circle_radius, circle_radius);
            }
        }

        // QuestBoard: attack destination (moved here because must be done after drawing creatures)
        {
            PainterContext painter_context(painter);

            if (_quest_board->getAttackDestinationValid())
            {
                QColor color;
                color.setRgb(150, 40, 40, 120);
                QBrush brush(color);
                painter.setBrush(brush);

                const NodeID& attack_destination = _quest_board->getAttackDestination();
                Vec2i field_center(_quest_board->getFieldCenter(attack_destination));
                int field_width = _quest_board->getFieldCorner3(attack_destination).x
                        - _quest_board->getFieldCorner0(attack_destination).x;
                int field_height = _quest_board->getFieldCorner3(attack_destination).y
                        - _quest_board->getFieldCorner0(attack_destination).y;
                int min_field_size = min(field_width, field_height);
                int circle_radius = int(min_field_size / 2 * 0.7);

                painter.drawEllipse(QPoint(field_center.x, field_center.y), circle_radius, circle_radius);
            }
        }
    }

#if 0
    // DEBUG
    vector<NodeID> viewed_fields;
    _quest_board->computeViewedFields(_check_field, &viewed_fields);
    for (unsigned int i = 0; i < viewed_fields.size(); ++i)
    {
        const NodeID& field = viewed_fields[i];
        Vec2i field_center(_quest_board->getFieldCenter(field));
        int field_size = _quest_board->getFieldCorner3(field).x - _quest_board->getFieldCorner0(field).x;
        int field_height = _quest_board->getFieldCorner3(field).y - _quest_board->getFieldCorner0(field).y;
        int min_field_size = min(field_size, field_height);
        int circle_radius = int(min_field_size / 2 * 0.7);
        ALLEGRO_COLOR color = al_map_rgb(0, 0, 255);
        al_draw_filled_circle(field_center.x, field_center.y, circle_radius, color);
    }
#endif
}
