#ifndef HERO_QUEST_NEW_GAME_DIALOG_H
#define HERO_QUEST_NEW_GAME_DIALOG_H

#include <vector>
#include <map>

#include <QtCore/QString>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtGui/QBackingStore>
#include <QtCore/QString>

#include "SpellCard.h"

class QPushButton;
class QCheckBox;
class QLabel;
class QComboBox;

class HeroQuestNewGameDialog : public QDialog
{
  Q_OBJECT

  public:
    enum DialogReturnValue {
    OK,
    QUIT
    };

    HeroQuestNewGameDialog(DialogReturnValue* dialog_return_value, std::vector<QString>* hero_names, SpellCard::SpellFamily* alb_spell_family);
    ~HeroQuestNewGameDialog();

  private slots:
    void okButtonClicked();
    void cancelButtonClicked();
    void updateButtons();

  private:
    void loadAvatars();

    QPushButton* getButton(const QString& str_id);

    DialogReturnValue* _dialog_return_value;
    std::vector<QString>* _hero_names;
    SpellCard::SpellFamily* _alb_spell_family;

    QCheckBox* _barbarian_check_box;
    QCheckBox* _dwarf_check_box;
    QCheckBox* _alb_check_box;
    QCheckBox* _magician_check_box;

    QPixmap* _barbarian_avatar;
    QPixmap* _dwarf_avatar;
    QPixmap* _alb_avatar;
    QPixmap* _magician_avatar;
    QPixmap* _inactive_avatar;

    QLabel* _barbarian_avatar_label;
    QLabel* _dwarf_avatar_label;
    QLabel* _alb_avatar_label;
    QComboBox* _alb_spell_pile_combo_box;
    QLabel* _magician_avatar_label;

    std::map<QString, uint> _button_ids; //!< button caption => array index
    std::vector<QPushButton*> _buttons; //!< order identical to _button_captions
};

#endif
