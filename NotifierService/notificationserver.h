#ifndef SERVER_H_DEFINED
#define SERVER_H_DEFINED

#include <QTcpServer>
#include <QTcpSocket>
#include <QHostAddress>
#include <QDateTime>
#include <QTimer>
#include <QList>

#include "magic.h"

#define IN_SOCKET_PORT 11040
#define OUT_SOCKET_PORT 11050

//in milliseconds
#define CONNECTION_TIMEOUT 30000
#define TIMER_PERIOD 5000

struct TConnection {
	QTcpSocket *connection;
	QDateTime lastActive;
	
	static void lock();
	static void unlock();
};

struct TNotification {
	static const quint64 magic = MAGIC;
	virtual void writeToSocket(QTcpSocket *sock){
			sock->write((char*)&magic,sizeof(magic));
	}
};

class TNotificationServer : public QObject {
	Q_OBJECT

	QList<TConnection*> activeConnections;
	QTcpServer *server;
	QTimer *timer;
protected:
	Q_SLOT void handleNewConnection();
	Q_SLOT void breakConnection(TConnection*);
	Q_SLOT void handleTimer();
	Q_SLOT void notifyAll();
public:
	TNotificationServer();
	~TNotificationServer();
};

#endif //SERVER_H_DEFINED