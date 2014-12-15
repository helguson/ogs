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
	DSVFormatReader(QRegExp const & delimiter, QRegExp const & attributeStructure);
	DSVFormatReader();
	~DSVFormatReader();
	
	QVariantMap processFile(QString const & path);
	static QVariantMap processFile(QString const & path, QRegExp const & delimiter, QRegExp const & attributeStructure);
private:
	// meta data
	QRegExp attributeStructure;
	QRegExp delimiter;
	QStringList header;
	
	// data
	
	
	QVariantMap processFileData(QTextStream  & filestream);
	void processMetaData(QTextStream & filestream);
	QVariantMap processData(QTextStream & filestream);
	QVariantMap processLine(QString const & lineData);
	QStringList structure(QString const & lineData);
	bool isErroneous(QString const & dataElement, int index);
	bool hasValidStructure(QString const & dataElement, int index);
	QVariant assignType(QString const & dataElement, int index);
	QVariant handleErroneous(QString const & dataElement, int index);
	
};

#endif // DSVFORMATREADER_H
