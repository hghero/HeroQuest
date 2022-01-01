#include "DialogLevelBriefing.h"

#include <memory>

#include <QtWidgets/QLabel>
#include <QtWidgets/QLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QScrollArea>
#include <QtGui/QGuiApplication>
#include <QtGui/QScreen>

#include "HeroQuestLevelWindow.h"
#include "Level.h"
#include "Debug.h"

using namespace std;

DialogLevelBriefing::DialogLevelBriefing(GameState::LevelID level_id)
        :
        QDialog((QDialog*) 0)
{
    int screen_width = 1920;
    if (!QGuiApplication::screens().empty())
    {
        QScreen* first_screen = *(QGuiApplication::screens().begin());
        screen_width = first_screen->availableGeometry().width();
    }

    // size of window: width 75 %
    setFixedWidth(screen_width * 3 / 4);

    unique_ptr<Level> tmp_level(HeroQuestLevelWindow::createLevelFromLevelID(level_id, 0));
    setWindowTitle("Briefing");
    setStyleSheet(HeroQuestLevelWindow::DIALOG_STYLE);

    QString title;
    QString description;
    extractTitleAndDescription(tmp_level->getLevelBriefing(), &title, &description);

    QBoxLayout* vertical_box_layout = new QBoxLayout(QBoxLayout::TopToBottom);
    vertical_box_layout->setAlignment(Qt::AlignHCenter);
    setLayout(vertical_box_layout);

    QLabel* text_label = new QLabel(title);
    text_label->setStyleSheet(HeroQuestLevelWindow::DIALOG_HEADER_BIG_STYLE);
    text_label->setMinimumHeight(100);
    text_label->setAlignment(Qt::AlignCenter);
    vertical_box_layout->addWidget(text_label);

    QLabel* space_label = new QLabel();
    space_label->setFixedHeight(10);
    vertical_box_layout->addWidget(space_label);

#if 0
    // test================================
    QLabel* text_label2 = new QLabel(description);
    text_label2->setStyleSheet(HeroQuestLevelWindow::DIALOG_TEXT_BIG_STYLE);
    text_label2->setAlignment(Qt::AlignCenter);
    text_label2->setWordWrap(true);
    text_label2->setMinimumHeight(100);

    QLabel* text_label2_wrapper = new QLabel;
    text_label2_wrapper->setFixedSize(800, 400);
    QScrollArea* scroll_area = new QScrollArea(text_label2_wrapper);
    scroll_area->setWidget(text_label2);
    scroll_area->setFixedSize(800, 400);

    vertical_box_layout->addWidget(text_label2_wrapper);
    // test================================
#else
    QLabel* text_label2 = new QLabel(description);
    text_label2->setStyleSheet(HeroQuestLevelWindow::DIALOG_TEXT_BIG_STYLE);
    text_label2->setAlignment(Qt::AlignCenter);
    text_label2->setWordWrap(true);
    vertical_box_layout->addWidget(text_label2);
#endif
    QLabel* space_label2 = new QLabel();
    space_label2->setFixedHeight(10);
    vertical_box_layout->addWidget(space_label2);

    QDialogButtonBox* button_box = new QDialogButtonBox(QDialogButtonBox::Ok);
    button_box->setStyleSheet(HeroQuestLevelWindow::DIALOG_STYLE);
    button_box->button(QDialogButtonBox::Ok)->setStyleSheet(HeroQuestLevelWindow::BUTTON_STYLE_ACTIVE);
    connect(button_box, SIGNAL(accepted()), this, SLOT(accept()));
    vertical_box_layout->addWidget(button_box);
}

DialogLevelBriefing::~DialogLevelBriefing()
{
	// NIX
}

/*!
 * Extracts title and description from given xml_description.
 *
 * Example:
 * "<title>The Probation</title><description>You have learned well, my friends.</description>"
 *
 * @param xml_description
 * @param title
 * @param description
 * @return True, if extraction successful, false otherwise.
 */
bool DialogLevelBriefing::extractTitleAndDescription(const QString& xml_description, QString* title,
        QString* description)
{
    *title = extractTokenValueFromXmlDescription(xml_description, "title");
    *description = extractTokenValueFromXmlDescription(xml_description, "description");
    return title->length() > 0 && description->length() > 0;
}

/*!
 * @param xml_description
 * @param token
 * @return The string in xml_description between token begin and token end. "" if the token was not found.
 */
QString DialogLevelBriefing::extractTokenValueFromXmlDescription(const QString& xml_description, const QString& token)
{
    QString token_begin = QString("<") + token + QString(">");
    QString token_end = QString("</") + token + QString(">");
    int index_token_begin = xml_description.indexOf(token_begin);
    int index_token_end = xml_description.indexOf(token_end);
    if (index_token_begin == -1)
    {
        DVX(
                ("ERROR: DialogLevelBriefing::extractTokenValueFromXmlDescription: begin token \"<%s>\" not found in \"%s\"!", qPrintable(token), qPrintable(xml_description)));
        return "";
    }
    if (index_token_end == -1)
    {
        DVX(
                ("ERROR: DialogLevelBriefing::extractTokenValueFromXmlDescription: end token \"</%s>\" not found in \"%s\"!", qPrintable(token), qPrintable(xml_description)));
        return "";
    }
    if (index_token_end < index_token_begin)
    {
        DVX(
                ("ERROR: DialogLevelBriefing::extractTokenValueFromXmlDescription: end token \"</%s>\" found before associated begin token in \"%s\"!", qPrintable(token), qPrintable(xml_description)));
        return "";
    }
    return xml_description.mid(index_token_begin + token_begin.length(),
            index_token_end - index_token_begin - token_begin.length());
}
