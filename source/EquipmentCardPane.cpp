#include "EquipmentCardPane.h"

#include <iostream>

#include <QtWidgets/QLabel>
#include <QtWidgets/QLayout>

#include "HeroQuestLevelWindow.h"
#include "EquipmentCardStorage.h"
#include "ParameterStorage.h"


using namespace std;

const float EquipmentCardPane::EQUIPMENT_CARD_BACK_WIDTH_FIELD_SIZE_FACTOR = 3;
const float EquipmentCardPane::EQUIPMENT_IMAGE_WIDTH_FACTOR = 3;
const float EquipmentCardPane::TEXT_WIDTH_FACTOR = 4;


//! adjusts width and height according to treasure card text
EquipmentCardPane::EquipmentCardPane(QWidget* parent, const EquipmentCard& equipment_card)
: QLabel(parent)
{
    //
    //  ----------   ----------
    // |          | |          | Equipment title
    // |          | | equipment|
    // | equipment| |  image   | Equipment text
    // |   back   | |          |
    // |  image   |  ----------
    // |          |
    // |          |
    //  ----------
    //
    _outer_layout = new QBoxLayout(QBoxLayout::LeftToRight);
    _outer_layout->setAlignment(Qt::AlignTop);
    _outer_layout->setSpacing(10);
    setLayout(_outer_layout);

    // --- equipment_card_back_image ---
    QPixmap* equipment_card_back_image = EquipmentCardStorage::instance->getEquipmentCardBackImage();
    if (equipment_card_back_image == 0)
        return;
    int equipment_card_back_image_scaled_width = ParameterStorage::instance->getFieldSize()
            * EQUIPMENT_CARD_BACK_WIDTH_FIELD_SIZE_FACTOR;
    QLabel* back_image_label = new QLabel();
    back_image_label->setPixmap(equipment_card_back_image->scaledToWidth(equipment_card_back_image_scaled_width));
    back_image_label->setAlignment(Qt::AlignTop);
    _outer_layout->addWidget(back_image_label);

    // --- equipment_image ---
    QPixmap* equipment_image = EquipmentCardStorage::instance->getEquipmentCardImage(equipment_card.getEquipmentID());
    if (equipment_image == 0)
        return;
    QLabel* image_label = new QLabel();
    image_label->setPixmap(
            equipment_image->scaledToWidth(ParameterStorage::instance->getFieldSize() * EQUIPMENT_IMAGE_WIDTH_FACTOR));
    image_label->setAlignment(Qt::AlignTop);
    _outer_layout->addWidget(image_label);

    // --- text ---
    {
        QBoxLayout* text_layout = new QBoxLayout(QBoxLayout::TopToBottom);
        _outer_layout->addLayout(text_layout);
        text_layout->setAlignment(Qt::AlignTop);
        text_layout->setContentsMargins(0, 0, 0, 0);
        text_layout->setSpacing(10);

        // text: title
        QLabel* text_title = new QLabel(equipment_card.getTitle());
        text_title->setStyleSheet(HeroQuestLevelWindow::DIALOG_HEADER_STYLE);
        text_title->setAlignment(Qt::AlignLeft);
        text_title->setFixedWidth(equipment_card_back_image_scaled_width * TEXT_WIDTH_FACTOR);
        text_layout->addWidget(text_title);

        // text: description
        QLabel* text_description = new QLabel(equipment_card.getDescription());
        text_description->setStyleSheet(HeroQuestLevelWindow::DIALOG_TEXT_STYLE);
        text_description->setAlignment(Qt::AlignLeft);
        text_description->setFixedWidth(equipment_card_back_image_scaled_width * TEXT_WIDTH_FACTOR);
        text_description->setWordWrap(true);
        text_layout->addWidget(text_description);

        text_layout->addStretch();
    }

    adjustSize();
}

EquipmentCardPane::~EquipmentCardPane()
{
	// NIX
}

QSize EquipmentCardPane::sizeHint() const
{
    return _outer_layout->sizeHint();
}

void EquipmentCardPane::setContentsMargins(int left, int top, int right, int bottom)
{
    QLabel::setContentsMargins(left, top, right, bottom);
    _outer_layout->setContentsMargins(left, top, right, bottom);
}
