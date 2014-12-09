/**
 * \author Thomas Hennig
 * \date   2014-12-09
 * \brief  extension of QWebView to forward JavaScript console messages
 */


#ifndef JAVASCRIPTMESSAGEFORWARDINGQWEBPAGE_H
#define JAVASCRIPTMESSAGEFORWARDINGQWEBPAGE_H

#include <QWebPage>

class JavaScriptMessageForwardingQWebPage : public QWebPage
{
	Q_OBJECT
public:
	explicit JavaScriptMessageForwardingQWebPage(QWidget *parent = 0);

protected:
	virtual void javaScriptConsoleMessage(const QString& message,
					      int lineNumber,
					      const QString& sourceID) override;
	
signals:
	
public slots:
	
};

#endif // JAVASCRIPTMESSAGEFORWARDINGQWEBPAGE_H
