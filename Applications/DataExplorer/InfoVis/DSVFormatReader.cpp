#include "DSVFormatReader.h"

#include <iostream>
#include <QFile>
#include <QTime>
#include <QDate>
#include <QDateTime>

//#################
//### constants ###
//#################
const QString DSVFormatReader::NUMBER				= QString("number");
const QString DSVFormatReader::TEXT				= QString("string");
const QString DSVFormatReader::TIME				= QString("time");

const QString DSVFormatReader::COLUMN_ELEMENT_TYPE_IDENTIFIER	= QString("column_element_type");
const QString DSVFormatReader::COLUMN_ELEMENT_UNIT_IDENTIFIER	= QString("column_element_unit");
const QString DSVFormatReader::COLUMN_ELEMENT_NAME_IDENTIFIER	= QString("column_element_name");
const QString DSVFormatReader::COLUMN_IDENTIFIER		= QString("column");
const QString DSVFormatReader::ROW_IDENTIFIER			= QString("row");

//######################################
//### constructors and deconstructor ###
//######################################
// TODO: clean parameters
DSVFormatReader::DSVFormatReader(QRegExp const & delimiter, QVariantList const & attributeStructures)
:
	attributeStructures(attributeStructures),
	delimiter(delimiter),
	headerElementStructure(QRegExp("(\\S*)( \\(([\\S ]+)\\))?")),
	types(getTypes()),
	columnDateIndices(),
	dataBuilder()
{
}

DSVFormatReader::DSVFormatReader()
	:DSVFormatReader(QRegExp(","), getValidStructures())
{}

DSVFormatReader::~DSVFormatReader(){
}

//###############
//### methods ###
//###############
// assert delimiter-seperated-values-format
void DSVFormatReader::processFile(QString const & path){

	this->setUpProcessing();

	QFile file(path);


	// TODO: handle possible problems with files
	if(!file.exists()){
		std::cout << "file does not exist" << std::endl;
	}

	bool openedFileSucessfully = file.open(QIODevice::ReadOnly | QIODevice::Text);

	if(!openedFileSucessfully){

		// TODO: throw exception
		std::cout << "FILE PROBLEM" << std::endl;
	}

	QTextStream filestream(&file);

	this->processFileData(filestream);

	file.close();
}

void DSVFormatReader::setUpSinkDates(){

	this->columnElementTypeIdentifierDateIndex = this->dataBuilder.addDateAndReturnIndex(QVariant(DSVFormatReader::COLUMN_ELEMENT_TYPE_IDENTIFIER));
	this->columnElementUnitIdentifierDateIndex = this->dataBuilder.addDateAndReturnIndex(QVariant(DSVFormatReader::COLUMN_ELEMENT_UNIT_IDENTIFIER));
	this->columnElementNameIdentifierDateIndex = this->dataBuilder.addDateAndReturnIndex(QVariant(DSVFormatReader::COLUMN_ELEMENT_NAME_IDENTIFIER));
	this->columnIdendifierDateIndex = this->dataBuilder.addDateAndReturnIndex(QVariant(DSVFormatReader::COLUMN_IDENTIFIER));
	this->rowIdentifierDateIndex = this->dataBuilder.addDateAndReturnIndex(QVariant(DSVFormatReader::ROW_IDENTIFIER));
}

void DSVFormatReader::setUpProcessing(){

	// clear previous extracted meta data
	this->columnDateIndices.clear();

	// clear previous extracted data
	this->dataBuilder.reset();
	this->baseDataIndices.clear();

	this->setUpSinkDates();
}

void DSVFormatReader::processFileData(QTextStream & filestream){

	this->processHead(filestream);

	this->processBody(filestream);
}

void DSVFormatReader::processName(QString name, int columnDateIndex){

	int dateID = this->dataBuilder.addDateAndReturnIndex(
		QVariant(name)
	);

	this->dataBuilder.addAsMetaDataFor(dateID, this->columnElementNameIdentifierDateIndex);
	this->dataBuilder.addAsMetaDataFor(columnDateIndex, dateID);
}

void DSVFormatReader::processUnit(QString unit, int columnDateIndex){

	int dateID = this->dataBuilder.addDateAndReturnIndex(
		QVariant(unit)
	);

	this->dataBuilder.addAsMetaDataFor(dateID, this->columnElementUnitIdentifierDateIndex);
	this->dataBuilder.addAsMetaDataFor(columnDateIndex, dateID);
}


void DSVFormatReader::processType(QString type, int columnDateIndex){

	int dateID = this->dataBuilder.addDateAndReturnIndex(
		QVariant(type)
	);

	this->dataBuilder.addAsMetaDataFor(dateID, this->columnElementTypeIdentifierDateIndex);
	this->dataBuilder.addAsMetaDataFor(columnDateIndex, dateID);
}


int DSVFormatReader::setUpColumnDate(int columnIndex){

	int dateID = this->dataBuilder.addDateAndReturnIndex(QVariant(columnIndex));

	this->dataBuilder.addAsMetaDataFor(dateID, this->columnIdendifierDateIndex);

	this->columnDateIndices.push_back(dateID);

	return dateID;
}

// assert use before 'processBody', due to structure of dsv-format (header data in first line), so that filestream contains no meta data but data afterwards
void DSVFormatReader::processHead(QTextStream & filestream){

	QString headRow = filestream.readLine();
	QStringList headElements = headRow.split(this->delimiter);

	for(int columnIndex = 0; columnIndex < headElements.length(); columnIndex++){
		QString headElement = headElements.at(columnIndex);

		// TODO: extract to 'extractMetaData'
		QString name("");
		QString unit("");
		QString type = this->types.at(columnIndex).toString();

		// exactMatch is nessesary in order to initialize 'cap()'
		if(this->headerElementStructure.exactMatch(headElement)){

			// TODO: make dynamic
			name = this->headerElementStructure.cap(1);
			unit = this->headerElementStructure.cap(3);
		}

		int columnDateIndex = this->setUpColumnDate(columnIndex);
		this->processName(name, columnDateIndex);
		this->processUnit(unit, columnDateIndex);
		this->processType(type, columnDateIndex);
	}


}

// assert filestream contains no meta data but data
// in order to transfer file data at once to JavaScript it will be stored recordwise in a QVariantMap with String indices
void DSVFormatReader::processBody(QTextStream & filestream){

	// assert processHead processed nothing but row 0
	int rowIndex = 1;

	while(!filestream.atEnd()){

		QString bodyRow = filestream.readLine();
		this->processBodyRow(bodyRow, rowIndex);

		rowIndex++;
	}
}

int DSVFormatReader::setUpRowDate(int rowIndex){

	int dateID = this->dataBuilder.addDateAndReturnIndex(
		QVariant(rowIndex)
	);

	this->dataBuilder.addAsMetaDataFor(dateID, this->rowIdentifierDateIndex);

	return dateID;
}

// assert |headElements| >= |bodyElements/Row|
void DSVFormatReader::processBodyRow(QString const & bodyRow, int rowIndex){

	// reserve space for new row
	this->baseDataIndices.push_back(QList<int>());

	// 1.
	QStringList bodyRowElements = this->structure(bodyRow);
	int rowDateIndex = this->setUpRowDate(rowIndex);

	// 2.
	for(int columnIndex = 0; columnIndex < bodyRowElements.length(); columnIndex++){

		QString bodyElement = bodyRowElements.at(columnIndex);
		QVariant dataElement;

		// 2.1.
		if(this->confirmsValidityOf(bodyElement, columnIndex)){

			// 2.1.a
			dataElement = this->assignTypeTo(bodyElement, columnIndex);
		}
		else{
			// 2.1.b
			dataElement = handleInvalid(bodyElement, columnIndex);
		}

		// 2.2 store date and assign meta data
		int dateIndex = this->dataBuilder.addDateAndReturnIndex(dataElement);
		this->dataBuilder.addAsMetaDataFor(dateIndex, this->columnDateIndices[columnIndex]);
		this->dataBuilder.addAsMetaDataFor(dateIndex, rowDateIndex);

		this->baseDataIndices.last().push_back(dateIndex);
	}
}

QStringList DSVFormatReader::structure(QString const & bodyRow){

	return bodyRow.split(this->delimiter);
}

bool DSVFormatReader::confirmsValidityOf(QString const & bodyElement, int columnIndex){

	return this->hasValidStructure(bodyElement, columnIndex);
}

// test against given regular expression for structure
// assert there is only one valid regular expression
bool DSVFormatReader::hasValidStructure(QString const & bodyElement, int columnIndex){

	QRegExp r = this->attributeStructures.at(columnIndex).toRegExp();

	return r.exactMatch(bodyElement);

	//return this->attributeStructures.at(columnIndex).toRegExp().exactMatch(bodyElement);
}

QVariant DSVFormatReader::assignTypeTo(QString const & bodyElement, int columnIndex){

	QVariant dataElement = QVariant();

	QString type  = this->types.at(columnIndex).toString();
	QRegExp structure = this->attributeStructures.at(columnIndex).toRegExp();


	// TODO: think about type handling object
	if(type.compare(NUMBER) == 0){

		dataElement = parseNumber(bodyElement, structure);
	}
	else{
		if(type.compare(TEXT) == 0){

			dataElement = parseText(bodyElement, structure);
		}
		else{
			if(type.compare(TIME) == 0){

				dataElement = parseTime(bodyElement, structure);
			}
			else{
				// TODO: throw error, ... and handle it somewhere
			}
		}
	}

	return dataElement;
}

QVariant DSVFormatReader::handleInvalid(QString const & bodyElement, int columnIndex){

	// returns 'undefined' in JavaScript
	return QVariant();
}

std::unique_ptr<QVariantList> DSVFormatReader::getValues(){

	return this->dataBuilder.getValues();
}

std::unique_ptr<QVariantList> DSVFormatReader::getMetaDataRelation(){

	return this->dataBuilder.getMetaDataRelation();
}

std::unique_ptr<QVariantList> DSVFormatReader::getBaseDataIndices(){

	std::unique_ptr<QVariantList> result(new QVariantList());

	for(int rowIndex = 0; rowIndex < this->baseDataIndices.length(); rowIndex++){

		QVariantList rowDataIndices;

		for(int columnIndex = 0; columnIndex < this->baseDataIndices[rowIndex].length(); columnIndex++){

			int indexOfCurrentDate = this->baseDataIndices[rowIndex][columnIndex];

			rowDataIndices.push_back(
				QVariant(indexOfCurrentDate)
			);
		}

		(*result).push_back(
			QVariant(rowDataIndices)
		);
	}

	return result;
}

//########################
//### static functions ###
//########################
QVariant DSVFormatReader::parseNumber(QString const & bodyElement, QRegExp const & structure){

	structure.exactMatch(bodyElement);

	// TODO: make dynamic
	return QVariant(structure.cap(1).toDouble());
}

QVariant DSVFormatReader::parseText(QString const & bodyElement, QRegExp const & structure){

	structure.exactMatch(bodyElement);

	// TODO: make dynamic
	return QVariant(structure.cap(1));
}

QVariant DSVFormatReader::parseTime(QString const & bodyElement, QRegExp const & structure){

	structure.exactMatch(bodyElement);

	// TODO: make dynamic
	QDate date(
		structure.cap(1).toInt(),
		structure.cap(2).toInt(),
		structure.cap(3).toInt()
	);
	QTime time(
		structure.cap(4).toInt(),
		structure.cap(5).toInt(),
		structure.cap(6).toInt()
	);

	// TODO: timespec / timezone
	QDateTime dateTime(date, time);

	return QVariant(dateTime);
}

// TODO: make dynamic by i. e. reading from a meta data file
/**
 * @brief returns types for attributes of "Wasserguetemessstation Bode-Selke _Gross Germersleben _GGL__.csv"
 * @return a list of QVariant objects that are QString descriptions of the type for the corresponding date
 */
QVariantList DSVFormatReader::getTypes(){

	QVariantList resultList = QVariantList();

	resultList.append(QVariant(TEXT));	// Logger
	resultList.append(QVariant(TIME));	// TimestampCaptured
	resultList.append(QVariant(TIME));	// TimestampConverted
	resultList.append(QVariant(TIME));	// TimestampMeasurement
	resultList.append(QVariant(NUMBER));	// LEVEL0RECORD
	resultList.append(QVariant(NUMBER));	// SurfacewaterTurbidity_transmittedlightmeasurement0mSingle_value15Min ([-])
	resultList.append(QVariant(NUMBER));	// DevicesVoltagelogger0mSingle_value15Min (V)
	resultList.append(QVariant(NUMBER));	// DevicesTemperaturedatalogger0mSingle_value15Min (�C)
	resultList.append(QVariant(NUMBER));	// SurfacewaterWatertemperature0mSingle_value15Min (�C)
	resultList.append(QVariant(NUMBER));	// DevicesRelativehumiditydatalogger0mSingle_value15Min (%)
	resultList.append(QVariant(NUMBER));	// SurfacewaterSaturationoxygen0mSingle_value15MinLokal (%)
	resultList.append(QVariant(NUMBER));	// SurfacewaterConcentrationoxygen0mSingle_value15Min (mg/l)
	resultList.append(QVariant(NUMBER));	// SurfacewaterConcentrationno3n0mSingle_value15Min (mg/l)
	resultList.append(QVariant(NUMBER));	// SurfacewaterWaterlevel0mSingle_value15Min (cm)
	resultList.append(QVariant(NUMBER));	// SurfacewaterConductivityelectrical0mSingle_value15Min (�S/cm)
	resultList.append(QVariant(NUMBER));	// SurfacewaterConcentrationchlorophyll0mSingle_value15Min (�g/l)
	resultList.append(QVariant(NUMBER));	// SurfacewaterSpecificabsorptioncoefficientsac0mSingle_value15Min254_korr (1/m)
	resultList.append(QVariant(NUMBER));	// SurfacewaterPhvalue0mSingle_value15Min
	resultList.append(QVariant(NUMBER));	// NumberingNumbersofsamples0mSingle_value15Min
	resultList.append(QVariant(NUMBER));	// TimeHktime0mSingle_value15Min
	resultList.append(QVariant(TEXT));	// NumberingValuenumber0mSingle_value15Min
	resultList.append(QVariant(NUMBER));	// SurfacewaterTurbidity_scatteredlightmeasurement0mSingle_value15Min ([-])

	return resultList;
}

QVariantList DSVFormatReader::getValidStructures(){

	QVariantList resultList = QVariantList();

	resultList.append(QVariant(QRegExp("(.*)")));	// Logger
	resultList.append(QVariant(QRegExp("(\\d{4})-(\\d{2})-(\\d{2}) (\\d{2}):(\\d{2}):(\\d{2})")));	// TimestampCaptured
	resultList.append(QVariant(QRegExp("(\\d{4})-(\\d{2})-(\\d{2}) (\\d{2}):(\\d{2}):(\\d{2})")));	// TimestampConverted
	resultList.append(QVariant(QRegExp("(\\d{4})-(\\d{2})-(\\d{2}) (\\d{2}):(\\d{2}):(\\d{2})")));	// TimestampMeasurement
	resultList.append(QVariant(QRegExp("(-?\\d+(\\.\\d+)?)")));	// LEVEL0RECORD
	resultList.append(QVariant(QRegExp("(-?\\d+(\\.\\d+)?)")));	// SurfacewaterTurbidity_transmittedlightmeasurement0mSingle_value15Min ([-])
	resultList.append(QVariant(QRegExp("(-?\\d+(\\.\\d+)?)")));	// DevicesVoltagelogger0mSingle_value15Min (V)
	resultList.append(QVariant(QRegExp("(-?\\d+(\\.\\d+)?)")));	// DevicesTemperaturedatalogger0mSingle_value15Min (�C)
	resultList.append(QVariant(QRegExp("(-?\\d+(\\.\\d+)?)")));	// SurfacewaterWatertemperature0mSingle_value15Min (�C)
	resultList.append(QVariant(QRegExp("(-?\\d+(\\.\\d+)?)")));	// DevicesRelativehumiditydatalogger0mSingle_value15Min (%)
	resultList.append(QVariant(QRegExp("(-?\\d+(\\.\\d+)?)")));	// SurfacewaterSaturationoxygen0mSingle_value15MinLokal (%)
	resultList.append(QVariant(QRegExp("(-?\\d+(\\.\\d+)?)")));	// SurfacewaterConcentrationoxygen0mSingle_value15Min (mg/l)
	resultList.append(QVariant(QRegExp("(-?\\d+(\\.\\d+)?)")));	// SurfacewaterConcentrationno3n0mSingle_value15Min (mg/l)
	resultList.append(QVariant(QRegExp("(-?\\d+(\\.\\d+)?)")));	// SurfacewaterWaterlevel0mSingle_value15Min (cm)
	resultList.append(QVariant(QRegExp("(-?\\d+(\\.\\d+)?)")));	// SurfacewaterConductivityelectrical0mSingle_value15Min (�S/cm)
	resultList.append(QVariant(QRegExp("(-?\\d+(\\.\\d+)?)")));	// SurfacewaterConcentrationchlorophyll0mSingle_value15Min (�g/l)
	resultList.append(QVariant(QRegExp("(-?\\d+(\\.\\d+)?)")));	// SurfacewaterSpecificabsorptioncoefficientsac0mSingle_value15Min254_korr (1/m)
	resultList.append(QVariant(QRegExp("(-?\\d+(\\.\\d+)?)")));	// SurfacewaterPhvalue0mSingle_value15Min
	resultList.append(QVariant(QRegExp("(-?\\d+(\\.\\d+)?)")));	// NumberingNumbersofsamples0mSingle_value15Min
	resultList.append(QVariant(QRegExp("(-?\\d+(\\.\\d+)?)")));	// TimeHktime0mSingle_value15Min
	resultList.append(QVariant(QRegExp("(.*)")));	// NumberingValuenumber0mSingle_value15Min
	resultList.append(QVariant(QRegExp("(-?\\d+(\\.\\d+)?)")));	// SurfacewaterTurbidity_scatteredlightmeasurement0mSingle_value15Min ([-])

	return resultList;
}
