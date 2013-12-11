
#include <QtCore/QCoreApplication>

#include <QObject>
#include "notificationclient.h"
#include "notificationserver.h"

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);
	
	TNotificationServer server;
	TNotificationClient client(QString("localhost"),11050);
	client.start();
	QTimer *timer = new QTimer;
	QObject::connect(timer,SIGNAL(timeout()),&server,SLOT(notifyAll()));
	timer->start(500);
	return a.exec();
}
