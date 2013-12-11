#ifndef CLIENT_H_DEFINED
#define CLIENT_H_DEFINED

#include <QTcpSocket>
#include <QString>
#include <QObject>

#include "magic.h"

class TNotificationClient : public QObject {
	Q_OBJECT

	QTcpSocket *sock;
	QString serverHostname;
	quint16 serverPort;

	QTcpSocket *getSocket();

	Q_SLOT void handleNotification();
	Q_SLOT void socketClosed();

public:
	Q_SIGNAL void notificationCame();

	TNotificationClient(QString &serverHostname, quint16 serverPort);
	void exec();
};

inline void notify(){
	printf("New Messages!!!\n");
}

#endif //CLIENT_H_DEFINED