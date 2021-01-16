#ifndef HINT_PANE_H
#define HINT_PANE_H

#include <vector>
#include <map>
#include <QtWidgets/QLabel>
#include "Level.h"

class HintPane : public QLabel
{
    Q_OBJECT

public:
    enum AbortBehaviour
    {
        ALLOW_ABORT = 0
    };
    
    HintPane();
    virtual ~HintPane();

    void reset();
    void setHint(const QString& hint, AbortBehaviour abort_behaviour);

private:
    QString _hint;
    QLabel* _hint_label;
    QLabel* _abort_label;
};

#endif
