#ifndef TREASURE_CARD_DESCRIPTION_H
#define TREASURE_CARD_DESCRIPTION_H

#include <list>
#include <vector>
#include <set>

#include <QtCore/Qt>
#include <QtCore/QString>

#include "TreasureDataTypes.h"

class QPixmap;
class SaveContext;
class LoadContext;

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

    void setTreasureImageID(const TreasureDataTypes::TreasureImageID& treasure_image_id);
    const TreasureDataTypes::TreasureImageID& getTreasureImageID() const;

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
    void executeImmediateActions() const;

	bool containsInventoryItem() const;
	bool getInventoryItems(std::vector<QString>* items) const;

    virtual bool save(SaveContext& save_context) const;
    virtual bool load(LoadContext& load_context);

    void updateTreasureImage();

    void disarmTraps();

private:
    static TreasureDataTypes::TreasureImageID getTreasureImageID(const QString& str);

	bool getInventoryItemsInternal(std::vector<QString>* items) const;
	bool getImmediateActionsInternal(std::vector<QString>* immediate_actions) const;

    static QString getEmbracedValue(const QString& action, Qt::SplitBehavior split_behaviour =
            Qt::SkipEmptyParts);
    static bool getValueOfGoldExpression(const QString& expr, uint* result_value);
    static bool interpretRoundExpression(const QString& expr, QString* command, uint* result_value);

	QString _id; //!< type of treasure card; given by .txt file name
	QPixmap* _treasure_image; // stored in Playground
	unsigned int _amount; //!< number of treasure cards of this kind
    TreasureDataTypes::TreasureImageID _image_id;
	QString _text;
	std::list<QString> _actions;
};

#endif
