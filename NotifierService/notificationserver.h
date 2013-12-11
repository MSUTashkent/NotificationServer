#ifndef SERVER_H_DEFINED
#define SERVER_H_DEFINED

#include <QTcpServer>
#include <QTcpSocket>
#include <QHostAddress>
#include <QDateTime>
#include <QTimer>
#include <QList>

struct TConnection;

class TNotificationServer : public QTcpServer {
	Q_OBJECT
	
	QList<TConnection*> activeConnections;
	QTimer *timer;
protected:
	Q_SLOT void handleNewConnection();
	Q_SLOT void breakConnection(TConnection*);
	//Q_SLOT void handleTimer();
	Q_SLOT void notifyAll();
public:
	TNotificationServer(QObject *parent=0);
	~TNotificationServer();
};

#endif //SERVER_H_DEFINED