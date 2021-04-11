#include "TreasureDescriptionPane.h"

#include <iostream>

#include <QtWidgets/QLabel>
#include <QtWidgets/QLayout>

#include "HeroQuestLevelWindow.h"
#include "QuestBoard.h"
#include "TreasureDescription.h"
#include "TreasureCardStorage.h"
#include "Playground.h"
#include "ParameterStorage.h"


using namespace std;

const float TreasureDescriptionPane::CHEST_IMAGE_WIDTH_FIELD_SIZE_FACTOR = 3;
const float TreasureDescriptionPane::TEXT_WIDTH_FACTOR = 4;

//! adjusts width and height according to treasure card text
TreasureDescriptionPane::TreasureDescriptionPane(QWidget* parent, const TreasureDescription& treasure_description)
: QLabel(parent)
{
    //
    //  ----------
    // |          | Treasure description text
    // |  chest   |
    // |  image   |
    // |          |
    //  ----------
    //
    _outer_layout = new QBoxLayout(QBoxLayout::LeftToRight);
    _outer_layout->setAlignment(Qt::AlignTop);
    setLayout(_outer_layout);

    // chest_3d_image
    QPixmap* chest_3d_image = TreasureCardStorage::instance->getChest3DImage();
    if (chest_3d_image == 0)
        return;
    int chest_3d_image_scaled_width = ParameterStorage::instance->getFieldSize() * CHEST_IMAGE_WIDTH_FIELD_SIZE_FACTOR;
    QLabel* chest_3d_image_label = new QLabel();
    chest_3d_image_label->setPixmap(chest_3d_image->scaledToWidth(chest_3d_image_scaled_width));
    chest_3d_image_label->setAlignment(Qt::AlignTop);
    _outer_layout->addWidget(chest_3d_image_label);

    // space
    QLabel* space1_label = new QLabel();
    space1_label->setFixedWidth(10);
    _outer_layout->addWidget(space1_label);

    // _text
    QLabel* text_label = new QLabel(treasure_description.getText());
    text_label->setStyleSheet(HeroQuestLevelWindow::DIALOG_TEXT_STYLE);
    text_label->setAlignment(Qt::AlignTop);
    text_label->setFixedWidth(chest_3d_image_scaled_width * TEXT_WIDTH_FACTOR);
    text_label->setWordWrap(true);
    _outer_layout->addWidget(text_label);

    adjustSize();
}

TreasureDescriptionPane::~TreasureDescriptionPane()
{
	// NIX
}

QSize TreasureDescriptionPane::sizeHint() const
{
    return _outer_layout->sizeHint();
}

void TreasureDescriptionPane::setContentsMargins(int left, int top, int right, int bottom)
{
    QLabel::setContentsMargins(left, top, right, bottom);
    _outer_layout->setContentsMargins(left, top, right, bottom);
}
