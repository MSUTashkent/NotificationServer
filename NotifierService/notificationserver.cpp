#include "notificationserver.h"
#include <QMutex>

static QMutex mutex;

void  TConnection::lock(){mutex.lock();}
void TConnection::unlock(){mutex.unlock();}

TNotificationServer::TNotificationServer(){
	timer = new QTimer;
	connect(timer,SIGNAL(timeout()),this,SLOT(handleTimer()));
	timer->start(TIMER_PERIOD);

	server = new QTcpServer;
	server->listen(QHostAddress::Any, OUT_SOCKET_PORT);
	connect(server,SIGNAL(newConnection()),this,SLOT(handleNewConnection));
	server->waitForNewConnection();
}

void TNotificationServer::handleNewConnection(){
	while(server->hasPendingConnections()){
		QTcpSocket *tcpCon = server->nextPendingConnection();
		if(tcpCon!=0){
			TConnection *con = new TConnection;
			con->connection = tcpCon;
			con->lastActive = QDateTime::currentDateTime();
			activeConnections << con;
		}
	}
	server->waitForNewConnection();
}

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