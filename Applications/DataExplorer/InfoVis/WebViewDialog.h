/**
 * \file
 * \author Lars Bilke
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

#include "ui_WebViewDialog.h"

#include <QtGui/QDialog>


/**
 * \brief A dialog for specifying the parameters to construct a mesh based on a raster
 */
class WebViewDialog : public QDialog, public Ui_WebViewDialog
{
	Q_OBJECT

public:
	/// Constructor
	WebViewDialog(QDialog* parent = 0);

	~WebViewDialog(void);

public slots:
	/// Load the given url
	//void loadUrl(QString url);

};

#endif //WEBVIEWDIALOG_H
