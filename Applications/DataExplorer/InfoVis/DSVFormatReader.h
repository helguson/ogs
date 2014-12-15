#ifndef DSVFORMATREADER_H
#define DSVFORMATREADER_H

#include <QString>
#include <QStringList>
#include <QVariant>
#include <QRegExp>

#include <QTextStream>

/**
 * @brief class to read files in delimiter-seperated-values format
 */
class DSVFormatReader
{
public:
	DSVFormatReader();
	~DSVFormatReader();
	
	static QVariantMap processFile(QString const & path, QRegExp const & delimiter, QRegExp const & attributeStructure);
private:
	static QVariantMap processFileData(QTextStream  & filestream, QRegExp const & delimiter, QRegExp const & attributeStructure);
	static QStringList processMetaData(QTextStream & filestream, QRegExp const & delimiter, QRegExp const & attributeStructure);
	static QVariantMap processData(QTextStream & filestream, QStringList const & header, QRegExp const & delimiter, QRegExp const & attributeStructure);
	static QVariantMap processLine(QString const & lineData, QStringList const & header, QRegExp const & delimiter, QRegExp const & attributeStructure);
	static QStringList structure(QString const & lineData, QRegExp const & delimiter);
	static bool isErroneous(QString const & dataElement, int index, QRegExp const & attributeStructure);
	static bool hasValidStructure(QString const & dataElement, int index, QRegExp const & attributeStructure);
	static QVariant assignType(QString const & dataElement, int index, QRegExp const & attributeStructure);
	static QVariant handleErroneous(QString const & dataElement, int index);
};

#endif // DSVFORMATREADER_H
