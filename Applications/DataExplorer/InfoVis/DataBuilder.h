#ifndef DATABUILDER_H
#define DATABUILDER_H

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
	std::unique_ptr<QVariantList> getValues() const;
	/**
	 * @brief returns a list that contains for every value V a
	 * list that contains indices of values that are meta data
	 * respective to V
	 */
	std::unique_ptr<QVariantList> getMetaDataRelation() const;
	
	/**
	 * @brief clears values and meta data relations
	 */
	void reset();
	
	static QVariantList toQVariantList(QList<int> list);
private:	
	QVariantList values;
	/// contains for every value V a list of indices of values that
	/// are meta data for V
	QList<QList<int>> metaDataRelation;
	

};


#endif // DATABUILDER_H
