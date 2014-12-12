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


/**
 * \brief A dialog for specifying the parameters to construct a mesh based on a raster
 */
class WebViewWidget : public QWidget, public Ui_WebViewWidget
{
	Q_OBJECT

public:
	/// Constructor
	WebViewWidget(QWidget* parent = 0);

	~WebViewWidget(void);

public slots:
	/// Load the given url
	//void loadUrl(QString url);

	void addToJavascript();
	void initiateDataTransfer();
	
private:
	JavaScriptGate* gate;
	int transferCount;
	
	void prepareJavaScriptMessageForwarding();
	void setUpAutomaticObjectPublishing();
	
	static QVariantMap processFile(QString const & path, QRegExp const & delimiter, QRegExp const & attributeStructure);
	static QVariantMap processFileData(QTextStream  & filestream, QRegExp const & delimiter, QRegExp const & attributeStructure);
	static QStringList processMetaData(QTextStream & filestream, QRegExp const & delimiter, QRegExp const & attributeStructure);
	static QVariantMap processData(QTextStream & filestream, QStringList const & header, QRegExp const & delimiter, QRegExp const & attributeStructure);
	
	static QVariantMap processLine(QString const & lineData, QStringList const & header, QRegExp const & delimiter, QRegExp const & attributeStructure);
	static QStringList structure(QString const & lineData, QRegExp const & delimiter);
	static bool isErroneous(QString const & dataElement, int index, QRegExp const & attributeStructure);
	static bool hasValidStructure(QString const & dataElement, int index, QRegExp const & attributeStructure);
	static QVariant assignType(QString const & dataElement, int index, QRegExp const & attributeStructure);
	static QVariant handleErroneous(QString const & dataElement, int index);
};



#endif //WEBVIEWDIALOG_H
