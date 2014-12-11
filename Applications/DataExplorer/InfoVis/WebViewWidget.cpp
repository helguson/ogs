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
	
	// generate test data
	QString testHeader = QString("name,colour,age,size");
	QString testData1 =  QString("Tina,green,22,xl");
	QString testData2 =  QString("Bernd,red,34,m");
	QRegExp delimiter = QRegExp(",");
	
	QStringList headerList = testHeader.split(delimiter);
	
	QVariantMap map1 = processLine(headerList, testData1, delimiter);
	QVariantMap map2 = processLine(headerList, testData2, delimiter);
	
	QVariantMap list = QVariantMap();
	list.insert("0", QVariant(map1));
	list.insert("1", QVariant(map2));
	
	this->gate->transferData(list);
}


// assert |headerElements| == |dataElements|
// assert for every i in [0...|headerElements|-1]: header[i] isHeaderFor(lineData[i])
QVariantMap WebViewWidget::processLine(QStringList const header, QString const lineData, QRegExp const delimiter){
	
	QVariantMap resultMap = QVariantMap();
	
	// 1.
	QStringList structuredLineData = structure(lineData, delimiter);
	
	// 2.
	for(int index = 0; index < structuredLineData.length(); index++){
		
		QString stringDataElement = structuredLineData.at(index);
		QString headerElement = header.at(index);
		QVariant dataElement;
		
		// 2.1.
		if(!isErroneous(stringDataElement, index)){
			
			// 2.1.a
			dataElement = assignType(stringDataElement, index);
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

QStringList WebViewWidget::structure(QString const lineData, QRegExp const delimiter){
	
	return lineData.split(delimiter);
}

bool WebViewWidget::isErroneous(QString const dataElement, int index){
	
	return hasValidStructure(dataElement, index);
}

// TODO: implement by testing against given regular expression for structure
bool WebViewWidget::hasValidStructure(QString const dataElement, int index){
	
	return true;
}

// TODO: implement by assigning type
QVariant WebViewWidget::assignType(QString const dataElement, int index){
	
	return QVariant(dataElement);
}

// TODO: implement
QVariant WebViewWidget::handleErroneous(QString const dataElement, int index){
	
	return QVariant(dataElement);
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
