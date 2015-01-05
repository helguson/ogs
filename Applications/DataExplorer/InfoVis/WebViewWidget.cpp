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

#include "DSVFormatReader.h"
#include "MetaData.h"

WebViewWidget::WebViewWidget(QWidget* parent)
:
	QWidget(parent)
{
	setupUi(this);
	
	this->gate = new JavaScriptGate(this);
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
	
	this->gate->announceAllTo(frame);
}

void WebViewWidget::initiateDataTransfer(){
	
	try{
		QString relativeFilePath = "../../data/DB/";
		QString fileName = "Cosmic-Ray-Station Seelhausener See _Cosmic Ray Station 2_.csv";
		QRegExp delimiter(",");
		QVariantList attributeStructures = DSVFormatReader::getValidStructures();
		
		DSVFormatReader reader(delimiter, attributeStructures);
		reader.processFile(relativeFilePath + fileName);
		
		this->gate->storeAndAnnounce(
			reader.getMetaData(),
			reader.getData(),
			this->webView->page()->mainFrame()
		);
	}
	catch(int i){
		// TODO: implement
	}
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
