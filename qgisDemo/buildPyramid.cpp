#include "buildPyramid.h"
#include"qgsrasterdataprovider.h"
#include <qgsrasterpyramid.h>
#include <qgsrasterinterface.h>
buildPyramid::buildPyramid(QgsRasterDataProvider*& data,QWidget *parent)
	: QDialog(parent)
{
	rasterdata = data;
	ui.setupUi(this);
	ui.progressBar->hide();
	ui.comboBox->addItem(QStringLiteral("最邻近插值"));
}
void buildPyramid::run()
{
	ui.progressBar->show();
	QList < QgsRasterPyramid > py;
	py = rasterdata->buildPyramidList();
	for (int i1 = 0; i1 < py.size(); i1++)
	{
		py[i1].build = true;
	}
	QgsRasterBlockFeedback* a=NULL;
	QString aa = ui.comboBox->currentText();
	if (aa == QStringLiteral("最邻近插值"))	
	{
		ui.progressBar->setOrientation(Qt::Horizontal);  // 水平方向
		ui.progressBar->setMinimum(0);  // 最小值
		ui.progressBar->setMaximum(0);  // 最大值
		QCoreApplication::processEvents();
		QString a1 = rasterdata->buildPyramids(py);
		QCoreApplication::processEvents();

	}
	if (aa == "双线性插值")
	{
		ui.progressBar->setOrientation(Qt::Horizontal);  // 水平方向
		ui.progressBar->setMinimum(0);  // 最小值
		ui.progressBar->setMaximum(0);  // 最大值
		QCoreApplication::processEvents();
		QString a1 = rasterdata->buildPyramids(py);
		QCoreApplication::processEvents();
	}
	if (aa == "三次插值")
	{
		ui.progressBar->setOrientation(Qt::Horizontal);  // 水平方向
		ui.progressBar->setMinimum(0);  // 最小值
		ui.progressBar->setMaximum(0);  // 最大值
		QCoreApplication::processEvents();
		QString a1 = rasterdata->buildPyramids(py);
		QCoreApplication::processEvents();
	}

	close();
}
void buildPyramid::return1()
{
	close();
}
buildPyramid::~buildPyramid()
{
}
