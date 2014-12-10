#include "JavaScriptGate.h"

JavaScriptGate::JavaScriptGate(QObject *parent) :
	QObject(parent)
{
}

void JavaScriptGate::transferData(QVariantMap data){
	
	emit transferredData(data);
}
