#include "DiceRollArea.h"

#include <QtWidgets/QLayout>
#include "Debug.h"

//#include "Image.h"

using namespace std;

DiceRollArea::DiceRollArea(int width, const Vec2i& avatar_image_max_size, int dice_width)
:
QLabel(),
_layout(new QHBoxLayout),
_avatar_image(0),
_width(width),
_avatar_image_max_size(avatar_image_max_size),
_dice_width(dice_width)
{
	/*
	 * avatar   |   movement dice
	 */
    setLayout(_layout);
    _layout->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    _layout->setSpacing(dice_width / 2);
    _layout->setMargin(0);
}

DiceRollArea::~DiceRollArea()
{
	// NIX
}

void DiceRollArea::setContent(QPixmap* avatar_image)
{
    vector<QPixmap*> dice_images_dummy;
    setContent(avatar_image, dice_images_dummy);
}

void DiceRollArea::setContent(QPixmap* avatar_image, vector<QPixmap*>& dice_images)
{
    clearContent();

    QLabel* avatar_label = new QLabel;
    _layout->addWidget(avatar_label);
    if (avatar_image != 0)
    {
        avatar_label->setPixmap(
                avatar_image->scaled(_avatar_image_max_size.x, _avatar_image_max_size.y, Qt::KeepAspectRatio));
    }
    avatar_label->setAlignment(Qt::AlignTop);

    /*
     *   avatar_width  space  dice  space  dice  space  ...
     *                        dice  space  dice  space  ...
     *
     *   <-------------------------------- _width ------------------------------->
     */
    int avatar_width = avatar_label->pixmap()->width();
    int dice_width = _dice_width;
    int space = _layout->spacing();

    int max_num_dice_per_row = (_width - (avatar_width + space)) / (dice_width + space);

    /*DVX(("_width %d, avatar_width %d, dice_width %d, space %d, max_num_dice_per_row %d",
            _width, avatar_width, dice_width, space, max_num_dice_per_row));*/

    if (max_num_dice_per_row < 1)
        return;

    int num_rows = (dice_images.size() / max_num_dice_per_row) + (dice_images.size() % max_num_dice_per_row > 0 ? 1 : 0);

    //DVX(("max_num_dice_per_row %d, num_rows % d", max_num_dice_per_row, num_rows));

    /*DVX(("_width %d, avatar_width %d, dice_width %d, space %d, max_num_dice_per_row %d, num_rows %d",
            _width, avatar_width, dice_width, space, max_num_dice_per_row, num_rows));*/

    if (num_rows == 1)
    {
        // only 1 row
        for (uint i = 0; i < dice_images.size(); ++i)
        {
            QLabel* dice_label = new QLabel;
            _layout->addWidget(dice_label);
            dice_label->setPixmap(dice_images[i]->scaled(_dice_width, _dice_width, Qt::KeepAspectRatio));
        }
    }
    else
    {
        // multiple rows => need a sub-layout

        /*
         *  --------------------------------------
         * | avatar    | dice_layout              |
         * |           |                          |
         *  --------------------------------------
         */
        QGridLayout* dice_layout = new QGridLayout;
        _layout->addLayout(dice_layout);
        dice_layout->setSpacing(_layout->spacing());

        int x = 0;
        int y = 0;
        for (uint i = 0; i < dice_images.size(); ++i)
        {
            //DVX(("i %d, x %d, y %d", i, x, y));
            QLabel* dice_label = new QLabel();
            dice_layout->addWidget(dice_label, y, x);
            dice_label->setPixmap(dice_images[i]->scaled(_dice_width, _dice_width, Qt::KeepAspectRatio));

            ++x;
            if (x == max_num_dice_per_row)
            {
                x = 0;
                ++y;
            }
        }
    }
}

void DiceRollArea::clearContent()
{
    if (_layout->count() == 0)
        return;

    // remove all items
    for (int i = _layout->count() - 1; i >= 0; --i)
    {
        QLayoutItem* item = _layout->itemAt(i);
        if (item != 0)
        {
            // sub-layout?
            QLayout* item_layout = dynamic_cast<QLayout*>(item);
            if (item_layout != 0)
            {
                // => delete sub-layout items
                clearContent(item_layout);
            }

            _layout->removeItem(item);
            delete item->widget(); // does not work without this line?!
            delete item;
        }
    }
}

void DiceRollArea::clearContent(QLayout* layout)
{
    if (layout->count() == 0)
        return;

    // remove all items
    for (int i = layout->count() - 1; i >= 0; --i)
    {
        QLayoutItem* item = layout->itemAt(i);
        if (item != 0)
        {
            layout->removeItem(item);
            delete item->widget(); // does not work without this line?!
            delete item;
        }
    }
}
