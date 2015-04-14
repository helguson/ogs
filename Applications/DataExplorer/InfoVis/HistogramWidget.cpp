#include "HistogramWidget.h"

#include <iostream>

#include "BaseLib/Histogram.h"
#include "BaseLib/HistogramEqualSizedClassIntervalsFactory.h"
#include "BaseLib/HistogramGammaDistortedSizeClassIntervalsFactory.h"
#include "BaseLib/HistogramExponentialDistortedClassIntervalsFactory.h"
#include "HistogramDataTransformer.h"

HistogramWidget::HistogramWidget(std::vector<double> const & metricsData, QWidget *parent) :
	WebViewWidget(parent)
{

	this->setAttribute(Qt::WA_DeleteOnClose);
	this->webView->load(QUrl("./QtWebkitRessources/Histogram.html"));
	
	std::unique_ptr<BaseLib::histogram::ClassIntervalFactory<double>> equalSizedIntervalsFactoryPtr(
		new BaseLib::histogram::EqualSizedClassIntervalsFactory<double>()
	);
	
	this->processMetricsData(
		"equal sized intervals",
		metricsData,
		std::move(equalSizedIntervalsFactoryPtr)
	);

	std::unique_ptr<BaseLib::histogram::ClassIntervalFactory<double>> powerDistortedIntervalsFactoryPtr(
		new BaseLib::histogram::GammaDistortedSizeClassIntervalsFactory<double>(2.0)
	);
	
	this->processMetricsData(
		"gamma distorted intervals: g(t)=t^2; t in [0, 1]",
		metricsData,
		std::move(powerDistortedIntervalsFactoryPtr)
	);
	
	std::unique_ptr<BaseLib::histogram::ClassIntervalFactory<double>> exponentialDistortedIntervalsFactoryPtr(
		new BaseLib::histogram::ExponentialDistortedClassIntervalsFactory<double>(10.0)
	);
	
	this->processMetricsData(
		"exponential distorted intervals: g(t)=(10^t-1)/(10-1); t in [0, 1]",
		metricsData,
		std::move(exponentialDistortedIntervalsFactoryPtr)
	);

}

void HistogramWidget::processMetricsData(QString const & description, std::vector<double> const & metricsData, std::unique_ptr<BaseLib::histogram::ClassIntervalFactory<double>> intervalFactoryPtr){
	
	// generate intervals and histogram data
	BaseLib::Histogram<double> histogram(metricsData, true, std::move(intervalFactoryPtr));
	
	// transform data
	HistogramDataTransformer<double> dataTransformer = HistogramDataTransformer<double>();
	dataTransformer.process(histogram);
	
	// transfer data
	this->gate->storeAndTransfer(
		description,
		dataTransformer.getValues(),
		dataTransformer.getMetaDataRelation(),
		dataTransformer.getBaseDataIndices()
	);
}
