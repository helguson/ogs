#include "MetaData.h"

//######################################
//### constructors and deconstructor ###
//######################################
MetaData::MetaData(QObject *parent)
:
	QObject(parent),
	dataScope(),
	dataRowScope(),
	dataColumnScope(),
	dataElementScope()
{}

MetaData::~MetaData()
{}

//###############
//### methods ###
//###############
QVariantMap MetaData::getForDataScope() const{
	
	QVariantMap result = QVariantMap();
	
	result.unite(this->accessDataScope());
	
	return result;
}

QVariant MetaData::getForDataScope(QString const & metaDataIdentifier) const{
	
	return this->getForDataScope().value(metaDataIdentifier);
}

QVariantMap MetaData::getForDataColumnScope(int columnIndex) const{
	
	QVariantMap result = QVariantMap();
	
	result.unite(this->accessDataScope());
	result.unite(this->accessDataColumnScope(columnIndex));
	
	return result;
}

QVariant MetaData::getForDataColumnScope(int columnIndex, QString const & metaDataIdentifier) const{
	
	return this->getForDataColumnScope(columnIndex).value(metaDataIdentifier);
}

QVariantMap MetaData::getForDataRowScope(int rowIndex) const{
	
	QVariantMap result = QVariantMap();
	
	result.unite(this->accessDataScope());
	result.unite(this->accessDataRowScope(rowIndex));
	
	return result;
}

QVariant MetaData::getForDataRowScope(int rowIndex, QString const & metaDataIdentifier) const{
	
	return this->getForDataRowScope(rowIndex).value(metaDataIdentifier);
}

QVariantMap MetaData::getForDataElementScope(int rowIndex, int columnIndex) const{
	
	QVariantMap result = QVariantMap();
	
	result.unite(this->accessDataScope());
	result.unite(this->accessDataColumnScope(columnIndex));
	result.unite(this->accessDataRowScope(rowIndex));
	result.unite(this->accessDataElementScope(rowIndex, columnIndex));
	
	return result;
}

QVariant MetaData::getForDataElementScope(int rowIndex, int columnIndex, QString const & metaDataIdentifier) const{
	
	return this->getForDataElementScope(rowIndex, columnIndex).value(metaDataIdentifier);
}


void MetaData::setForDataElementScope(int rowIndex, int columnIndex, QString const & metaDataIdentifier, QVariant const & metaDate){
	
	//QVariantMap specificDataElementScope = this->dataElementScope.value(determineKeyForElement(rowIndex, columnIndex));
	//specificDataElementScope.insert(metaDataIdentifier, metaDate);
	
	this->dataElementScope[determineKeyForElement(rowIndex, columnIndex)]
			.insert(metaDataIdentifier, metaDate);
}

void MetaData::setForDataRowScope(int rowIndex, QString const & metaDataIdentifier, QVariant const & metaDate){
	
	this->dataRowScope[determineKeyFromRow(rowIndex)]
			.insert(metaDataIdentifier, metaDate);
}

void MetaData::setForDataColumnScope(int columnIndex, QString const & metaDataIdentifier, QVariant const & metaDate){
	
	this->dataColumnScope[determineKeyFromColumn(columnIndex)]
			.insert(metaDataIdentifier, metaDate);
}

void MetaData::setForDataScope(QString const & metaDataIdentifier, QVariant const & metaDate){
	
	this->dataScope.insert(
		metaDataIdentifier,
		metaDate
	);
}

QVariantMap MetaData::accessDataScope() const{
	
	return QVariantMap(this->dataScope);
}

QVariantMap MetaData::accessDataRowScope(int rowIndex) const{
	
	return QVariantMap(
		this->dataRowScope.value(
			determineKeyFromRow(rowIndex)	
		)	   
	);
}

QVariantMap MetaData::accessDataColumnScope(int columnIndex) const{
	
	return QVariantMap(
		this->dataColumnScope.value(
			determineKeyFromColumn(columnIndex)	
		)	   
	);
}

QVariantMap MetaData::accessDataElementScope(int rowIndex, int columnIndex) const{
	
	return QVariantMap(
		this->dataElementScope.value(
			determineKeyForElement(rowIndex, columnIndex)	
		)	   
	);
}

//######################
//### static methods ###
//######################
QString MetaData::determineKeyFromRow(int rowIndex){
	
	return QString::number(rowIndex);
}

QString MetaData::determineKeyFromColumn(int columnIndex){
	
	return QString::number(columnIndex);
}

QString MetaData::determineKeyForElement(int rowIndex, int columnIndex){
	
	return QString::number(rowIndex) + "," + QString::number(columnIndex);
}
