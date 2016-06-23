#include "analyserq.h"


analyserQ::analyserQ(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	QQuickView *view = new QQuickView();
	QWidget *map = new QWidget();
	QWidget *container = QWidget::createWindowContainer(view, map);
	container->setMinimumSize(200, 200);
	container->setMaximumSize(200, 200);
	container->setFocusPolicy(Qt::TabFocus	);
	view->setSource(QUrl("map.qml"));
	
	/*QQuickView *view2 = new QQuickView();
	QWidget *poi = new QWidget();
	QWidget *container2 = QWidget::createWindowContainer(view2, poi);
	view2->setSource(QUrl("poi.qml"));
	container2->setParent(map);*/
	
	//container->
	//ui->verticalLayout->addWidget(container);
	button = new QPushButton(tr("&Load..."), this);
	//	connect(browseButton, &QAbstractButton::clicked, this, &Compressor::browse);

	QGridLayout *mainLayout = new QGridLayout;
	mainLayout->addWidget(button, 0, 0);
	mainLayout->addWidget(map, 3, 0);
	setLayout(mainLayout);
	
	QVariant returnedValue;
	QVariant latitude = 5;
	QVariant longitude =  6;
	QVariant type = 0;

	QObject *object1 = view->rootObject();
	QMetaObject::invokeMethod(object1, "addPoi",
		Q_RETURN_ARG(QVariant, returnedValue),
		Q_ARG(QVariant, latitude), Q_ARG(QVariant, longitude), Q_ARG(QVariant, type));
	qDebug() << "QML function returned:" << returnedValue.toString();

	/*QQmlEngine engine;
	QQmlComponent component(&engine, "map.qml");
	QObject *object = component.create();
	QMetaObject::invokeMethod(object, "addPoi",
		Q_RETURN_ARG(QVariant, returnedValue),
		Q_ARG(QVariant, latitude), Q_ARG(QVariant, longitude), Q_ARG(QVariant, type));
	qDebug() << "QML function2 returned:" << returnedValue.toString();*/

	//http://stackoverflow.com/questions/26704621/how-to-create-a-qqmlcomponent-from-c-at-runtime?lq=1


	
	//QQuickItem *item = qobject_cast<QQuickItem*>(map);
	//item->
	//item->setWidth(500);

	/*QMetaObject::invokeMethod(container, "myQmlFunction",
		Q_RETURN_ARG(QVariant, returnedValue),
		Q_ARG(QVariant, msg));*/


	/*
	QQmlEngine engine;
	QQmlComponent component(&engine, "MyItem.qml");
	QObject *object = component.create();

	QVariant returnedValue;
	QVariant msg = "Hello from C++";
	QMetaObject::invokeMethod(object, "myQmlFunction",
		Q_RETURN_ARG(QVariant, returnedValue),
		Q_ARG(QVariant, msg));

	qDebug() << "QML function returned:" << returnedValue.toString();*/

	/*
	// Using QQmlComponent
		QQmlEngine engine;
		QQmlComponent component(&engine, QUrl::fromLocalFile("MyItem.qml"));
		QObject *object = component.create();*/
}

QString* loadPoI(int nb){

}

void addPoI(QObject* map, QVariant latitude, QVariant longitude, QVariant type)
{
	QVariant returnedValue;

	QMetaObject::invokeMethod(map, "addPoi",
		Q_RETURN_ARG(QVariant, returnedValue),
		Q_ARG(QVariant, latitude), Q_ARG(QVariant, longitude), Q_ARG(QVariant, type));
	qDebug() << "QML function returned:" << returnedValue.toString();
}

analyserQ::~analyserQ()
{

}
