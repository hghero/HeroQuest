#ifndef TREASURE_CARD_PANEL_H
#define TREASURE_CARD_PANEL_H

#include <QtWidgets/QLabel>
#include <QtWidgets/QLayout>

#include "TreasureCard.h"


class TreasureCardPane : public QLabel
{
    Q_OBJECT

    public:
    // Concept for sizes of UI elements:
    //
    //  ----------   ----------
    // |          | |          | Treasure text
    // |          | | treasure |
    // | treasure | |  image   |
    // |   back   | |          |
    // |  image   |  ----------
    // |          |
    // |          |
    //  ----------
    //
    // The width of the treasure card back image is derived from the playground field size.
    // The other widths of the dialog elements are derived from the treasure card back image width.
    // Heights of the images are defined through width and ratio.
    static const float TREASURE_CARD_BACK_WIDTH_FIELD_SIZE_FACTOR; //!< _treasure_card_back_size.x := (playground field size) * TREASURE_CARD_BACK_WIDTH_FIELD_SIZE_FACTOR
    static const float TREASURE_IMAGE_WIDTH_FACTOR; //!< _treasure_image_size.x := _treasure_card_back_size.x * TREASURE_IMAGE_WIDTH_FACTOR
    static const float TEXT_WIDTH_FACTOR; //!< _text_width := _treasure_card_back_size.x * TEXT_WIDTH_FACTOR

    //! adjusts width and height according to treasure card text; positions the dialog in the middle of the screen
    TreasureCardPane(QWidget* parent, const TreasureCard& treasure_card);

    virtual ~TreasureCardPane();

    virtual QSize sizeHint() const;

    virtual void setContentsMargins(int left, int top, int right, int bottom);

    private:
    QLayout* _outer_layout;
};

#endif
