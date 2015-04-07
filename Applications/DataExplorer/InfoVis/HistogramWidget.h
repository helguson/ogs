#ifndef HISTOGRAMWIDGET_H
#define HISTOGRAMWIDGET_H

#include "WebViewWidget.h"

#include <vector>
#include <QString>

#include "BaseLib/HistogramClassIntervalFactory.h"

class HistogramWidget : public WebViewWidget
{
	Q_OBJECT
public:
	explicit HistogramWidget(std::vector<double> const & metricsData, QWidget *parent = 0);
	
signals:
	
public slots:
	
private:
	void processMetricsData(QString const & description, std::vector<double> const & metricsData, std::unique_ptr<BaseLib::histogram::ClassIntervalFactory<double>> intervalFactoryPtr);
	
};

#endif // HISTOGRAMWIDGET_H
