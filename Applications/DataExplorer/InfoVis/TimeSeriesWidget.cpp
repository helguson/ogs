#include "TimeSeriesWidget.h"

#include <QVariant>
#include <QString>
#include <QStringList>
#include <QRegExp>

#include <QTextStream>

#include <memory>
#include <vector>

#include "DSVFormatReader.h"

TimeSeriesWidget::TimeSeriesWidget(QWidget *parent) :
	WebViewWidget(parent)
{

	this->setAttribute(Qt::WA_DeleteOnClose);
	this->webView->load(QUrl("./QtWebkitRessources/TimeSeriesVisualization.html"));


	this->loadTestData();
}

TimeSeriesWidget::~TimeSeriesWidget(){
}

void TimeSeriesWidget::loadTestData(){
	try{
		QString relativeFilePath = "../../data/Bode und Selke/";
		QStringList fileNames;
		fileNames
				<< "Wasserguetemessstation Bode-Selke _Gross Germersleben _GGL__.csv"
				<< "Wasserguetemessstation Bode-Selke _HAU_Pegel  _Hausneindorf__.csv"
				<< "Wasserguetemessstation Bode-Selke _MEI_Pegel _Meisdorf__.csv"
				<< "Wasserguetemessstation Bode-Selke _SB_Pegel _Sauerbach_Borde__.csv";
		
		QRegExp delimiter(",");
		QVariantList attributeStructures = DSVFormatReader::getValidStructures();

		DSVFormatReader reader(delimiter, attributeStructures);
		
		for(int i = 0; i < fileNames.size(); i++){
			
			QString fileName = fileNames.at(i);
			reader.processFile(relativeFilePath + fileName);
	
			this->gate->storeAndTransfer(
				fileName,
				reader.getValues(),
				reader.getMetaDataRelation(),
				reader.getBaseDataIndices()
			);
		}
		
		
	}
	catch(int i){
		// TODO: implement
	}
}
