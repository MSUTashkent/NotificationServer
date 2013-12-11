#include "notificationclient.h"
#include "magic.h"

QTcpSocket *TNotificationClient::getSocket(){
	if(sock==0){
		sock = new QTcpSocket;
		sock->connectToHost(serverHostname, serverPort, QIODevice::ReadOnly);
	}
	return sock;
}

void TNotificationClient::handleNotification(){
	notify();
}

void TNotificationClient::socketClosed(){
	if(sock!=0){
		sock->close();
		delete sock;
	}
}

TNotificationClient::TNotificationClient(QString &serverHostname, quint16 serverPort){
	this->serverHostname = serverHostname;
	this->serverPort = serverPort;
	connect(this,SIGNAL(notificationCame()),this,SLOT(handleNotification()));
}

void TNotificationClient::newDataAvailable(){
	quint64 piece;
	if(sock->read((char*)&piece,sizeof(piece))){
		if(piece==MAGIC){
			Q_EMIT notificationCame();
		}
	}
}

void TNotificationClient::run(){
	if(serverHostname.isEmpty()){
		return;
	}
	if(sock==0){
		getSocket();
		if(sock->isValid()){
			connect(sock,SIGNAL(disconnected()),this,SLOT(socketClosed()));
			connect(sock,SIGNAL(readyRead()),this,SLOT(newDataAvailable()));
		}
		
	}
	exec();
}
