#ifndef SPELL_CARD_PANE_H
#define SPELL_CARD_PANE_H

#include <QtWidgets/QLabel>
#include <QtWidgets/QLayout>

#include "SpellCard.h"

class HeroQuestLevelWindow;

class SpellCardPane : public QLabel
{
    Q_OBJECT

    public:
    // Concept for sizes of UI elements:
    //
    //  ----------   ----------
    // |          | |          | Spell text
    // |          | |  spell   |
    // |  spell   | |  image   |
    // |   back   | |          |
    // |  image   |  ----------
    // |          |
    // |          |
    //  ----------
    //
    // The width of the spell card back image is derived from the playground field size.
    // The other widths of the dialog elements are derived from the spell card back image width.
    // Heights of the images are defined through width and ratio.
    static const float SPELL_CARD_BACK_WIDTH_FIELD_SIZE_FACTOR; //!< _spell_card_back_size.x := (playground field size) * SPELL_CARD_BACK_WIDTH_FIELD_SIZE_FACTOR
    static const float SPELL_IMAGE_WIDTH_FACTOR; //!< _spell_image_size.x := _spell_card_back_size.x * SPELL_IMAGE_WIDTH_FACTOR
    static const float TEXT_WIDTH_FACTOR; //!< _text_width := _spell_card_back_size.x * TEXT_WIDTH_FACTOR

    //! adjusts width and height according to treasure card text; positions the dialog in the middle of the screen
    SpellCardPane(QWidget* parent, const SpellCard& treasure_card);

    virtual ~SpellCardPane();

    virtual QSize sizeHint() const;

    virtual void setContentsMargins(int left, int top, int right, int bottom);

    private:
    QLayout* _outer_layout;
};

#endif
