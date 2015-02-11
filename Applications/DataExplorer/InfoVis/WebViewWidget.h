/**
 * \file
 * \author Lars Bilke, Thomas Hennig
 * \date   2014-11-03
 * \brief  Definition of the WebViewDialog class.
 *
 * \copyright
 * Copyright (c) 2014-2014, OpenGeoSys Community (http://www.opengeosys.org)
 *            Distributed under a Modified BSD License.
 *              See accompanying file LICENSE.txt or
 *              http://www.opengeosys.org/project/license
 *
 */

#ifndef WEBVIEWDIALOG_H
#define WEBVIEWDIALOG_H

#include "ui_WebViewWidget.h"
#include "JavaScriptGate.h"

#include <QVariant>
#include <QString>
#include <QRegExp>

#include <QTextStream>

#include <memory>
#include <vector>


/**
 * \brief a test environement for transferring data to JavaScript in order to
 * render it with d3.js
 */
class WebViewWidget : public QWidget, public Ui_WebViewWidget
{
	Q_OBJECT

public:
	/// Constructor
	WebViewWidget(QWidget* parent = 0);

	~WebViewWidget(void);

public slots:
	void announceToJavascript();
	
private:
	JavaScriptGate*	gate;

	void setUpSignalSlotConnections();
	void enableDebugging();
	void loadTestData();
};



#endif //WEBVIEWDIALOG_H
