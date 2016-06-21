#include "dal.h"

Dal::Dal(QObject *parent)
	: QObject(parent)
{
	QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
	db.setHostName("bigblue");
	db.setDatabaseName("flightdb");
	db.setUserName("acarlson");
	db.setPassword("1uTbSbAs");
	bool ok = db.open();


	QSqlDatabase firstDB = QSqlDatabase::addDatabase("QMYSQL", "first");
	QSqlDatabase secondDB = QSqlDatabase::addDatabase("QMYSQL", "second");
}

Dal::~Dal()
{

}
