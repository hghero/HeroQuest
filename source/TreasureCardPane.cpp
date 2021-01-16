#include "TreasureCardPane.h"

#include <iostream>

#include <QtWidgets/QLabel>
#include <QtWidgets/QLayout>

#include "HeroQuestLevelWindow.h"
#include "QuestBoard.h"
#include "TreasureDescription.h"


using namespace std;

const float TreasureCardPane::TREASURE_CARD_BACK_WIDTH_FIELD_SIZE_FACTOR = 3;
const float TreasureCardPane::TREASURE_IMAGE_WIDTH_FACTOR = 3;
const float TreasureCardPane::TEXT_WIDTH_FACTOR = 4;


//! adjusts width and height according to treasure card text
TreasureCardPane::TreasureCardPane(QWidget* parent, const TreasureCard& treasure_card)
: QLabel(parent)
{
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
    _outer_layout = new QBoxLayout(QBoxLayout::LeftToRight);
    _outer_layout->setAlignment(Qt::AlignTop);
    setLayout(_outer_layout);

    const TreasureDescription& treasure_card_description = treasure_card.getTreasureDescription();

    // _treasure_card_back_image
    QPixmap* treasure_card_back_image = HeroQuestLevelWindow::_hero_quest->getPlayground()->getTreasureCardBackImage();
    if (treasure_card_back_image == 0)
        return;
    int treasure_card_back_image_scaled_width = HeroQuestLevelWindow::_hero_quest->getPlayground()->getQuestBoard()->getFieldWidth() * TREASURE_CARD_BACK_WIDTH_FIELD_SIZE_FACTOR;
    QLabel* back_image_label = new QLabel();
    back_image_label->setPixmap(treasure_card_back_image->scaledToWidth(treasure_card_back_image_scaled_width));
    back_image_label->setAlignment(Qt::AlignTop);
    _outer_layout->addWidget(back_image_label);

    // space
    QLabel* space1_label = new QLabel();
    space1_label->setFixedWidth(10);
    _outer_layout->addWidget(space1_label);

    // _treasure_image
    QPixmap* treasure_image = HeroQuestLevelWindow::_hero_quest->getPlayground()->getTreasureCardImage(treasure_card_description.getTreasureImageID());
    if (treasure_image == 0)
        return;
    QLabel* image_label = new QLabel();
    image_label->setPixmap(treasure_image->scaledToWidth(HeroQuestLevelWindow::_hero_quest->getPlayground()->getQuestBoard()->getFieldWidth() * TREASURE_IMAGE_WIDTH_FACTOR));
    image_label->setAlignment(Qt::AlignTop);
    _outer_layout->addWidget(image_label);

    // space
    QLabel* space2_label = new QLabel();
    space2_label->setFixedWidth(10);
    _outer_layout->addWidget(space2_label);

    // _text
    QLabel* text_label = new QLabel(treasure_card.getTreasureDescription().getText());
    text_label->setStyleSheet(HeroQuestLevelWindow::DIALOG_TEXT_STYLE);
    text_label->setAlignment(Qt::AlignTop);
    text_label->setFixedWidth(treasure_card_back_image_scaled_width * TEXT_WIDTH_FACTOR);
    text_label->setWordWrap(true);
    _outer_layout->addWidget(text_label);

    adjustSize();
}

TreasureCardPane::~TreasureCardPane()
{
	// NIX
}

QSize TreasureCardPane::sizeHint() const
{
    return _outer_layout->sizeHint();
}

void TreasureCardPane::setContentsMargins(int left, int top, int right, int bottom)
{
    QLabel::setContentsMargins(left, top, right, bottom);
    _outer_layout->setContentsMargins(left, top, right, bottom);
}
