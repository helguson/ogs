#include "JavaScriptGate.h"

JavaScriptGate::JavaScriptGate(QObject *parent) :
	QObject(parent)
{
}

void JavaScriptGate::transferData(int data){
	
	emit transferredData(data);
}
