/**
 * \file
 * \author Lars Bilke
 * \date   2014-11-03
 * \brief  Implementation of the WebViewDialog class.
 *
 * \copyright
 * Copyright (c) 2014-2014, OpenGeoSys Community (http://www.opengeosys.org)
 *            Distributed under a Modified BSD License.
 *              See accompanying file LICENSE.txt or
 *              http://www.opengeosys.org/project/license
 *
 */

#include "WebViewDialog.h"

WebViewDialog::WebViewDialog(QDialog* parent)
: QDialog(parent)
{
	setupUi(this);
}


WebViewDialog::~WebViewDialog()
{
}

//void WebViewDialog::loadUrl(QString url)
//{
//	this->webView->load(QUrl(url));
//	//this->webView->load(QUrl("http://google.com"));
//}
