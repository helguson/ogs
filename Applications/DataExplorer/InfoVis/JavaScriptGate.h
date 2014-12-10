/**
 * \author Thomas Hennig
 * \date   2014-12-09
 * \brief  QObject for transferring data to JavaScript
 */


#ifndef JAVASCRIPTGATE_H
#define JAVASCRIPTGATE_H

#include <QObject>
#include <QVariant>

class JavaScriptGate : public QObject
{
	Q_OBJECT
public:
	explicit JavaScriptGate(QObject *parent = 0);
	
	/**
	 * @brief triggers signal 'transferredData' to be send with given 'data'
	 * @param data that will be send
	 */
	void transferData(QVariantMap data);
	
signals:
	void transferredData(QVariantMap data);
	
public slots:
	
};

#endif // JAVASCRIPTGATE_H
