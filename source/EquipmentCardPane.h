#ifndef EQUIPMENT_CARD_PANEL_H
#define EQUIPMENT_CARD_PANEL_H

#include <QtWidgets/QLabel>
#include <QtWidgets/QLayout>

#include "EquipmentCard.h"


class EquipmentCardPane : public QLabel
{
    Q_OBJECT

    public:
    // Concept for sizes of UI elements:
    //
    //  ----------   ----------
    // |          | |          | Equipment text
    // |          | | equipment|
    // | equipment| |  image   |
    // |   back   | |          |
    // |  image   |  ----------
    // |          |
    // |          |
    //  ----------
    //
    // The width of the equipment card back image is derived from the field size.
    // The other widths of the dialog elements are derived from the equipment card back image width.
    // Heights of the images are defined through width and ratio.
    static const float EQUIPMENT_CARD_BACK_WIDTH_FIELD_SIZE_FACTOR; //!< _equipment_card_back_size.x := (field size) * EQUIPMENT_CARD_BACK_WIDTH_FIELD_SIZE_FACTOR
    static const float EQUIPMENT_IMAGE_WIDTH_FACTOR; //!< _equipment_image_size.x := _equipment_card_back_size.x * EQUIPMENT_IMAGE_WIDTH_FACTOR
    static const float TEXT_WIDTH_FACTOR; //!< _text_width := _equipment_card_back_size.x * TEXT_WIDTH_FACTOR

    //! adjusts width and height according to equipment card text; positions the dialog in the middle of the screen
    EquipmentCardPane(QWidget* parent, const EquipmentCard& equipment_card);

    virtual ~EquipmentCardPane();

    virtual QSize sizeHint() const;

    virtual void setContentsMargins(int left, int top, int right, int bottom);

    private:
    QBoxLayout* _outer_layout;
};

#endif
