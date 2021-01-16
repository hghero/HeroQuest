#include "SpellCardPane.h"

#include <iostream>

#include <QtWidgets/QLabel>
#include <QtWidgets/QLayout>

#include "HeroQuestLevelWindow.h"
#include "QuestBoard.h"


using namespace std;

const float SpellCardPane::SPELL_CARD_BACK_WIDTH_FIELD_SIZE_FACTOR = 3;
const float SpellCardPane::SPELL_IMAGE_WIDTH_FACTOR = 3;
const float SpellCardPane::TEXT_WIDTH_FACTOR = 4;


//! adjusts width and height according to treasure card text
SpellCardPane::SpellCardPane(QWidget* parent, const SpellCard& spell_card)
: QLabel(parent)
{
    //
    //  ----------   ----------
    // |          | |          | spell title
    // |          | |  spell   |
    // |  spell   | |  image   | spell description
    // |   back   | |          |
    // |  image   |  ----------
    // |          |
    // |          |
    //  ----------
    //
    _outer_layout = new QBoxLayout(QBoxLayout::LeftToRight);
    _outer_layout->setAlignment(Qt::AlignTop);
    setLayout(_outer_layout);

    const QString& spell_card_title = spell_card.getTitle();
    const QString& spell_card_description = spell_card.getDescription();

    // _spell_card_back_image
    QPixmap* spell_card_back_image = spell_card.getBackImage();
    if (spell_card_back_image == 0)
        return;
    int spell_card_back_image_scaled_width = HeroQuestLevelWindow::_hero_quest->getPlayground()->getQuestBoard()->getFieldWidth() * SPELL_CARD_BACK_WIDTH_FIELD_SIZE_FACTOR;
    QLabel* back_image_label = new QLabel();
    back_image_label->setPixmap(spell_card_back_image->scaledToWidth(spell_card_back_image_scaled_width));
    back_image_label->setAlignment(Qt::AlignTop);
    _outer_layout->addWidget(back_image_label);

    // space
    QLabel* space1_label = new QLabel();
    space1_label->setFixedWidth(10);
    _outer_layout->addWidget(space1_label);

    // _spell_image
    QPixmap* spell_image = spell_card.getImage();
    if (spell_image == 0)
        return;
    QLabel* image_label = new QLabel();
    image_label->setPixmap(spell_image->scaledToWidth(HeroQuestLevelWindow::_hero_quest->getPlayground()->getQuestBoard()->getFieldWidth() * SPELL_IMAGE_WIDTH_FACTOR));
    image_label->setAlignment(Qt::AlignTop);
    _outer_layout->addWidget(image_label);

    // space
    QLabel* space2_label = new QLabel();
    space2_label->setFixedWidth(10);
    _outer_layout->addWidget(space2_label);

    // text
    // ----
    QLabel* text_label = new QLabel();
    QBoxLayout* text_layout = new QBoxLayout(QBoxLayout::TopToBottom);
    text_layout->setAlignment(Qt::AlignTop);
    text_layout->setContentsMargins(0, 0, 0, 0);
    text_layout->setSpacing(0);
    text_label->setLayout(text_layout);
    text_label->setFixedWidth(spell_card_back_image_scaled_width * TEXT_WIDTH_FACTOR);

    // text: title
    QLabel* title_label = new QLabel(spell_card_title);
    title_label->setStyleSheet(HeroQuestLevelWindow::DIALOG_HEADER_STYLE);
    title_label->setAlignment(Qt::AlignLeft);
    title_label->setFixedWidth(spell_card_back_image_scaled_width * TEXT_WIDTH_FACTOR);
    text_layout->addWidget(title_label);

    // text: space
    QLabel* space3_label = new QLabel();
    space3_label->setFixedHeight(10);
    text_layout->addWidget(space3_label);

    // text: description
    QLabel* description_label = new QLabel(spell_card_description);
    description_label->setStyleSheet(HeroQuestLevelWindow::DIALOG_TEXT_STYLE);
    description_label->setAlignment(Qt::AlignLeft);
    description_label->setFixedWidth(spell_card_back_image_scaled_width * TEXT_WIDTH_FACTOR);
    description_label->setWordWrap(true);
    text_layout->addWidget(description_label);

    _outer_layout->addWidget(text_label);
    // ----

    adjustSize();
}

SpellCardPane::~SpellCardPane()
{
	// NIX
}

QSize SpellCardPane::sizeHint() const
{
    return _outer_layout->sizeHint();
}

void SpellCardPane::setContentsMargins(int left, int top, int right, int bottom)
{
    QLabel::setContentsMargins(left, top, right, bottom);
    _outer_layout->setContentsMargins(left, top, right, bottom);
}
