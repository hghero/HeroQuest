#ifndef DICE_ROLL_AREA_H
#define DICE_ROLL_AREA_H

#include <vector>

#include <QtWidgets/QLabel>

#include "Math.h"

class QHBoxLayout;

/*!
 * A DiceRollArea shows a player/monster image and the rolled dice result.
 */
class DiceRollArea : public QLabel
{
    Q_OBJECT
public:
	DiceRollArea(int width, const Vec2i& avatar_image_max_size, int dice_width);
	virtual ~DiceRollArea();

	void setContent(QPixmap* avatar_image);
	void setContent(QPixmap* avatar_image, std::vector<QPixmap*>& dice_images);

	void clearContent();

private:
	void clearContent(QLayout* layout);

	QHBoxLayout* _layout;

	//! the player / human / monster which rolls the dice
	QPixmap* _avatar_image; // just a "copy"; not created or destroyed by this class

	//! width of the widget
	int _width;

	//! the maximum size (in pixels) of the area to which the avatar image is drawn (height, width)
	Vec2i _avatar_image_max_size;

	//! the width (in pixels) of each dice (height is not needed, because dice are typically quadratic anyway)
	int _dice_width;
};

#endif
