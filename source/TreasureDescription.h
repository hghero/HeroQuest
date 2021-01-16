#ifndef TREASURE_CARD_DESCRIPTION_H
#define TREASURE_CARD_DESCRIPTION_H

#include <list>
#include <QtCore/QString>

#include "Playground.h"

/*!
 * Interprets and stores the treasure card descriptions given by .txt treasure card files.
 */
class TreasureDescription
{
public:
	static const QString TREASURE_CARD_DIR;

	TreasureDescription();
	TreasureDescription(const QString& filename);
	virtual ~TreasureDescription();

	void setID(const QString& id);
	const QString& getID() const;

	void setAmount(unsigned int amount);
	unsigned int getAmount() const;

	void setTreasureImageID(const Playground::TreasureImageID& treasure_image_id);
	const Playground::TreasureImageID& getTreasureImageID() const;

	void setText(const QString& text);
	const QString& getText() const;

	void clearActions();
	void addActionString(const QString& action_str);
	const std::list<QString>& getActions() const;

	bool readFromFile(const QString& filename);

	static bool buildDescriptions(std::set<TreasureDescription>* treasure_card_descriptions);

	bool operator<(const TreasureDescription& other) const;

	bool containsImmediateActions() const;
	bool getImmediateActions(std::vector<QString>* items) const;

	bool containsInventoryItem() const;
	bool getInventoryItems(std::vector<QString>* items) const;

    virtual bool save(std::ostream& stream) const;
    virtual bool load(std::istream& stream);

    void updateTreasureImage();

private:
	static Playground::TreasureImageID getTreasureImageID(const QString& str);

	bool getInventoryItemsInternal(std::vector<QString>* items) const;
	bool getImmediateActionsInternal(std::vector<QString>* immediate_actions) const;

	QString _id; //!< type of treasure card; given by .txt file name
	QPixmap* _treasure_image; // stored in Playground
	unsigned int _amount; //!< number of treasure cards of this kind
	Playground::TreasureImageID _image_id;
	QString _text;
	std::list<QString> _actions;
};

#endif
