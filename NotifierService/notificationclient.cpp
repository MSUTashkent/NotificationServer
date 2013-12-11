#include "notificationclient.h"

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

void TNotificationClient::exec(){
	quint64 piece;
	if(serverHostname.isEmpty()){
		return;
	}
	while(true){
		if(sock==0){
			getSocket();
			connect(sock,SIGNAL(disconnected()),this,SLOT(socketClosed()));
		}
		if(sock->read((char*)&piece,sizeof(piece))){
			if(piece==MAGIC){
				Q_EMIT notificationCame();
			}
		}
	}
}
