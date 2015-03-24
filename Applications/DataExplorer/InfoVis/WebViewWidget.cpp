/**
 * \file
 * \author Lars Bilke, Thomas Hennig
 * \date   2015-03-24
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


WebViewWidget::WebViewWidget(QWidget* parent)
:
	QWidget(parent)
{
	setupUi(this);
	
	this->gate = new JavaScriptGate(this);

	this->setUpSignalSlotConnections();
	this->enableDebugging();
	
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
	
	connect(this->reloadButton, SIGNAL(clicked()), this->webView, SLOT(reload()));
}

void WebViewWidget::announceToJavascript()
{
	QWebFrame *frame = this->webView->page()->mainFrame();
	this->gate->announceYourselfTo(frame);
}


