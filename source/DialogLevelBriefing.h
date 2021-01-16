#ifndef DIALOG_LEVEL_BRIEFING_H
#define DIALOG_LEVEL_BRIEFING_H

#include <QtWidgets/QDialog>

#include "GameState.h"

class DialogLevelBriefing : public QDialog
{
    Q_OBJECT

    public:
    DialogLevelBriefing(GameState::LevelID level_id);
    virtual ~DialogLevelBriefing();

    private:
    static bool extractTitleAndDescription(const QString& xml_description, QString* title, QString* description);
    static QString extractTokenValueFromXmlDescription(const QString& xml_description, const QString& token);
};

#endif
