#include "analyserq.h"

//static QObject *rootObj;
QObject *rootObj;
QComboBox *liste;
QString dbUrl;

analyserQ::analyserQ(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	QSettings settings(QString("config.ini"), QSettings::IniFormat);
	dbUrl = settings.value("db/url", "http://localhost:32768").toString(); // settings.value() returns QVariant

	qDebug() << "db: "+dbUrl;
	QQuickView *view = new QQuickView();
	QWidget *map = new QWidget();
	QWidget *container = QWidget::createWindowContainer(view, map);
	container->setMinimumSize(400, 450);
	container->setMaximumSize(800, 800);
	container->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	container->setFocusPolicy(Qt::TabFocus	);
	view->setSource(QUrl("map.qml"));

	button = new QPushButton(tr("&Load..."), this);
	connect(button, &QAbstractButton::clicked, this, &analyserQ::loadPoI);


	label = new QLabel(tr("Choix Trajets."), this);
	labelMaxH = new QLabel(tr("Plus haute humidite du trajet"), this);
	labelMaxA = new QLabel(tr("Plus fort choc du trajet"), this);
	labelMaxT = new QLabel(tr("Plus haute temperature du trajet"), this);

	liste = new QComboBox;
	liste->setEditable(false);
	//liste->addItem("1");

	loadCombobox();

	QGridLayout *mainLayout = new QGridLayout;
	mainLayout->addWidget(button, 0, 0);
	mainLayout->addWidget(map, 1, 0);

	mainLayout->addWidget(label, 5, 0);
	mainLayout->addWidget(labelMaxH, 1, 4);
	mainLayout->addWidget(labelMaxA, 2, 4);
	mainLayout->addWidget(labelMaxT, 3, 4);

	mainLayout->addWidget(liste, 5,1);


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

void analyserQ::loadCombobox(){

	HttpRequestWorker *worker = new HttpRequestWorker(this);
	connect(worker, SIGNAL(on_execution_finished(HttpRequestWorker*)), this, SLOT(fillCombobox(HttpRequestWorker*)));

	QString url_str = dbUrl + "/_all_docs";
	HttpRequestInput input(url_str, "GET");
	//input.add_var("key1", "value1");
	//input.add_var("key2", "value2");
	worker->execute(&input);
}
	
void analyserQ::fillCombobox(HttpRequestWorker* worker){
	qDebug() << "handleResult";
	QString msg;
	if (worker->error_type == QNetworkReply::NoError) {
		// communication was successful
		msg = worker->response;
	}
	else {
		// an error occurred
		msg = "Error: " + worker->error_str;
		QMessageBox::information(this, "", msg);
		//return;
	}

	//QMessageBox::information(this, "", msg);
	qDebug() << msg;
	qDebug() << msg.toUtf8();
	QJsonParseError error;
	QJsonDocument document = QJsonDocument::fromJson(msg.toUtf8(), &error);

	if (!document.isObject()) {
		qDebug() << "Document is not an object"+error.errorString();
	}
	QJsonObject object = document.object();
	QJsonValue jsonValue = object.value("id");
	if (jsonValue.isUndefined()) {
		qDebug() << "Key id does not exist";
	}

	//QJsonObject object = document.object();
	//qDebug() << err->errorString();
	QJsonValue value = object.value("rows");
	qDebug() << "rowval" + value.toString();
	QJsonArray array = value.toArray();

	//array = fileLoad();
	//qDebug() << "file loaded";

	foreach(const QJsonValue & v, array)
	{
		liste->addItem(v.toObject().value("id").toString());
	}
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

	QString choosenTraject = liste->currentText();

	QString docId = "1";
	docId = choosenTraject;
	
	QString url_str = dbUrl + "/"+docId ;
	QMessageBox::information(this, "", url_str);
	HttpRequestInput input(url_str, "GET");
	//input.add_var("key1", "value1");
	//input.add_var("key2", "value2");
	worker->execute(&input);
	qDebug() << "LoadPoi";
}

void analyserQ::handle_result(HttpRequestWorker *worker) {
	QString msg;
	qDebug() << "handleResult";
	if (worker->error_type == QNetworkReply::NoError) {
		// communication was successful
		msg = worker->response;
	}
	else {
		// an error occurred
		msg = "Error: " + worker->error_str;
		QMessageBox::information(this, "", msg);
		//return;
	}

	//QMessageBox::information(this, "", msg);

	
	QJsonParseError error;
	QJsonDocument document = QJsonDocument::fromJson(msg.toUtf8(), &error);
	QJsonObject object = document.object();
	if (!document.isObject()) {
		qDebug() << "Document is not an object" + error.errorString();
	}
	QJsonValue value = object.value("DATA");
	QJsonArray array = value.toArray();
	double highestTemp = 0, highestAcc = 0, highestHumidite= 0;

	//array = fileLoad();
	//qDebug() << "file loaded";

	foreach(const QJsonValue & v, array)//				debug
	{
		qDebug() << v.toObject().value("id").toString(); 
		qDebug() << v.toObject().value("Latitude").toVariant();
		qDebug() << v.toObject().value("Longitude").toVariant();
		QVariant valLat = v.toObject().value("Latitude").toVariant();
		QVariant valLong = v.toObject().value("Longitude").toVariant();
		QVariant type = 3;//green

		double maxAcc = v.toObject().value("Accel_x").toDouble() + v.toObject().value("Accel_y").toDouble() + v.toObject().value("Accel_z").toDouble();
		highestAcc = maxAcc > highestAcc ? maxAcc : highestAcc;

		double humidite = v.toObject().value("Humidite").toDouble();
		highestHumidite = humidite > highestHumidite ? humidite : highestHumidite;
		if (humidite >= 100){
			type = 0;//blue
		}

		double temp = v.toObject().value("Temperature").toDouble();
		highestTemp = temp > highestTemp ? temp : highestTemp;
		if (temp >= 320){
			type = 1;//yellow
		}

		qDebug() << "max acc:" + QString::number(maxAcc);
		if (maxAcc >= 10)
		{
			type = 2;//red
		}
		qDebug() << "poi:" + valLat.toString() + valLong.toString() + type.toString();
		addPoI(valLat, valLong, type);
		
	}
	setHighestLabels(highestAcc, highestTemp, highestHumidite);
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

void analyserQ::setHighestLabels(double highestAcc, double highestTemp, double highestHumidite){
	labelMaxH->setText("Plus haute humidite du trajet :" + QString::number(highestHumidite));
	labelMaxA->setText("Plus fort choc du trajet :" + QString::number(highestAcc));
	labelMaxT->setText("Plus haute temperature du trajet :" + QString::number(highestTemp));
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
