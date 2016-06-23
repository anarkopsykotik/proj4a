#ifndef ANALYSERQ_H
#define ANALYSERQ_H

#include <QtWidgets/QWidget>
#include <QtWidgets>
#include "ui_analyserq.h"
#include <QQuickView>
#include <QQuickItem>
#include <qqmlengine.h>
#include "httpRequestWorker.h"

class analyserQ : public QWidget
{
	Q_OBJECT

public:
	analyserQ(QWidget *parent = 0);
	~analyserQ();

private:
	Ui::analyserQClass ui;
	QPushButton *button;
	QLabel *label;
	QComboBox *liste;
	void loadPoI();
	QObject *rootObj;
	void addPoI(QVariant lat, QVariant longi, QVariant type);
	//void handle_result(HttpRequestWorker *worker);
	QJsonArray fileLoad();
private slots:
	void handle_result(HttpRequestWorker*);
	//QVariant addPoi(QVariant latitude, QVariant longitude, QVariant type);

};

#endif // ANALYSERQ_H
