#ifndef COMPRESSOR_H
#define COMPRESSOR_H

#include <QtWidgets/QMainWindow>
#include "ui_compressor.h"
#include <QFileDialog>
#include <QPushButton>
#include <QtWidgets>

class Compressor : public QWidget
{
	Q_OBJECT

public:
	Compressor(QWidget *parent = 0);
	~Compressor();

private:
	Ui::CompressorClass ui;
	QStringList fuseFiles();
	void showFiles(const QStringList &files);
	void displayFiles();
	QComboBox *createComboBox(const QString &text = QString());
	void createFilesTable();

	/*QComboBox *fileComboBox;
	QComboBox *textComboBox;*/
	QComboBox *directoryComboBox;
	QComboBox *directoryDestComboBox;

	/*QLabel *fileLabel;
	//QLabel *textLabel;*/
	QLabel *directoryLabel;
	QLabel *filesFoundLabel;
	QPushButton *browseButton;
	QPushButton *fuseButton;
	QPushButton *browseDestButton;

	QTableWidget *filesTable;

	QDir currentDir;

private slots:
	void browse();
	void browseDest();
	//void displayFiles();
	void openFileOfItem(int row, int column);
};

#endif // COMPRESSOR_H
