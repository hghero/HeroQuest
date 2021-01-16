#ifndef HERO_STATISTIC_PANE_H
#define HERO_STATISTIC_PANE_H

#include <QtCore/QString>
#include <QtWidgets/QLabel>

class Hero;
class QPushButton;

/*!
 * A panel which shows
 *
 * - the hero's name
 *
 * - num_dice_move
 * - number of remaining movement points
 * - num_dice_attack
 * - num_dice_defend
 *
 * - life_points
 * - intelligence_points
 *
 * - gold
 * - weapons
 * - further equipment
 */
class HeroStatisticPane : public QLabel
{
    Q_OBJECT
private:
	static const QString BACKGROUND_YELLOW_FILENAME;

public:
	HeroStatisticPane(Hero* hero);
	virtual ~HeroStatisticPane();

	void update();

	const Hero* getHero() const;

	QPushButton* getInventoryButton();

private:
	QPixmap* _background_image;

	// to get the hero's number of dice, life points, etc.
	Hero* _hero;

    QLabel* _hero_name_label;
    QLabel* _num_dice_move_label;
    QLabel* _num_move_points_remaining_label;
    QLabel* _num_dice_attack_label;
    QLabel* _num_dice_defend_label;
    QLabel* _life_points_label;
    QLabel* _intelligence_points_label;
    QLabel* _gold_label;
    QLabel* _weapons_label;

    QPushButton* _inventory_button;
};

#endif
