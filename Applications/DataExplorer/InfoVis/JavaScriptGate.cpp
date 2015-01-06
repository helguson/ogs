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
	storage()
{
}

//###############
//### methods ###
//###############
void JavaScriptGate::store(std::unique_ptr<QVariantList> values, std::unique_ptr<QVariantList> metaDataRelation){
	
	this->storage.push_back(
		DataPair()
	);
	
	this->storage.back().first = std::move(values);
	this->storage.back().second = std::move(metaDataRelation);
}

void JavaScriptGate::storeAndTransfer(std::unique_ptr<QVariantList> values, std::unique_ptr<QVariantList> metaDataRelation){
	
	this->store(std::move(values), std::move(metaDataRelation));
	
	this->transferStored(
		this->storage.size()-1	// last element
	);
}

void JavaScriptGate::transferEveryStored(){
	
	for(uint index = 0; index < this->storage.size(); index++){
		
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

	DataPair & pair = this->storage[index];
	
	emit transferredData(*(pair.first), *(pair.second));
}
