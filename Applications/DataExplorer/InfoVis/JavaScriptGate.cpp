#include "JavaScriptGate.h"

//#################
//### constants ###
//#################
const QString JavaScriptGate::GATE_ANNOUNCEMENT_NAME = QString("gate");

//######################################
//### constructors and deconstructor ###
//######################################
JavaScriptGate::JavaScriptGate(QObject *parent)
:
	QObject(parent),
	descriptionStorage(),
	valuesStorage(),
	metaDataRelationStorage(),
	baseDataIndicesStorage()
{
}

//###############
//### methods ###
//###############
void JavaScriptGate::store(QString description, std::unique_ptr<QVariantList> values, std::unique_ptr<QVariantList> metaDataRelation, std::unique_ptr<QVariantList> baseDataIndices){
	
	this->descriptionStorage.push_back(description);
	this->valuesStorage.push_back(std::move(values));
	this->metaDataRelationStorage.push_back(std::move(metaDataRelation));
	this->baseDataIndicesStorage.push_back(std::move(baseDataIndices));
}

void JavaScriptGate::storeAndTransfer(QString description, std::unique_ptr<QVariantList> values, std::unique_ptr<QVariantList> metaDataRelation, std::unique_ptr<QVariantList> baseDataIndices){
	
	this->store(
		description,
		std::move(values),
		std::move(metaDataRelation),
		std::move(baseDataIndices)
	);
	
	this->transferStored(
		this->valuesStorage.size()-1	// last element
	);
}

void JavaScriptGate::transferEveryStored(){
	
	for(uint index = 0; index < this->valuesStorage.size(); index++){
		
		this->transferStored(index);
	}
}

void JavaScriptGate::announceYourselfTo(QWebFrame* frame){
	
	frame->addToJavaScriptWindowObject(
		JavaScriptGate::GATE_ANNOUNCEMENT_NAME,
		this
	);
}

void JavaScriptGate::transferStored(int index){
	
	emit transferredData(
		this->descriptionStorage[index],
		*(this->valuesStorage[index]),
		*(this->metaDataRelationStorage[index]),
		*(this->baseDataIndicesStorage[index])
	);
}
