#ifndef TREASURE_DESCRIPTION_PANEL_H
#define TREASURE_DESCRIPTION_PANEL_H

#include <QtWidgets/QLabel>
#include <QtWidgets/QLayout>

class HeroQuestLevelWindow;
class TreasureDescription;

class TreasureDescriptionPane : public QLabel
{
    Q_OBJECT

    public:
    // Concept for sizes of UI elements:
    //
    //  ----------
    // |          | Treasure description text
    // |  chest   |
    // |  image   |
    // |          |
    //  ----------
    //
    // The width of the chest image is derived from the playground field size.
    // The other widths of the dialog elements are derived from the chest_3d image width.
    // Heights of the images are defined through width and ratio.
    static const float CHEST_IMAGE_WIDTH_FIELD_SIZE_FACTOR; //!< _chest_3d_size.x := (playground field size) * CHEST_IMAGE_WIDTH_FIELD_SIZE_FACTOR
    static const float TEXT_WIDTH_FACTOR; //!< _text_width := _chest_3d_size.x * TEXT_WIDTH_FACTOR

    //! adjusts width and height according to treasure card text; positions the dialog in the middle of the screen
    TreasureDescriptionPane(QWidget* parent, const TreasureDescription& treasure_description);

    virtual ~TreasureDescriptionPane();

    virtual QSize sizeHint() const;

    virtual void setContentsMargins(int left, int top, int right, int bottom);

    private:
    QLayout* _outer_layout;
};

#endif
