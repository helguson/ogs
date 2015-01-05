#include "Data.h"

Data::Data(QVariantList const & m_data, QObject *parent)
:
	QObject(parent),
	m_data(m_data)
{}

QVariantList Data::getData(){
	
	return this->m_data;
}
