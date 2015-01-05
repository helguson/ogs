#ifndef METADATA_H
#define METADATA_H

#include <QObject>
#include <QList>
#include <QVariant>

class MetaData : public QObject
{
	Q_OBJECT
public:
	explicit MetaData(QObject *parent = 0);
	virtual ~MetaData();
	
	// from JavaScript accessible methods
	Q_INVOKABLE QVariantMap	getForDataElementScope(int rowIndex, int columnIndex) const;
	Q_INVOKABLE QVariant	getForDataElementScope(int rowIndex, int columnIndex, QString const & metaDataIdentifier) const;
	Q_INVOKABLE QVariantMap	getForDataRowScope(int rowIndex) const;
	Q_INVOKABLE QVariant	getForDataRowScope(int rowIndex, QString const & metaDataIdentifier) const;
	Q_INVOKABLE QVariantMap	getForDataColumnScope(int columnIndex) const;
	Q_INVOKABLE QVariant	getForDataColumnScope(int columnIndex, QString const & metaDataIdentifier) const;
	Q_INVOKABLE QVariantMap	getForDataScope() const;
	Q_INVOKABLE QVariant	getForDataScope(QString const & metaDataIdentifier) const;
	
	void setForDataElementScope(int rowIndex, int columnIndex, QString const & metaDataIdentifier, QVariant const & metaDate);
	void setForDataRowScope(int rowIndex, QString const & metaDataIdentifier, QVariant const & metaDate);
	void setForDataColumnScope(int columnIndex, QString const & metaDataIdentifier, QVariant const & metaDate);
	void setForDataScope(QString const & metaDataIdentifier, QVariant const & metaDate);
	
private:
	QVariantMap			dataScope;
	QMap<QString, QVariantMap>	dataRowScope;
	QMap<QString, QVariantMap>	dataColumnScope;
	QMap<QString, QVariantMap>	dataElementScope;
	
	QVariantMap accessDataScope() const;
	QVariantMap accessDataRowScope(int rowIndex) const;
	QVariantMap accessDataColumnScope(int columnIndex) const;
	QVariantMap accessDataElementScope(int rowIndex, int columnIndex) const;
	
	static QString determineKeyFromRow(int rowIndex);
	static QString determineKeyFromColumn(int columnIndex);
	static QString determineKeyForElement(int rowIndex, int columnIndex);
};

#endif // METADATA_H
