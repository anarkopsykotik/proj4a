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
	//container->
	//ui->verticalLayout->addWidget(container);
	button = new QPushButton(tr("&Load..."), this);

	QGridLayout *mainLayout = new QGridLayout;
	mainLayout->addWidget(button, 0, 0);
	mainLayout->addWidget(map, 3, 0);
	setLayout(mainLayout);

}

analyserQ::~analyserQ()
{

}
