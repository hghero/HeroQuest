#ifndef DIALOGOKCANCEL_H
#define DIALOGOKCANCEL_H

#include <QtWidgets/QDialog>

class DialogOkCancel : public QDialog
{
    Q_OBJECT

    public:
    DialogOkCancel(const QString& title, const QString& text);
    virtual ~DialogOkCancel();
};

#endif
