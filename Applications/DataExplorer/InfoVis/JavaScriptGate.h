/**
 * \author Thomas Hennig
 * \date   2014-12-09
 * \brief  QObject for transferring data and meta data to JavaScript
 */


#ifndef JAVASCRIPTGATE_H
#define JAVASCRIPTGATE_H

#include <QObject>
#include <QVariant>
#include <QWebFrame>
#include <QString>

#include <memory>
#include <utility>
#include "MetaData.h"
#include "Data.h"

/**
 * @brief class that announces data to a specified JavaScript engine
 */
class JavaScriptGate : public QObject
{
	Q_OBJECT
public:
	explicit JavaScriptGate(QObject *parent = 0);
	
	void store(std::unique_ptr<MetaData> metaData, std::unique_ptr<Data> data);
	void storeAndAnnounce(std::unique_ptr<MetaData> metaData, std::unique_ptr<Data> data, QWebFrame * frame);
	void announceAllTo(QWebFrame * frame);
	
signals:
	void announcedData(QString metaDataName, QString dataName);
	
private:
	void announceStoredTo(int index, QWebFrame * frame);
	void announceYourselfTo(QWebFrame * frame);
	void announceEveryStoredTo(QWebFrame * frame);
	void announceTo(QString nameForAnnoucement, QObject* object, QWebFrame* frame);
	
	typedef std::pair<std::unique_ptr<MetaData>, std::unique_ptr<Data>> DataPair;
	
	std::vector<DataPair>	storage;
	
	static std::pair<QString, QString> getNamesForAnnouncement(int index);
};

#endif // JAVASCRIPTGATE_H
