#include "analyserq.h"
#include <QtWidgets/QApplication>
#include <QQuickView>
#include <QQuickItem>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	analyserQ w;

	w.show();
	return a.exec();
}

//
//