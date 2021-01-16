#ifndef CREATURE_H
#define CREATURE_H

#include <QtCore/QString>
#include <QtGui/QPixmap>

#include "Math.h"
#include "Reference.h"

// ==================================================================

class Creature : public Reference
{
protected:
	static const QString ICON_FILENAME;

public:
	Creature(const QString& name, int num_dice_attack, int num_dice_defend, int life_points, int life_points_max, int intelligence_points);
	virtual ~Creature();

	bool create();

	virtual const QString& getIconFilename() const;

	const QString& getName() const;

    void redrawCentered(QPainter& painter, const Vec2i& pos, int max_width, int max_height, bool highlight,
            Qt::GlobalColor highlight_color);

	QPixmap* getAvatar();

	virtual int getNumDiceAttack() const;
	void setNumDiceAttack(int num_dice_attack);

	virtual int getNumDiceDefend() const;
	void setNumDiceDefend(int num_dice_defend);

	int getLifePoints() const;
	void setLifePoints(int life_points);
	void addLifePoints(int life_points_add);

	int getIntelligencePoints() const;
	void setIntelligencePoints(int intelligence_points);

	uint getSkipRounds() const;
	void setSkipRounds(uint skip_rounds);
	void increaseSkipRounds(uint inc_skip_rounds = 1);
	void decreaseSkipRounds(uint dec_skip_rounds = 1);

	virtual bool isHero() const;
	virtual bool isMonster() const;
	virtual bool canCastSpells() const;

	virtual Creature& operator=(const Creature& other);
	virtual Creature* copy() const;

    virtual bool save(std::ostream& stream) const;
    virtual bool load(std::istream& stream);

protected:
	bool _created;

	// name of the creature
	QString _name;

	// fighting
	int _num_dice_attack;
	int _num_dice_defend;

	// life and intelligence
	int _life_points;
	int _life_points_max;
	int _intelligence_points;

	// round behaviour
	uint _skip_rounds;

	// the creature icon
	QPixmap* _creature_icon;
	int _creature_icon_orig_size_x;
	int _creature_icon_orig_size_y;
};

#endif
