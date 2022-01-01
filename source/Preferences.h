#ifndef PREFERENCES_H
#define PREFERENCES_H

#include <QtCore/QString.h>

class Preferences
{
    private:
    Preferences();

public:
    static const bool ENABLE_SAVE_LOAD_LOG;
    static const bool ENABLE_AUTOSAVE;
    static const QString AUTOSAVE_DIR;
};

#endif
