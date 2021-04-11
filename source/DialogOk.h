#ifndef DIALOGOK_H
#define DIALOGOK_H

#include <QtWidgets/QDialog>

class DialogOk : public QDialog
{
    Q_OBJECT

    public:
    DialogOk(const QString& title, const QString& text);
    virtual ~DialogOk();
};

#endif
