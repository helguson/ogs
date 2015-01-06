#ifndef DSVFORMATREADER_H
#define DSVFORMATREADER_H

#include <QString>
#include <QStringList>
#include <QVariant>
#include <QRegExp>

#include <QTextStream>
#include <memory>

#include "Data.h"

/**
 * @brief class to read files in delimiter-seperated-values format
 * @author Thomas Hennig
 * 
 * 
 * nomenclature of structure elements:
 * 1 dsv document domain
 *	- headRow
 *		* (usually first) row that contains headElements
 *	- headElement
 *		* string that represents a certain set of meta data, that
 *			describes the dateElements of the bodyColumn below this
 *			element
 *	- body
 *		* set of bodyRows (or theoretical: set of bodyColums)
 *	- bodyRow
 *		* row that contains bodyElements
 *		* represents a data set / record
 *	- bodyColumn
 *		* column of bodyElements
 *		* represents data with set of common assigned meta data (see
 *			headElement)
 *	- bodyElement
 *		* string that represents a date
 * 2 data domain
 *	- meta data
 *		* set of additional information about the data
 *	- metaDataColumnSet
 *		* set of metaDataElements that are represented by a headElement
 *	- metaDataElement
 *		* date (like type, structure, unit, identifier, ...) about the
 *			dataElements of a dataColumn (or theoretical about the
 *			data itself)
 *	- data
 *		* set of information
 *		* set of dataRows (or theoretical: set of dataColums)
 *	- dataRow
 *		* set of dataElements that are represented by a bodyRow
 *	- dataColumn
 *		* set of dataElements that are representes by a bodyColumn
 *	- dataElement
 *		* date that is represented by a bodyElement
 * 
 * Note that this nomenclature is actually more the description of a goal than
 * the description of the current state.
 * 
 * assertions:
 * - each bodyElement represents exactly one date
 */

// TODO: mark elements of the nomenclature that actually appear in the code
// TODO: remove the 'Note ...' paragraph if it is no longer necessary
class DSVFormatReader
{
public:
	DSVFormatReader(QRegExp const & delimiter, QVariantList const & attributeStructures);
	DSVFormatReader();
	~DSVFormatReader();
	
	void processFile(QString const & path);
	std::unique_ptr<QVariantList> getValues();
	std::unique_ptr<QVariantList> getMetaDataRelation();
	
	static QVariantList getTypes();
	static QVariantList getValidStructures();
	
	static const QString TEXT;
	static const QString NUMBER;
	static const QString TIME;
private:
	// tmp meta data
	// TODO: think about its structure (QVariantMap? / getSetOfMetaDataFor<Row/Column/Element/all>()?)
	QVariantList attributeStructures;
	QRegExp delimiter;
	QRegExp headerElementStructure;	//TODO: meta meta data?
	QList<int> names;
	QVariantList types;
	QList<int> units;
	
	// data
	DataBuilder dataBuilder;
	
	// TODO: const correctness
	
	void setUpProcessing();
	void processFileData(QTextStream  & filestream);
	void processHead(QTextStream & filestream);
	void processName(QString name);
	void processUnit(QString unit);
	void processBody(QTextStream & filestream);
	void processBodyRow(QString const & bodyRow);
	QStringList structure(QString const & bodyRow);
	bool confirmsValidityOf(QString const & bodyElement, int columnIndex);
	bool hasValidStructure(QString const & bodyElement, int columnIndex);
	QVariant assignTypeTo(QString const & bodyElement, int columnIndex);
	QVariant handleInvalid(QString const & bodyElement, int columnIndex);
	
	static QVariant parseNumber(QString const & bodyElement, QRegExp const & structure);
	static QVariant parseText(QString const & bodyElement, QRegExp const & structure);
	static QVariant parseTime(QString const & bodyElement, QRegExp const & structure);
};

#endif // DSVFORMATREADER_H
