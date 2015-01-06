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

#include <QWebFrame>

#include "DSVFormatReader.h"

WebViewWidget::WebViewWidget(QWidget* parent)
:
	QWidget(parent)
{
	setupUi(this);
	
	this->gate = new JavaScriptGate(this);

	this->setUpSignalSlotConnections();
	this->enableDebugging();
	
	this->loadTestData();
}


WebViewWidget::~WebViewWidget()
{
}

void WebViewWidget::enableDebugging(){
	
	// Question: is there a non global approach?
	QWebSettings::globalSettings()->setAttribute(QWebSettings::DeveloperExtrasEnabled, true);
}

void WebViewWidget::setUpSignalSlotConnections(){
	
	// automatic object announcement on JS start
	QWebFrame *frame = this->webView->page()->mainFrame();
	connect(frame, SIGNAL(javaScriptWindowObjectCleared()), this, SLOT(announceToJavascript()));
	
	connect(this->pushButton, SIGNAL(clicked()), this, SLOT(initiateDataTransfer()));
	connect(this->reloadButton, SIGNAL(clicked()), this->webView, SLOT(reload()));
}

void WebViewWidget::announceToJavascript()
{
	QWebFrame *frame = this->webView->page()->mainFrame();
	frame->addToJavaScriptWindowObject("qtPushButton", this->pushButton);
	
	this->gate->announceYourselfTo(frame);
}

void WebViewWidget::initiateDataTransfer(){
	
	this->gate->transferEveryStored();

}

void WebViewWidget::loadTestData(){
	try{
		QString relativeFilePath = "../../data/DB/";
		QString fileName = "Cosmic-Ray-Station Seelhausener See _Cosmic Ray Station 2_.csv";
		QRegExp delimiter(",");
		QVariantList attributeStructures = DSVFormatReader::getValidStructures();
		
		DSVFormatReader reader(delimiter, attributeStructures);
		reader.processFile(relativeFilePath + fileName);
		
		this->gate->store(
			reader.getValues(),
			reader.getMetaDataRelation()
		);
	}
	catch(int i){
		// TODO: implement
	}
}
