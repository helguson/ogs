#ifndef DATA_H
#define DATA_H

#include <QObject>
#include <QVariant>

/**
 * @brief wrapper class to hull data in a QObject, in order to annouce it to
 * JavaScript like MetaData
 */
class Data : public QObject
{
	Q_OBJECT
	//Q_PROPERTY(QVariantList data MEMBER m_data)
	//Q_PROPERTY(QVariantList data READ getData)
	
public:
	explicit Data(QVariantList const & m_data,  QObject *parent = 0);
	QVariantList getData();
	
private:
	QVariantList m_data;
	
};

#endif // DATA_H
