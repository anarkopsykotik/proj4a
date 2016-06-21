#include "analyserq.h"
#include <QtWidgets/QApplication>
#include <QQuickView>
#include <QQuickItem>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	analyserQ w;

	/*QQuickView view;
	view.setSource(QUrl::fromLocalFile("map.qml"));
	view.show();
	QObject *object = view.rootObject();
	QQuickItem *item = qobject_cast<QQuickItem*>(object);
	item->setWidth(500);

	QGridLayout *mainLayout = new QGridLayout;*/
	//mainLayout->addWidget(object, 0, 0);

	w.show();
	return a.exec();
}
