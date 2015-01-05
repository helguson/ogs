#include "JavaScriptGate.h"
#include <iostream>

JavaScriptGate::JavaScriptGate(QObject *parent)
:
	QObject(parent),
	storage()
{
}

void JavaScriptGate::store(std::unique_ptr<MetaData> metaData, std::unique_ptr<Data> data){
	
	this->storage.push_back(
		DataPair()
	);
	
	this->storage.back().first = std::move(metaData);
	this->storage.back().second = std::move(data);
}

void JavaScriptGate::storeAndAnnounce(std::unique_ptr<MetaData> metaData, std::unique_ptr<Data> data, QWebFrame *frame){
	
	this->store(std::move(metaData), std::move(data));
	
	this->announceStoredTo(
		this->storage.size()-1,	// last element
		frame		
	);
}

void JavaScriptGate::announceEveryStoredTo(QWebFrame *frame){
	
	for(uint index = 0; index < this->storage.size(); index++){
		
		this->announceStoredTo(index, frame);
	}
}

void JavaScriptGate::announceAllTo(QWebFrame* frame){
	
	this->announceYourselfTo(frame);
	this->announceEveryStoredTo(frame);
}

void JavaScriptGate::announceYourselfTo(QWebFrame* frame){
	
	this->announceTo(
		"gate",
		this,
		frame
	);
}

void JavaScriptGate::announceStoredTo(int index, QWebFrame *frame){
	std::pair<QString, QString> names = getNamesForAnnouncement(index);
	
	// announce metaData
	this->announceTo(
		names.first,
		this->storage.at(index).first.get(),
		frame
	);
	
	// announce data
	this->announceTo(
		names.second,
		this->storage.at(index).second.get(),
		frame
	);
	
	emit announcedData(names.first, names.second);
}

void JavaScriptGate::announceTo(QString nameForAnnoucement, QObject* object, QWebFrame* frame){
	
	frame->addToJavaScriptWindowObject(
		nameForAnnoucement,
		object
	);
}

std::pair<QString, QString> JavaScriptGate::getNamesForAnnouncement(int index){
	
	QString metaDataName = "metaData_" + QString::number(index);
	QString dataName = "data_" + QString::number(index);
	
	return std::pair<QString, QString>(metaDataName, dataName);
}
