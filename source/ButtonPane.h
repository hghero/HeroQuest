#ifndef BUTTON_PANE_H
#define BUTTON_PANE_H

#include <vector>
#include <map>
#include <QtWidgets/QLabel>
#include "Level.h"

class QPushButton;

class ButtonPane : public QLabel
{
    Q_OBJECT

public:
	ButtonPane();
	virtual ~ButtonPane();

	void setAllInactive();
	void updateButtons(const Level::HeroActionStates& hero_action_states);
	QPushButton* getButton(const QString& str_id);

private:
	std::map<QString, uint> _button_ids; //!< button caption => array index
	std::vector<QPushButton*> _buttons; //!< order identical to _button_captions
};

#endif
