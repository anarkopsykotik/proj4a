#ifndef ANALYSERQ_H
#define ANALYSERQ_H

#include <QtWidgets/QWidget>
#include <QtWidgets>
#include "ui_analyserq.h"
#include <QQuickView>
#include <QQuickItem>

class analyserQ : public QWidget
{
	Q_OBJECT

public:
	analyserQ(QWidget *parent = 0);
	~analyserQ();

private:
	Ui::analyserQClass ui;
	QPushButton *button;

};

#endif // ANALYSERQ_H
