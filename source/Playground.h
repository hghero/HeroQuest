#ifndef PLAYGROUND_H
#define PLAYGROUND_H

#include <map>
#include <set>
#include "Math.h"
#include "GraphBase.h"
#include "SpellCard.h"
#include <QtWidgets/QLabel>
#include <QtWidgets/QWidget>


class HeroQuestLevelWindow;
class QuestBoard;
class Creature;
class Monster;
class Hero;
class Decoration;
class SecretDoor;
class Trap;
class QMouseEvent;
class Door;
class SaveContext;
class LoadContext;

// ==================================================================

class Playground : public QLabel
{
    Q_OBJECT

    public:
    enum ActionMode
    {
        ACTION_MODE_NONE = 0,
        ACTION_MODE_MOVE_OR_ATTACK_OR_OPEN_DOOR,
        ACTION_MODE_SELECT_FIELD_OR_DOOR,
        ACTION_MODE_SELECT_FIELD_IN_VISUAL_LINE_OF_SIGHT,
        ACTION_MODE_SELECT_ADJACENT_FIELD
    };

	//! decoration image filenames
	static const QString SECRET_DOOR_FILENAME;
	static const QString PIT_TRAP_FILENAME;
	static const QString SPEAR_TRAP_FILENAME;
	static const QString DEBRIS_FILENAME;

	//! furniture image filenames
	static const QString ALCHEMIST_TABLE_FILENAME;
	static const QString ARMORY_FILENAME;
	static const QString CHEST_FILENAME;
	static const QString CUPBOARD_FILENAME;
	static const QString FIREPLACE_FILENAME;
	static const QString SARCOPHAGUS_FILENAME;
	static const QString STAIRS_FILENAME;
	static const QString TABLE_FILENAME;
	static const QString THRONE_FILENAME;
	static const QString TORTURE_RACK_FILENAME;
	static const QString WARLOCK_TABLE_FILENAME;

	Playground(QWidget* parent = 0);
	~Playground();

	bool create(bool only_load_images = false);

	QuestBoard* getQuestBoard();

	bool addCreature(Creature* creature, const NodeID& node_id);

	void addDecoration(Decoration* decoration);
    bool isWalkable(const NodeID& node_id) const;

	bool moveCreature(Creature* creature, const NodeID& node_id);

	void removeCreatureFromMaps(Creature* creature);

    void setActionMode(ActionMode action_mode, const Hero* related_hero = 0);
	Playground::ActionMode getActionMode() const;
    const Hero* getActionModeRelatedHero() const;

	bool contains(const NodeID& node_id) const;

	const NodeID* getCreaturePos(Creature* creature) const;
	const NodeID* getCreaturePos(const Creature& creature) const;
	const NodeID* getCurrentlyActingCreaturePos() const;

	Creature* getCreature(const NodeID& node_id);
    Monster* getMonster(const NodeID& node_id);

	const std::map<Creature*, NodeID>& getCreatures() const;

	bool isFieldOccupiedByCreature(const NodeID& node_id) const;
    bool isFieldOccupiedByDecoration(const NodeID& node_id) const;
    bool isFieldOccupied(const NodeID& node_id) const;

	bool containsTrap(const NodeID& node_id) const;
	Trap* getTrap(const NodeID& node_id);

    bool nodeIsOnStairway(const NodeID& node_id) const;

    void computeViewableNodes(const NodeID& node_id, bool respect_target_node_border, bool view_through_heroes,
            std::set<NodeID>* viewable_nodes) const;
	void computeViewableSecretDoors(const std::set<NodeID>& viewable_nodes, std::vector<SecretDoor*>* viewable_secret_doors) const;
	void computeViewableTraps(const std::set<NodeID>& viewable_nodes, std::vector<Trap*>* viewable_traps) const;

	void computeIncidentCreatures(const NodeID& node_id, std::vector<Creature*>* incident_creatures) const;
	void computeAdjacentHeroes(Monster* monster, std::vector<Hero*>* adjacent_heroes) const;
	void computeNearestHeroes(Monster* monster, const std::vector<Hero*>& selected_heroes, std::vector<Hero*>* nearest_heroes);
	void computeNearestHeroes(Monster* monster, std::vector<Hero*>* nearest_heroes);
	void computeReachableHeroes(Monster* monster, const std::vector<Hero*>& selected_heroes, bool one_turn_policy, std::vector<Hero*>* reachable_heroes);
	void computeReachableHeroes(Monster* monster, bool one_turn_policy, std::vector<Hero*>* reachable_heroes);

    void computeShortestPathNextTo(Creature* creature_from, Creature* creature_to, bool pass_heroes, bool pass_monsters,
            bool through_walls, unsigned int max_path_length, std::vector<NodeID>* path_next_to_creature_to) const;

	bool computeRoamingMonsterPos(const NodeID& target_node, NodeID* roaming_monster_pos) const;

	bool heroSeesMonster(Hero* hero) const;

    bool creatureIsInHorizontalOrVerticalLineOfSightOfHero(const Creature* creature, const Hero* hero) const;

	QPixmap* getSecretDoorImage() const;
	QPixmap* getPitTrapImage() const;
	QPixmap* getSpearTrapImage() const;
	QPixmap* getDebrisImage() const;
	QPixmap* getAlchemistTableImage() const;
	QPixmap* getArmoryImage() const;
	QPixmap* getChestImage() const;
	QPixmap* getCupboardImage() const;
	QPixmap* getFireplaceImage() const;
	QPixmap* getSarcophagusImage() const;
	QPixmap* getStairsImage() const;
	QPixmap* getTableImage() const;
	QPixmap* getThroneImage() const;
	QPixmap* getTortureRackImage() const;
	QPixmap* getWarlockTableImage() const;

	void setNodeVisibility(const NodeID& node_id, bool visible);
	bool getNodeVisibility(const NodeID& node_id);
	void setNodeVisibility(const std::set<NodeID>& nodes, bool visible);
    void setNodeVisibility(const std::list<NodeID>& nodes, bool visible);
    void setNodesViewableFromNodeVisible(const NodeID& node_id);
    void setRoomVisible(const Door* door);
	void updateFieldVisibility(const Creature& creature);
	void updateFieldVisibility();
    void makeAllRelevantNodesVisible();

    virtual bool save(SaveContext& save_context) const;
    virtual bool load(LoadContext& load_context);
    bool loadImagesAndAdjustPointers();

private:
	virtual bool event(QEvent* event);

	void paintEvent(QPaintEvent *event);

	//! The quest board
	QuestBoard* _quest_board;

	//! Map of heroes (barbarian, dwarf, alb, magician) and monsters, mapped to Playground positions (NodeID)
	std::map<Creature*, NodeID> _creatures;
	//! reverse mapping
	std::map<NodeID, Creature*> _nodes;

	//! Mapping of nodes to decoration
	std::map<NodeID, Decoration*> _nodes_to_decoration;

	StorageMatrix<bool> _node_visibility;

	//! action mode
	ActionMode _action_mode;
    const Hero* _action_mode_related_hero;

	// Further playground images

	//! Special fields images
	QPixmap* _secret_door_image;
	QPixmap* _pit_trap_image;
	QPixmap* _spear_trap_image;
	QPixmap* _debris_image;

	//! Furniture images
	QPixmap* _alchemist_table_image;
	QPixmap* _armory_image;
	QPixmap* _chest_image;
	QPixmap* _cupboard_image;
	QPixmap* _fireplace_image;
	QPixmap* _sarcophagus_image;
	QPixmap* _stairs_image;
	QPixmap* _table_image;
	QPixmap* _throne_image;
	QPixmap* _torture_rack_image;
	QPixmap* _warlock_table_image;
};

// ==================================================================

#endif
