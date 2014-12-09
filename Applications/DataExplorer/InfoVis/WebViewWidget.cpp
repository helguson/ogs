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

WebViewWidget::WebViewWidget(QWidget* parent)
: QWidget(parent)
{
	setupUi(this);
	
	// replace QWebPage element in order to forward JavaScript console messages
	QWebPage* javaScriptMessageForwardingQWebPage = new JavaScriptMessageForwardingQWebPage(this->webView);
	this->webView->setPage(javaScriptMessageForwardingQWebPage);

	QWebFrame *frame = this->webView->page()->mainFrame();
	connect(frame, SIGNAL(javaScriptWindowObjectCleared()), this, SLOT(addToJavascript()));
}


WebViewWidget::~WebViewWidget()
{
}

void WebViewWidget::addToJavascript()
{
	QWebFrame *frame = this->webView->page()->mainFrame();
	frame->addToJavaScriptWindowObject("qtPushButton", this->pushButton);
}
