#ifndef HERO_QUEST_START_DIALOG_H
#define HERO_QUEST_START_DIALOG_H

#include <vector>
#include <map>

#include <QtCore/QString>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtGui/QBackingStore>
#include <QtCore/QString>

class QPushButton;

class HeroQuestStartDialog : public QDialog
{
  Q_OBJECT

  public:
    enum DialogReturnValue {
    QUICK_GAME = 0,
    NEW_GAME,
    LOAD_GAME,
    QUIT
    };

    HeroQuestStartDialog(DialogReturnValue* dialog_return_value);
    ~HeroQuestStartDialog();

  private slots:
    void quickGameButtonClicked();
    void newGameButtonClicked();
    void loadGameButtonClicked();
    void quitButtonClicked();

  private:
    QPushButton* getButton(const QString& str_id);

    std::map<QString, uint> _button_ids; //!< button caption => array index
    std::vector<QPushButton*> _buttons; //!< order identical to _button_captions

    DialogReturnValue* _dialog_return_value;
};

#endif
