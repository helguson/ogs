#include "Data.h"

DataBuilder::DataBuilder()
:
	values(),
	metaDataRelation()
{}

int DataBuilder::addDateAndReturnIndex(QVariant date){
	
	this->values.push_back(date);
	
	// create metaDataRelatio entry for this value
	this->metaDataRelation.push_back(QList<int>());
	
	return this->values.length()-1;
}

void DataBuilder::addAsMetaDataFor(int dateIndex, int metaDateIndex){
	
	this->metaDataRelation[dateIndex].push_back(metaDateIndex);
}

std::unique_ptr<QVariantList> DataBuilder::getValues(){
	
	std::unique_ptr<QVariantList> result(new QVariantList(this->values));
	
	return std::move(result);
}

std::unique_ptr<QVariantList> DataBuilder::getMetaDataRelation(){
	
	std::unique_ptr<QVariantList> result(new QVariantList());
	
	for(QList<int> list: this->metaDataRelation){
		
		result->push_back(
			QVariant(
				toQVariantList(list)
			)
		);
	}
	
	return std::move(result);
}

void DataBuilder::reset(){
	
	this->values.clear();
	this->metaDataRelation.clear();
}

QVariantList DataBuilder::toQVariantList(QList<int> list){
	
	QVariantList result;
	
	for(int index = 0; index < list.length(); index++){
			
		int metaDataIndex = list[index];
		
		result.push_back(
			QVariant(metaDataIndex)
		);
	}
	
	return result;
}
