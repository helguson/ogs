#ifndef DSVFORMATREADER_H
#define DSVFORMATREADER_H

#include <QString>
#include <QStringList>
#include <QVariant>
#include <QRegExp>

#include <QTextStream>

/**
 * @brief class to read files in delimiter-seperated-values format
 * @author Thomas Hennig
 */
class DSVFormatReader
{
public:
	DSVFormatReader(QRegExp const & delimiter, QVariantList const & attributeStructures);
	DSVFormatReader();
	~DSVFormatReader();
	
	QVariantMap processFile(QString const & path);
	
	static QVariantMap processFile(QString const & path, QRegExp const & delimiter, QVariantList const & attributeStructures);
	static QVariantList getTypes();
	static QVariantList getValidStructures();
	
	static const QString TEXT;
	static const QString NUMBER;
	static const QString TIME;
private:
	// meta data
	QVariantList attributeStructures;
	QRegExp delimiter;
	QRegExp headerElementStructure;
	QStringList names;
	QVariantList types;
	QVariantList units;
	
	// data
	
	
	QVariantMap processFileData(QTextStream  & filestream);
	void processMetaData(QTextStream & filestream);
	QVariantMap processData(QTextStream & filestream);
	QVariantMap processDataRow(QString const & dataRow);
	QStringList structure(QString const & dataRow);
	bool confirmsValidityOf(QString const & dataElement, int index);
	bool hasValidStructure(QString const & dataElement, int index);
	QVariant assignTypeTo(QString const & dataElement, int index);
	QVariant handleErroneous(QString const & dataElement, int index);
	
	static QVariant parseNumber(QString const & dataElement, QRegExp const & structure);
	static QVariant parseText(QString const & dataElement, QRegExp const & structure);
	static QVariant parseTime(QString const & dataElement, QRegExp const & structure);
};

#endif // DSVFORMATREADER_H
