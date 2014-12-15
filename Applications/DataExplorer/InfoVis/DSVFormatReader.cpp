#include "DSVFormatReader.h"

#include <iostream>
#include <QFile>

//######################################
//### constructors and deconstructor ###
//######################################
DSVFormatReader::DSVFormatReader(QRegExp const & delimiter, QRegExp const & attributeStructure)
:
	attributeStructure(attributeStructure),
	delimiter(delimiter),
	header()
{}

DSVFormatReader::DSVFormatReader()
	:DSVFormatReader(QRegExp(","), QRegExp("\"*\""))
{}

DSVFormatReader::~DSVFormatReader(){
}

//###############
//### methods ###
//###############
// assert delimiter-seperated-values-format
QVariantMap DSVFormatReader::processFile(QString const & path){
	
	QFile file(path);
	
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
	
	this->processMetaData(filestream);
	
	return this->processData(filestream);
}

// assert use before 'processData', due to structure of dsv-format (header data in first line), so that filestream contains no meta data but data afterwards
void DSVFormatReader::processMetaData(QTextStream & filestream){
	
	QString headerLine = filestream.readLine();
	this->header = headerLine
			     .split(this->delimiter)
			     .replaceInStrings(this->attributeStructure, "\\1");	// TODO: Where should that step be put and how should it be named? data extraction?
}

// assert filestream contains no meta data but data
// in order to transfer file data at once to JavaScript it will be stored recordwise in a QVariantMap with String indices
// TODO: find meaningful names for the different kinds of data (record, dataElement, ...) or define them
QVariantMap DSVFormatReader::processData(QTextStream & filestream){
	
	QVariantMap resultData = QVariantMap();
	int nextIndex = 0;
	
	while(!filestream.atEnd()){
		
		QString stringRecord = filestream.readLine();
		QVariantMap record = this->processLine(stringRecord);
		
		resultData.insert(
			QString::number(nextIndex),
			QVariant(record)
		);
		
		nextIndex++;
	}
	
	return resultData;
}

// assert |headerElements| == |dataElements|
// assert for every i in [0...|headerElements|-1]: header[i] isHeaderFor(lineData[i])
// TODO: divide data and meta data
QVariantMap DSVFormatReader::processLine(QString const & lineData){
	
	QVariantMap resultMap = QVariantMap();
	
	// 1.
	QStringList structuredLineData = this->structure(lineData);
	
	// 2.
	for(int index = 0; index < structuredLineData.length(); index++){
		
		QString stringDataElement = structuredLineData.at(index);
		QString headerElement = this->header.at(index);
		QVariant dataElement;
		
		// 2.1.
		if(!this->isErroneous(stringDataElement, index)){
			
			// 2.1.a
			dataElement = this->assignType(stringDataElement, index);
		}
		else{
			// 2.1.b
			dataElement = handleErroneous(stringDataElement, index);
		}
		
		// 2.2
		resultMap.insert(headerElement, dataElement);
	}
	
	return resultMap;
}

QStringList DSVFormatReader::structure(QString const & lineData){
	
	return lineData.split(this->delimiter);
}

bool DSVFormatReader::isErroneous(QString const & dataElement, int index){
	
	return !this->hasValidStructure(dataElement, index);
}

// TODO: implement by testing against given regular expression for structure
// assert there is only one valid regular expression
bool DSVFormatReader::hasValidStructure(QString const & dataElement, int index){
	
	return this->attributeStructure.exactMatch(dataElement);
}

// TODO: implement by assigning type (parse int, datetime, ...)
QVariant DSVFormatReader::assignType(QString const & dataElement, int index){
	
	// TODO: make more obvious
	// initialize captured texts
	this->attributeStructure.exactMatch(dataElement);
	return QVariant(this->attributeStructure.cap(1));
}

// TODO: implement
QVariant DSVFormatReader::handleErroneous(QString const & dataElement, int index){
	
	return QVariant(QString("ERRONEOUS:") + dataElement);
}

//########################
//### static functions ###
//########################
QVariantMap DSVFormatReader::processFile(QString const & path, QRegExp const & delimiter, QRegExp const & attributeStructure){
	
	DSVFormatReader reader(delimiter, attributeStructure);
	
	return reader.processFile(path);
}
