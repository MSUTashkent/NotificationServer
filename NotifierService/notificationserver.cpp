#include "notificationserver.h"
#include <QMutex>

#include "magic.h"

#define IN_SOCKET_PORT 11040
#define OUT_SOCKET_PORT 11050

//in milliseconds
#define CONNECTION_TIMEOUT 30000
#define TIMER_PERIOD 5000

struct TNotification {
	static const quint64 magic = MAGIC;
	static void writeToSocket(QTcpSocket *sock){
		sock->write((char*)&magic,sizeof(magic));
	}
};

static QMutex mutex;

struct TConnection {
	QTcpSocket *connection;
	QDateTime lastActive;

	static void lock();
	static void unlock();
};

void  TConnection::lock(){mutex.lock();}
void TConnection::unlock(){mutex.unlock();}

TNotificationServer::TNotificationServer(QObject *parent) :  QTcpServer(parent) {
	//timer = new QTimer;
	//connect(timer,SIGNAL(timeout()),this,SLOT(handleTimer()));
	//timer->start(TIMER_PERIOD);

	listen(QHostAddress::Any, OUT_SOCKET_PORT);
	connect(this,SIGNAL(newConnection()),this,SLOT(handleNewConnection()));
	waitForNewConnection();
}

void TNotificationServer::handleNewConnection(){
	while(hasPendingConnections()){
		QTcpSocket *tcpCon = nextPendingConnection();
		if(tcpCon!=0){
			TConnection *con = new TConnection;
			con->connection = tcpCon;
			con->lastActive = QDateTime::currentDateTime();
			activeConnections << con;
		}
	}
	waitForNewConnection();
}

/*
void TNotificationServer::handleTimer(){
	QDateTime now = QDateTime::currentDateTime();
	qint64 elapsed;
	QList<TConnection*>::iterator it;
	TConnection *con;

	for(it=activeConnections.begin(); it!=activeConnections.end(); it++){
		con = *it;
		elapsed = con->lastActive.msecsTo(now);
		if(elapsed>=CONNECTION_TIMEOUT){  //should we move the IF to section
			TConnection::lock();
			breakConnection(con);
			activeConnections.erase(it);
			TConnection::unlock();
		}
	}
}
*/

void TNotificationServer::breakConnection(TConnection *con){
	con->connection->close();
	delete con->connection;
}


void TNotificationServer::notifyAll(){
	TNotification notfication;
	Q_FOREACH(TConnection *con, activeConnections){
		TConnection::lock();
		notfication.writeToSocket(con->connection);
		TConnection::unlock();
	}
}

TNotificationServer::~TNotificationServer(){
	QList<TConnection*>::iterator it;
	TConnection *con;
	for(it=activeConnections.begin(); it!=activeConnections.end(); it++){
		con = *it;
		TConnection::lock();
		breakConnection(con);
		activeConnections.erase(it);
		TConnection::unlock();
	}
}