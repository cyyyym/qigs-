#include "qgisDemo.h"
#include <QtWidgets/QApplication>
#include"qgsapplication.h"

int main(int argc, char *argv[])
{
    //QApplication a(argc, argv);
	
	QgsApplication app(argc, argv, true);
	QTranslator qgistor(nullptr);
	if (qgistor.load(":/new/prefix1/qgis_zh-Hans.qm"))
	{
		app.installTranslator(&qgistor);
	}
	QgsApplication::setTranslation("zh-Hans");
	QgsApplication::init();
	QgsApplication::initQgis();
	QgsApplication::setPrefixPath("../../../devenv/", true);
	QgsApplication::processEvents();
    qgisDemo w;
    w.showMaximized();
    return app.exec();
}
