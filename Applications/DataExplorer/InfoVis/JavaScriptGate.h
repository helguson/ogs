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

#include <memory>

/**
 * @brief class that announces data to a specified JavaScript engine
 */
class JavaScriptGate : public QObject
{
	Q_OBJECT
public:
	explicit JavaScriptGate(QObject *parent = 0);
	
	void store(std::unique_ptr<QVariantList> values, std::unique_ptr<QVariantList> metaDataRelation, std::unique_ptr<QVariantList> baseDataIndices);
	void storeAndTransfer(std::unique_ptr<QVariantList> values, std::unique_ptr<QVariantList> metaDataRelation, std::unique_ptr<QVariantList> baseDataIndices);
	void transferEveryStored();
	void announceYourselfTo(QWebFrame * frame);
	
	static const QString GATE_ANNOUNCEMENT_NAME;
signals:
	void transferredData(QVariantList values, QVariantList metaDataRelation, QVariantList baseDataIndices);
	
private:
	void transferStored(int index);
	
	std::vector<std::unique_ptr<QVariantList>> valuesStorage;
	std::vector<std::unique_ptr<QVariantList>> metaDataRelationStorage;
	std::vector<std::unique_ptr<QVariantList>> baseDataIndicesStorage;
	
};

#endif // JAVASCRIPTGATE_H
