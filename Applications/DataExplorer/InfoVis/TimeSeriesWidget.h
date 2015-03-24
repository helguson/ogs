/**
 * @author Thomas Hennig
 * @date 2015-03-23
 * @brief class to forward data from dsv file to JavaScript in order to create
 * time series visualizations.
 * 
 */

#ifndef TIMESERIESWIDGET_H
#define TIMESERIESWIDGET_H

#include "WebViewWidget.h"


class TimeSeriesWidget : public WebViewWidget
{
	Q_OBJECT
public:
	explicit TimeSeriesWidget(QWidget *parent = 0);
	
	virtual ~TimeSeriesWidget();
	
signals:
	
public slots:
	
private:
	
	void loadTestData();
};

#endif // TIMESERIESWIDGET_H
