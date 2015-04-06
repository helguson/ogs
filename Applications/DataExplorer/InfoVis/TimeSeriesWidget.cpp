#include "TimeSeriesWidget.h"

#include <QVariant>
#include <QString>
#include <QRegExp>

#include <QTextStream>

#include <memory>
#include <vector>

#include "DSVFormatReader.h"

TimeSeriesWidget::TimeSeriesWidget(QWidget *parent) :
	WebViewWidget(parent)
{
	
	this->setAttribute(Qt::WA_DeleteOnClose);
	this->webView->load(QUrl("./QtWebkitRessources/D3Test.html"));
	
	
	this->loadTestData();
}

TimeSeriesWidget::~TimeSeriesWidget(){
}

void TimeSeriesWidget::loadTestData(){
	try{
		QString relativeFilePath = "../../data/DB/";
		QString fileName = "Cosmic-Ray-Station Seelhausener See _Cosmic Ray Station 2_.csv";
		QRegExp delimiter(",");
		QVariantList attributeStructures = DSVFormatReader::getValidStructures();
		
		DSVFormatReader reader(delimiter, attributeStructures);
		reader.processFile(relativeFilePath + fileName);
		
		this->gate->storeAndTransfer(
			fileName,
			reader.getValues(),
			reader.getMetaDataRelation(),
			reader.getBaseDataIndices()
		);
	}
	catch(int i){
		// TODO: implement
	}
}
