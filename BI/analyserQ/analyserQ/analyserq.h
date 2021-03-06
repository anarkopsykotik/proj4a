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
	QString dbUrl;
	Ui::analyserQClass ui;
	QPushButton *button;
	QLabel *label;
	QLabel *labelMaxH;
	QLabel *labelMaxT;
	QLabel *labelMaxA;

	QComboBox *liste;
	void loadPoI();
	QObject *rootObj;
	void addPoI(QVariant lat, QVariant longi, QVariant type);
	//void handle_result(HttpRequestWorker *worker);
	QJsonArray fileLoad();
	void loadCombobox();
	void setHighestLabels(double highestAcc, double highestTemp, double highestHumidite);
private slots:
	void handle_result(HttpRequestWorker*);
	void fillCombobox(HttpRequestWorker*);
	//QVariant addPoi(QVariant latitude, QVariant longitude, QVariant type);

};

#endif // ANALYSERQ_H
