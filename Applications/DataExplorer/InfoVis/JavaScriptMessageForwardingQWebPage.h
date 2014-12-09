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
