#include "analyserq.h"

//static QObject *rootObj;
QObject *rootObj;

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

	button = new QPushButton(tr("&Load..."), this);
	connect(button, &QAbstractButton::clicked, this, &analyserQ::loadPoI);


	label = new QLabel(tr("Choix Trajets."), this);
	liste = new QComboBox;
	liste->setEditable(false);
	liste->addItem("1");

	QGridLayout *mainLayout = new QGridLayout;
	mainLayout->addWidget(button, 0, 0);
	mainLayout->addWidget(map, 3, 0);

	mainLayout->addWidget(label, 4, 0);//dir name
	mainLayout->addWidget(liste, 4,1);//dir name


	setLayout(mainLayout);
	rootObj = view->rootObject();

	QVariant returnedValue;
	QVariant lat = 59.9485;
	QVariant longi = 10.7686;
	QVariant type = 0;

	//QObject *object1 = view->rootObject();

	/*
	QMetaObject::invokeMethod(rootObj, "addPoi",
		Q_RETURN_ARG(QVariant, returnedValue),
		Q_ARG(QVariant, lat), Q_ARG(QVariant, longi), Q_ARG(QVariant, type));
	qDebug() << "QML function returned:" << returnedValue.toString();

	longi = 10.8686;
	type = 1;
	QMetaObject::invokeMethod(rootObj, "addPoi",
		Q_RETURN_ARG(QVariant, returnedValue),
		Q_ARG(QVariant, lat), Q_ARG(QVariant, longi), Q_ARG(QVariant, type));*/

	
}

void analyserQ::addPoI(QVariant latitude, QVariant longitude, QVariant type)
{
	QVariant returnedValue;

	QMetaObject::invokeMethod(this->rootObj, "addPoi",
		Q_RETURN_ARG(QVariant, returnedValue),
		Q_ARG(QVariant, latitude), Q_ARG(QVariant, longitude), Q_ARG(QVariant, type));
	qDebug() << "QML function returned:" << returnedValue.toString();
}

void analyserQ::loadPoI() {
	QString msg;
	HttpRequestWorker *worker = new HttpRequestWorker(this);
	connect(worker, SIGNAL(on_execution_finished(HttpRequestWorker*)), this, SLOT(handle_result(HttpRequestWorker*)));
	QString url_str = "http://www.example.com/path/to/page.php" ;
	HttpRequestInput input(url_str, "GET");
	input.add_var("key1", "value1");
	input.add_var("key2", "value2");
	worker->execute(&input);
	qDebug() << "LoadPoi";
}

void analyserQ::handle_result(HttpRequestWorker *worker) {
	QString msg;
	qDebug() << "handleResult";
	if (worker->error_type == QNetworkReply::NoError) {
		// communication was successful
		msg = "Success - Response: " + worker->response;
	}
	else {
		// an error occurred
		msg = "Error: " + worker->error_str;
		QMessageBox::information(this, "", msg);
		//return;
	}

	QMessageBox::information(this, "", msg);

	
	
	QJsonDocument document = QJsonDocument::fromJson(msg.toUtf8());
	QJsonObject object = document.object();

	QJsonValue value = object.value("DATA");
	QJsonArray array = value.toArray();

	array = fileLoad();
	qDebug() << "file loaded";

	foreach(const QJsonValue & v, array)//				debug
	{
		qDebug() << v.toObject().value("id").toString(); 
		qDebug() << v.toObject().value("Latitude").toVariant();
		qDebug() << v.toObject().value("Longitude").toVariant();
		QVariant valLat = v.toObject().value("Latitude").toVariant();
		QVariant valLong = v.toObject().value("Longitude").toVariant();
		QVariant type = 1;

		if (v.toObject().value("Accel_x").toDouble() >= 10)
		{
			type = 2;
		}
		qDebug() << "poi:" + valLat.toString() + valLong.toString() + type.toString();
		addPoI(valLat, valLong, type);
	}

	/*for each (QJsonValue val in array)
	{

		qDebug() << "val:"+val.toString();
		QVariant valLat = val.toObject()["latitude"];
		QVariant valLong = val.toObject()["longitude"];
		QVariant type = 1;
		if (val.toObject()["choc"].toDouble() >= 10 )
		{
			type = 2;
		}
		qDebug() << "poi:" + valLat.toString() + valLong.toString() + type.toString();
		addPoI(valLat, valLong, type);
	}*/
}

QJsonArray analyserQ::fileLoad(){
	QFile file("C:\\Users\\Ugo\\Documents\\GitHub\\proj4a\\Compresseur\\output.json");
	//QString val;
	QByteArray jsonData;
	QJsonArray jarray;
	if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {//reading file
		jsonData = file.readAll();
		file.close();
		QJsonDocument document = QJsonDocument::fromJson(jsonData);
		QJsonObject object = document.object();

		QJsonValue value = object.value("DATA");
		jarray = value.toArray();
	}
	return jarray;
}

analyserQ::~analyserQ()
{

}
