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
#include <QVariant>
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
	QString testHeader = QString("name,colour,age");
	QString testData =  QString("Tina,green,22");
	QRegExp delimiter = QRegExp(",");
	
	QVariantMap map = QVariantMap();
	
	QStringList headerList = testHeader.split(delimiter);
	QStringList dataList = testData.split(delimiter);
	
	if(headerList.length() == dataList.length()){
		
		for(int i = 0; i < headerList.length(); i++){
			
			QString key = QString(headerList.at(i));
			QVariant value = QVariant(dataList.at(i));
			
			//map.insert(headerList.at(i),QVariant(dataList.at(i)));
			map.insert(key,value);
		}
		
		std::cout << map[headerList.at(0)].toString().toStdString() << std::endl;
	}
	else{
		// TODO: either warn or handle it
		std::cout << "element count of header and data is not equal" << std::endl;
	}
	
	this->gate->transferData(this->transferCount);
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
