/**
 * \file
 * \author Lars Bilke
 * \date   2014-11-03
 * \brief  Implementation of the WebViewWidget class.
 *
 * \copyright
 * Copyright (c) 2014-2014, OpenGeoSys Community (http://www.opengeosys.org)
 *            Distributed under a Modified BSD License.
 *              See accompanying file LICENSE.txt or
 *              http://www.opengeosys.org/project/license
 *
 */

#include "WebViewWidget.h"
#include "JavaScriptMessageForwardingQWebPage.h"

#include <QWebFrame>
#include <iostream>

#include <QFile>

WebViewWidget::WebViewWidget(QWidget* parent)
: QWidget(parent)
{
	setupUi(this);
	
	this->gate = new JavaScriptGate(this);
	this->transferCount = 0;
	this->prepareJavaScriptMessageForwarding();

	this->setUpAutomaticObjectPublishing();
	
	connect(this->pushButton, SIGNAL(clicked()), this, SLOT(initiateDataTransfer()));
	connect(this->reloadButton, SIGNAL(clicked()), this->webView, SLOT(reload()));
}


WebViewWidget::~WebViewWidget()
{
}

void WebViewWidget::addToJavascript()
{
	QWebFrame *frame = this->webView->page()->mainFrame();
	frame->addToJavaScriptWindowObject("qtPushButton", this->pushButton);
	frame->addToJavaScriptWindowObject("gate", this->gate);
}

void WebViewWidget::initiateDataTransfer(){
	
	try{
		QString relativeFilePath = "../../data/DB/";
		QString fileName = "Cosmic-Ray-Station Seelhausener See _Cosmic Ray Station 2_.csv";
		QRegExp delimiter(",");
		QRegExp attributeStructure("\"([^\"]*)\"");
		
		QVariantMap data = processFile(relativeFilePath + fileName, delimiter, attributeStructure);
		
		this->gate->transferData(data);
	}
	catch(int i){
		// TODO: implement
	}
}

// TODO: implement
// assert delimiter-seperated-values-format
QVariantMap WebViewWidget::processFile(QString const & path, QRegExp const & delimiter, QRegExp const & attributeStructure){
	
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
	QVariantMap processedData = processFileData(filestream, delimiter, attributeStructure);
	
	file.close();
	
	return processedData;
}

QVariantMap WebViewWidget::processFileData(QTextStream & filestream, QRegExp const & delimiter, QRegExp const & attributeStructure){
	
	QStringList header = processMetaData(filestream, delimiter, attributeStructure);
	
	return processData(filestream, header, delimiter, attributeStructure);
}

// assert use before 'processData', due to structure of dsv-format (header data in first line), so that filestream contains no meta data but data afterwards
QStringList WebViewWidget::processMetaData(QTextStream & filestream, QRegExp const & delimiter, QRegExp const & attributeStructure){
	
	QString headerLine = filestream.readLine();
	QStringList header = headerLine
			     .split(delimiter)
			     .replaceInStrings(attributeStructure, "\\1");	// TODO: Where should that step be put and how should it be named? data extraction?
	
	return header;
}

// assert filestream contains no meta data but data
// in order to transfer file data at once to JavaScript it will be stored recordwise in a QVariantMap with String indices
QVariantMap WebViewWidget::processData(QTextStream & filestream, QStringList const & header, QRegExp const & delimiter, QRegExp const & attributeStructure){
	
	QVariantMap resultData = QVariantMap();
	int nextIndex = 0;
	
	while(!filestream.atEnd()){
		
		QString stringRecord = filestream.readLine();
		QVariantMap record = processLine(stringRecord, header, delimiter, attributeStructure);
		
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
QVariantMap WebViewWidget::processLine(QString const & lineData, QStringList const & header, QRegExp const & delimiter, QRegExp const & attributeStructure){
	
	QVariantMap resultMap = QVariantMap();
	
	// 1.
	QStringList structuredLineData = structure(lineData, delimiter);
	
	// 2.
	for(int index = 0; index < structuredLineData.length(); index++){
		
		QString stringDataElement = structuredLineData.at(index);
		QString headerElement = header.at(index);
		QVariant dataElement;
		
		// 2.1.
		if(!isErroneous(stringDataElement, index, attributeStructure)){
			
			// 2.1.a
			dataElement = assignType(stringDataElement, index, attributeStructure);
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

QStringList WebViewWidget::structure(QString const & lineData, QRegExp const & delimiter){
	
	return lineData.split(delimiter);
}

bool WebViewWidget::isErroneous(QString const & dataElement, int index, QRegExp const & attributeStructure){
	
	return !hasValidStructure(dataElement, index, attributeStructure);
}

// TODO: implement by testing against given regular expression for structure
bool WebViewWidget::hasValidStructure(QString const & dataElement, int index, QRegExp const & attributeStructure){
	
	return attributeStructure.exactMatch(dataElement);
}

// TODO: implement by assigning type (parse int, datetime, ...)
QVariant WebViewWidget::assignType(QString const & dataElement, int index, QRegExp const & attributeStructure){
	
	// TODO: make more obvious
	// initialize captured texts
	attributeStructure.exactMatch(dataElement);
	return QVariant(attributeStructure.cap(1));
}

// TODO: implement
QVariant WebViewWidget::handleErroneous(QString const & dataElement, int index){
	
	return QVariant(QString("ERRONEOUS:") + dataElement);
}

/// replace QWebPage element in order to forward JavaScript console messages
void WebViewWidget::prepareJavaScriptMessageForwarding(){
	
	QWebPage* javaScriptMessageForwardingQWebPage = new JavaScriptMessageForwardingQWebPage(this->webView);
	this->webView->setPage(javaScriptMessageForwardingQWebPage);
}

void WebViewWidget::setUpAutomaticObjectPublishing(){
	
	QWebFrame *frame = this->webView->page()->mainFrame();
	connect(frame, SIGNAL(javaScriptWindowObjectCleared()), this, SLOT(addToJavascript()));
}
