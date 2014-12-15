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
		
		QVariantMap data = DSVFormatReader::processFile(relativeFilePath + fileName, delimiter, attributeStructure);
		
		this->gate->transferData(data);
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
