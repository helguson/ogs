#include "JavaScriptMessageForwardingQWebPage.h"
#include <iostream>

JavaScriptMessageForwardingQWebPage::JavaScriptMessageForwardingQWebPage(QWidget *parent) :
	QWebPage(parent)
{
}

void JavaScriptMessageForwardingQWebPage::javaScriptConsoleMessage(
		const QString &message,
		int lineNumber,
		const QString &sourceID){

	std::cout << "[" << sourceID.toStdString() << ":" << lineNumber << "] " 
		  << message.toStdString() << std::endl;
}
