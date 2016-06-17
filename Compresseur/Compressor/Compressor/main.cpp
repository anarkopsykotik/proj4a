#include "compressor.h"
#include <QtWidgets/QApplication>


int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	Compressor w;
	w.show();
	return a.exec();
}
 