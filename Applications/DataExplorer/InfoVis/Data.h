#ifndef DATA_H
#define DATA_H

#include <QVariant>

#include <memory>

class DataBuilder{
	
public:
	DataBuilder();
	
	int addDateAndReturnIndex(QVariant date);
	void addAsMetaDataFor(int dateIndex, int metaDateIndex);
	
	/**
	 * @brief returns a list of all values
	 */
	std::unique_ptr<QVariantList> getValues();
	/**
	 * @brief returns a list that contains for every value V a
	 * list that contains indices of values that are meta data
	 * respective to V
	 */
	std::unique_ptr<QVariantList> getMetaDataRelation();
	
	/**
	 * @brief clears values and meta data relations
	 */
	void reset();
private:	
	QVariantList values;
	/// contains for every value V a list of indices of values that
	/// are meta data for V
	QList<QList<int>> metaDataRelation;
	
	static QVariantList toQVariantList(QList<int> list);
};


#endif // DATA_H
