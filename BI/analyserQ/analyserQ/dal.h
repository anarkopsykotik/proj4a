#ifndef DAL_H
#define DAL_H

#include <QObject>
#include <QSqlQuery>
#include <QSqlDatabase>


class Dal : public QObject
{
	Q_OBJECT

public:
	Dal(QObject *parent);
	~Dal();

private:
	
};

#endif // DAL_H
