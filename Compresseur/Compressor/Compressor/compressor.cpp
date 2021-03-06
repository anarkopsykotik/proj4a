#include "compressor.h"



Compressor::Compressor(QWidget *parent)
	: QWidget(parent)
{
	//QPushButton bouton("Salut les Z�ros, la forme ?", this);
	browseButton = new QPushButton(tr("&Browse..."), this);
	connect(browseButton, &QAbstractButton::clicked, this, &Compressor::browse);

	browseDestButton = new QPushButton(tr("&Destination..."), this);
	connect(browseDestButton, &QAbstractButton::clicked, this, &Compressor::browseDest);

	fuseButton = new QPushButton(tr("&Fuse"), this);
	connect(fuseButton, &QAbstractButton::clicked, this, &Compressor::fuseFiles);


//	fileComboBox = createComboBox(tr("*"));
//	textComboBox = createComboBox();
	directoryComboBox = createComboBox(QDir::currentPath());
	directoryDestComboBox = createComboBox(QDir::currentPath());;
	//fileLabel = new QLabel(tr("Named:"));
	//textLabel = new QLabel(tr("Containing text:"));
	directoryLabel = new QLabel(tr("Choosen directory:"));
	filesFoundLabel = new QLabel;

	createFilesTable();

	QGridLayout *mainLayout = new QGridLayout;
//	mainLayout->addWidget(fileLabel, 0, 0);
//	mainLayout->addWidget(fileComboBox, 0, 1, 1, 2);
//	mainLayout->addWidget(textLabel, 1, 0);
//	mainLayout->addWidget(textComboBox, 1, 1, 1, 2);
	mainLayout->addWidget(directoryLabel, 2, 0);//dir name
	mainLayout->addWidget(directoryComboBox, 2, 1);//dir choice
	mainLayout->addWidget(browseButton, 2, 2);//browse dir buttons

	mainLayout->addWidget(filesTable, 3, 0, 1, 3);//list file in selected dir
	mainLayout->addWidget(filesFoundLabel, 4, 0, 1, 2);
	mainLayout->addWidget(fuseButton, 4, 2);//fuse files

	//mainLayout->addWidget(directoryDestLabel, 2, 0);//dir name
	mainLayout->addWidget(directoryDestComboBox, 5, 1);//dest dir choice
	mainLayout->addWidget(browseDestButton, 5, 2);


	setLayout(mainLayout);

	setWindowTitle(tr("Fuse Files"));
	resize(700, 300);
	//QString fileName;
	//fileName = QFileDialog::getOpenFileName(this, tr("Choose folder"), "C:/", tr("Data files (*.json)"));

	//QPushButton bouton("Salut les Z�ros, la forme ?");
	//bouton.show();
	//ui.setupUi(this);
	
}

void Compressor::browse()
{
	QString directory = QFileDialog::getExistingDirectory(this,
		tr("Files to Fuse"), QDir::currentPath());

	if (!directory.isEmpty()) {
		if (directoryComboBox->findText(directory) == -1)
			directoryComboBox->addItem(directory);
		directoryComboBox->setCurrentIndex(directoryComboBox->findText(directory));
	}
	Compressor::displayFiles();
}

void Compressor::browseDest()
{
	QString directory = QFileDialog::getExistingDirectory(this,
		tr("Where to fuse"), QDir::currentPath());

	if (!directory.isEmpty()) {
		if (directoryDestComboBox->findText(directory) == -1)
			directoryDestComboBox->addItem(directory);
		directoryDestComboBox->setCurrentIndex(directoryDestComboBox->findText(directory));
	}
}

static void updateComboBox(QComboBox *comboBox)
{
	if (comboBox->findText(comboBox->currentText()) == -1)
		comboBox->addItem(comboBox->currentText());
}

void Compressor::displayFiles()
{
	filesTable->setRowCount(0);

	//QString fileName = "";// fileComboBox->currentText();
	//QString text = "";// textComboBox->currentText();
	QString path = directoryComboBox->currentText();

//	updateComboBox(fileComboBox);
//	updateComboBox(textComboBox);
	updateComboBox(directoryComboBox);

	currentDir = QDir(path);
	QStringList files;
	//if (fileName.isEmpty())
	//	fileName = "*";
	files = currentDir.entryList(QStringList("*"),
		QDir::Files | QDir::NoSymLinks, QDir::Time );

	//if (!text.isEmpty())
	//	files = fuseFiles(files, text);
	showFiles(files);
}

void Compressor::fuseFiles()
{
	QJsonArray entryList;


	QString destFileLoc = currentDir.absoluteFilePath ( directoryDestComboBox->currentText() );
	QFile destFile(destFileLoc + "/output.json");
	qDebug() << destFileLoc + "/output.json";

	QProgressDialog progressDialog(this);
	progressDialog.setCancelButtonText(tr("&Cancel"));
	
	progressDialog.setWindowTitle(tr("Fuse Files"));

	QList<QTableWidgetItem*> foundFiles;
	//directoryComboBox;
	foundFiles = filesTable->selectedItems();

	progressDialog.setRange(0, foundFiles.size());

	for (int i = 0; i < foundFiles.size(); ++i) {
		progressDialog.setValue(i);
		progressDialog.setLabelText(tr("Fusing file number %1 of %2...")
			.arg(i).arg(foundFiles.size()));
		qApp->processEvents();

		if (progressDialog.wasCanceled())
			break;

		QFile file(currentDir.absoluteFilePath(foundFiles[i]->text()));
		//QString val;
		QByteArray jsonData;

		if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {//reading file
			jsonData = file.readAll();
			file.close();
			QJsonDocument document = QJsonDocument::fromJson(jsonData);
			QJsonObject object = document.object();

			QJsonValue value = object.value("DATA");
			QJsonArray array = value.toArray();
			//foreach(const QJsonValue & v, array)//				debug
			//	qDebug() << v.toObject().value("id").toString();

			foreach(const QJsonValue & v, array)
				entryList.append(v);			

		}
	}

	QJsonDocument doc;
	QJsonObject rootObject;
	rootObject.insert("DATA", entryList);
	doc.setObject(rootObject);


	destFile.open(QFile::WriteOnly | QFile::Text | QFile::Truncate);
	destFile.write(doc.toJson());
	destFile.close();
	//QJsonArray Array = RootObject.value("array").toArray();

	/*foreach(const QJsonObject & v, entryList)		debug
		qDebug() << v.value("id").toString();*/
}

void Compressor::showFiles(const QStringList &files)
{
	for (int i = 0; i < files.size(); ++i) {
		QFile file(currentDir.absoluteFilePath(files[i]));
		qint64 size = QFileInfo(file).size();

		QTableWidgetItem *fileNameItem = new QTableWidgetItem(files[i]);
		fileNameItem->setFlags(fileNameItem->flags() ^ Qt::ItemIsEditable);
		QTableWidgetItem *sizeItem = new QTableWidgetItem(tr("%1 KB")
			.arg(int((size + 1023) / 1024)));
		sizeItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
		sizeItem->setFlags(sizeItem->flags() ^ Qt::ItemIsEditable);

		int row = filesTable->rowCount();
		filesTable->insertRow(row);
		filesTable->setItem(row, 0, fileNameItem);
		filesTable->setItem(row, 1, sizeItem);
	}
	filesFoundLabel->setText(tr("%1 file(s) found").arg(files.size()) +
		(" (Double click on a file to open it)"));
	filesFoundLabel->setWordWrap(true);
}

QComboBox *Compressor::createComboBox(const QString &text)
{
	QComboBox *comboBox = new QComboBox;
	comboBox->setEditable(true);
	comboBox->addItem(text);
	comboBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
	return comboBox;
}

void Compressor::createFilesTable()
{
	filesTable = new QTableWidget(0, 2);
	filesTable->setSelectionBehavior(QAbstractItemView::SelectRows);

	QStringList labels;
	labels << tr("Filename") << tr("Size");
	filesTable->setHorizontalHeaderLabels(labels);
	filesTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
	filesTable->verticalHeader()->hide();
	filesTable->setShowGrid(false);

	connect(filesTable, &QTableWidget::cellActivated, this, &Compressor::openFileOfItem);

}


void Compressor::openFileOfItem(int row, int /* column */)
{
	QTableWidgetItem *item = filesTable->item(row, 0);

	QDesktopServices::openUrl(QUrl::fromLocalFile(currentDir.absoluteFilePath(item->text())));
}

Compressor::~Compressor()
{

}
