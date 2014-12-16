#include "DSVFormatReader.h"

#include <iostream>
#include <QFile>
#include <QTime>
#include <QDate>
#include <QDateTime>

//#################
//### constants ###
//#################
const QString DSVFormatReader::NUMBER = QString("NUMBER");
const QString DSVFormatReader::TEXT = QString("TEXT");
const QString DSVFormatReader::TIME = QString("TIME");

//######################################
//### constructors and deconstructor ###
//######################################
// TODO: clean parameters
DSVFormatReader::DSVFormatReader(QRegExp const & delimiter, QVariantList const & attributeStructures)
:
	attributeStructures(attributeStructures),
	delimiter(delimiter),
	headerElementStructure(QRegExp("\"(\\S*)( \\(([\\S ]+)\\))?\"")),
	names(),
	types(getTypes()),
	units()
{}

DSVFormatReader::DSVFormatReader()
	:DSVFormatReader(QRegExp(","), getValidStructures())
{}

DSVFormatReader::~DSVFormatReader(){
}

//###############
//### methods ###
//###############
// assert delimiter-seperated-values-format
QVariantMap DSVFormatReader::processFile(QString const & path){
	
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
	QVariantMap processedData = this->processFileData(filestream);
	
	file.close();
	
	return processedData;
}

QVariantMap DSVFormatReader::processFileData(QTextStream & filestream){
	
	this->processHead(filestream);
	
	return this->processBody(filestream);
}

// assert use before 'processBody', due to structure of dsv-format (header data in first line), so that filestream contains no meta data but data afterwards
void DSVFormatReader::processHead(QTextStream & filestream){
	
	// clear previous meta data
	this->names.clear();
	this->units.clear();
	
	QString headRow = filestream.readLine();
	QStringList headElements = headRow.split(this->delimiter);
	
	for(QString headElement: headElements){
		
		// TODO: extract to 'extractMetaData'
		
		QString name("");
		QString unit("");
		
		// exactMatch is nessesary in order to initialize 'cap()'
		if(this->headerElementStructure.exactMatch(headElement)){
			
			// TODO: make dynamic
			name = this->headerElementStructure.cap(1);
			unit = this->headerElementStructure.cap(2);
		}
		
		this->names.append(name);
		this->units.append(QVariant(unit));
	}
	
		
}

// assert filestream contains no meta data but data
// in order to transfer file data at once to JavaScript it will be stored recordwise in a QVariantMap with String indices
QVariantMap DSVFormatReader::processBody(QTextStream & filestream){
	
	QVariantMap data = QVariantMap();
	int nextIndex = 0;
	
	while(!filestream.atEnd()){
		
		QString bodyRow = filestream.readLine();
		QVariantMap dataRow = this->processBodyRow(bodyRow);
		
		data.insert(
			QString::number(nextIndex),
			QVariant(dataRow)
		);
		
		nextIndex++;
	}
	
	return data;
}

// assert |headElements| == |bodyElements/perRow|
// TODO: separate data and meta data
QVariantMap DSVFormatReader::processBodyRow(QString const & bodyRow){
	
	QVariantMap dataRow = QVariantMap();
	
	// 1.
	QStringList bodyRowElements = this->structure(bodyRow);
	
	// 2.
	for(int index = 0; index < bodyRowElements.length(); index++){
		
		QString bodyElement = bodyRowElements.at(index);
		QString name = this->names.at(index);
		QVariant dataElement;
		
		// 2.1.
		if(this->confirmsValidityOf(bodyElement, index)){
			
			// 2.1.a
			dataElement = this->assignTypeTo(bodyElement, index);
		}
		else{
			// 2.1.b
			dataElement = handleErroneous(bodyElement, index);
		}
		
		// 2.2
		dataRow.insert(name, dataElement);
	}
	
	return dataRow;
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
	
	return this->attributeStructures.at(columnIndex).toRegExp().exactMatch(bodyElement);
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

// TODO: implement handling
QVariant DSVFormatReader::handleErroneous(QString const & bodyElement, int index){
	
	return QVariant(QString("ERRONEOUS:") + bodyElement);
}

//########################
//### static functions ###
//########################
QVariantMap DSVFormatReader::processFile(QString const & path, QRegExp const & delimiter, QVariantList const & attributeStructures){
	
	DSVFormatReader reader(delimiter, attributeStructures);
	
	return reader.processFile(path);
}

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
 * @brief returns types for attributes of "Cosmic-Ray-Station Seelhausener See _Cosmic Ray Station 2_.csv"
 * @return a list of QVariant objects that are QString descriptions of the type for the corresponding date
 */
QVariantList DSVFormatReader::getTypes(){
	
	QVariantList resultList = QVariantList();
	
	resultList.append(QVariant(TEXT));	// "Logger"
	resultList.append(QVariant(TIME));	// "TimestampCaptured"
	resultList.append(QVariant(TIME));	// "TimestampConverted"
	resultList.append(QVariant(TIME));	// "TimestampMeasurement"
	resultList.append(QVariant(NUMBER));	// "LEVEL0RECORD"
	resultList.append(QVariant(NUMBER));	// "DevicesVoltagelogger0mSingle_value15Min (V)"
	resultList.append(QVariant(NUMBER));	// "DevicesTemperaturedevice1mSingle_value15Min (°C)"
	resultList.append(QVariant(NUMBER));	// "WeatherAirtemperature1mSingle_value15Min (°C)"
	resultList.append(QVariant(NUMBER));	// "DevicesTemperaturedatalogger0mSingle_value15Min (°C)"
	resultList.append(QVariant(NUMBER));	// "DevicesRelativehumiditydevice0mSingle_value15Min (%)"
	resultList.append(QVariant(NUMBER));	// "WeatherRelativehumidity1mSingle_value15Min (%)"
	resultList.append(QVariant(NUMBER));	// "DevicesRelativehumiditydevice1mSingle_value15Min (%)"
	resultList.append(QVariant(NUMBER));	// "DevicesImpulses/counts1mSingle_value15Min (mm)"
	resultList.append(QVariant(NUMBER));	// "PositionGpsposition:latitude0mSingle_value15Min (°)"
	resultList.append(QVariant(NUMBER));	// "PositionCourse/directionoftravel0mSingle_value15Min (°)"
	resultList.append(QVariant(NUMBER));	// "PositionGpsposition:longitude0mSingle_value15Min (°)"
	resultList.append(QVariant(NUMBER));	// "WeatherAirpressure0mSingle_value15Min (hPa)"
	resultList.append(QVariant(NUMBER));	// "WeatherAirpressure0mAvg15Min (hPa)"
	resultList.append(QVariant(NUMBER));	// "SoilNeutroncounts1mSingle_value15Min (1/t)"
	resultList.append(QVariant(NUMBER));	// "PositionGpsposition:height0mSingle_value15Min (m NHN)"
	resultList.append(QVariant(NUMBER));	// "PositionSpeed0mSingle_value15Min (km/h)"
	resultList.append(QVariant(NUMBER));	// "DevicesQualityflag0mSingle_value15Min"
	resultList.append(QVariant(TEXT));	// "TimeHk-time0mSingle_value15Min"
	resultList.append(QVariant(NUMBER));	// "NumberingValue/number0mSingle_value15Min"
	
	return resultList;
}

QVariantList DSVFormatReader::getValidStructures(){
	
	QVariantList resultList = QVariantList();
	
	resultList.append(QVariant(QRegExp("\"(.*)\"")));		// "Logger"
	resultList.append(QVariant(QRegExp("\"(\\d{4})-(\\d{2})-(\\d{2}) (\\d{2}):(\\d{2}):(\\d{2})\"")));	// "TimestampCaptured"
	resultList.append(QVariant(QRegExp("\"(\\d{4})-(\\d{2})-(\\d{2}) (\\d{2}):(\\d{2}):(\\d{2})\"")));	// "TimestampConverted"
	resultList.append(QVariant(QRegExp("\"(\\d{4})-(\\d{2})-(\\d{2}) (\\d{2}):(\\d{2}):(\\d{2})\"")));	// "TimestampMeasurement"
	resultList.append(QVariant(QRegExp("\"(\\d+(\\.\\d+)?)\"")));	// "LEVEL0RECORD"
	resultList.append(QVariant(QRegExp("\"(\\d+(\\.\\d+)?)\"")));	// "DevicesVoltagelogger0mSingle_value15Min (V)"
	resultList.append(QVariant(QRegExp("\"(\\d+(\\.\\d+)?)\"")));	// "DevicesTemperaturedevice1mSingle_value15Min (°C)"
	resultList.append(QVariant(QRegExp("\"(\\d+(\\.\\d+)?)\"")));	// "WeatherAirtemperature1mSingle_value15Min (°C)"
	resultList.append(QVariant(QRegExp("\"(\\d+(\\.\\d+)?)\"")));	// "DevicesTemperaturedatalogger0mSingle_value15Min (°C)"
	resultList.append(QVariant(QRegExp("\"(\\d+(\\.\\d+)?)\"")));	// "DevicesRelativehumiditydevice0mSingle_value15Min (%)"
	resultList.append(QVariant(QRegExp("\"(\\d+(\\.\\d+)?)\"")));	// "WeatherRelativehumidity1mSingle_value15Min (%)"
	resultList.append(QVariant(QRegExp("\"(\\d+(\\.\\d+)?)\"")));	// "DevicesRelativehumiditydevice1mSingle_value15Min (%)"
	resultList.append(QVariant(QRegExp("\"(\\d+(\\.\\d+)?)\"")));	// "DevicesImpulses/counts1mSingle_value15Min (mm)"
	resultList.append(QVariant(QRegExp("\"(\\d+(\\.\\d+)?)\"")));	// "PositionGpsposition:latitude0mSingle_value15Min (°)"
	resultList.append(QVariant(QRegExp("\"(\\d+(\\.\\d+)?)\"")));	// "PositionCourse/directionoftravel0mSingle_value15Min (°)"
	resultList.append(QVariant(QRegExp("\"(\\d+(\\.\\d+)?)\"")));	// "PositionGpsposition:longitude0mSingle_value15Min (°)"
	resultList.append(QVariant(QRegExp("\"(\\d+(\\.\\d+)?)\"")));	// "WeatherAirpressure0mSingle_value15Min (hPa)"
	resultList.append(QVariant(QRegExp("\"(\\d+(\\.\\d+)?)\"")));	// "WeatherAirpressure0mAvg15Min (hPa)"
	resultList.append(QVariant(QRegExp("\"(\\d+(\\.\\d+)?)\"")));	// "SoilNeutroncounts1mSingle_value15Min (1/t)"
	resultList.append(QVariant(QRegExp("\"(\\d+(\\.\\d+)?)\"")));	// "PositionGpsposition:height0mSingle_value15Min (m NHN)"
	resultList.append(QVariant(QRegExp("\"(\\d+(\\.\\d+)?)\"")));	// "PositionSpeed0mSingle_value15Min (km/h)"
	resultList.append(QVariant(QRegExp("\"(\\d+(\\.\\d+)?)\"")));	// "DevicesQualityflag0mSingle_value15Min"
	resultList.append(QVariant(QRegExp("\"(.*)\"")));		// "TimeHk-time0mSingle_value15Min"
	resultList.append(QVariant(QRegExp("\"(\\d+(\\.\\d+)?)\"")));	// "NumberingValue/number0mSingle_value15Min"
	
	return resultList;
}
