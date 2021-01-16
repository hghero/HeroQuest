#ifndef NON_WALKABLE_DECORATION_H
#define NON_WALKABLE_DECORATION_H

#include "Decoration.h"

// =============================================================================

class NonWalkableDecoration: public Decoration
{
public:
    typedef Decoration Parent;

	NonWalkableDecoration();
	NonWalkableDecoration(const NodeID& node_top_left, const NodeID& node_bottom_right, const Orientation& orientation);
	virtual ~NonWalkableDecoration();

	virtual QPixmap* getImage() const;

	virtual bool isWalkable() const;

    virtual bool save(std::ostream& stream) const;
    virtual bool load(std::istream& stream);
};

// =============================================================================

class Chest : public NonWalkableDecoration
{
public:
    typedef NonWalkableDecoration Parent;

	Chest();
	Chest(const NodeID& node_top_left, const NodeID& node_bottom_right, const Orientation& orientation);
	Chest(const NodeID& node_top_left, const Orientation& orientation);
	virtual ~Chest();

	virtual QPixmap* getImage() const;

    bool isOpen() const;
    void setOpen(bool value);

    virtual bool save(std::ostream& stream) const;
    virtual bool load(std::istream& stream);

    private:
    bool _open;
};

// =============================================================================

class AlchemistTable : public NonWalkableDecoration
{
public:
	AlchemistTable();
	AlchemistTable(const NodeID& node_top_left, const NodeID& node_bottom_right, const Orientation& orientation);
	AlchemistTable(const NodeID& node_top_left, const Orientation& orientation);
	virtual ~AlchemistTable();

	virtual QPixmap* getImage() const;
};

// =============================================================================

class Armory : public NonWalkableDecoration
{
public:
	Armory();
	Armory(const NodeID& node_top_left, const NodeID& node_bottom_right, const Orientation& orientation);
	Armory(const NodeID& node_top_left, const Orientation& orientation);
	virtual ~Armory();

	virtual QPixmap* getImage() const;
};

// =============================================================================

class Cupboard : public NonWalkableDecoration
{
public:
	Cupboard();
	Cupboard(const NodeID& node_top_left, const NodeID& node_bottom_right, const Orientation& orientation);
	Cupboard(const NodeID& node_top_left, const Orientation& orientation);
	virtual ~Cupboard();

	virtual QPixmap* getImage() const;
};

// =============================================================================

class Fireplace : public NonWalkableDecoration
{
public:
	Fireplace();
	Fireplace(const NodeID& node_top_left, const NodeID& node_bottom_right, const Orientation& orientation);
	Fireplace(const NodeID& node_top_left, const Orientation& orientation);
	virtual ~Fireplace();

	virtual QPixmap* getImage() const;
};

// =============================================================================

class Sarcophagus : public NonWalkableDecoration
{
public:
	Sarcophagus();
	Sarcophagus(const NodeID& node_top_left, const NodeID& node_bottom_right, const Orientation& orientation);
	Sarcophagus(const NodeID& node_top_left, const Orientation& orientation);
	virtual ~Sarcophagus();

	virtual QPixmap* getImage() const;
};

// =============================================================================

class Table : public NonWalkableDecoration
{
public:
	Table();
	Table(const NodeID& node_top_left, const NodeID& node_bottom_right, const Orientation& orientation);
	Table(const NodeID& node_top_left, const Orientation& orientation);
	virtual ~Table();

	virtual QPixmap* getImage() const;
};

// =============================================================================

class Throne : public NonWalkableDecoration
{
public:
	Throne();
	Throne(const NodeID& node_top_left, const NodeID& node_bottom_right, const Orientation& orientation);
	Throne(const NodeID& node_top_left, const Orientation& orientation);
	virtual ~Throne();

	virtual QPixmap* getImage() const;
};

// =============================================================================

class TortureRack : public NonWalkableDecoration
{
public:
	TortureRack();
	TortureRack(const NodeID& node_top_left, const NodeID& node_bottom_right, const Orientation& orientation);
	TortureRack(const NodeID& node_top_left, const Orientation& orientation);
	virtual ~TortureRack();

	virtual QPixmap* getImage() const;
};

// =============================================================================

class WarlockTable : public NonWalkableDecoration
{
public:
	WarlockTable();
	WarlockTable(const NodeID& node_top_left, const NodeID& node_bottom_right, const Orientation& orientation);
	WarlockTable(const NodeID& node_top_left, const Orientation& orientation);
	virtual ~WarlockTable();

	virtual QPixmap* getImage() const;
};

// =============================================================================

class Debris : public NonWalkableDecoration
{
public:
	Debris();
	Debris(const NodeID& node_top_left, const NodeID& node_bottom_right, const Orientation& orientation);
	Debris(const NodeID& node_top_left, const Orientation& orientation);
	virtual ~Debris();

	virtual QPixmap* getImage() const;
};

// =============================================================================

#endif
