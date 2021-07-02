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
	ui.comboBox->addItem(QStringLiteral("���ڽ���ֵ"));
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
	if (aa == QStringLiteral("���ڽ���ֵ"))	
	{
		ui.progressBar->setOrientation(Qt::Horizontal);  // ˮƽ����
		ui.progressBar->setMinimum(0);  // ��Сֵ
		ui.progressBar->setMaximum(0);  // ���ֵ
		QCoreApplication::processEvents();
		QString a1 = rasterdata->buildPyramids(py);
		QCoreApplication::processEvents();

	}
	if (aa == "˫���Բ�ֵ")
	{
		ui.progressBar->setOrientation(Qt::Horizontal);  // ˮƽ����
		ui.progressBar->setMinimum(0);  // ��Сֵ
		ui.progressBar->setMaximum(0);  // ���ֵ
		QCoreApplication::processEvents();
		QString a1 = rasterdata->buildPyramids(py);
		QCoreApplication::processEvents();
	}
	if (aa == "���β�ֵ")
	{
		ui.progressBar->setOrientation(Qt::Horizontal);  // ˮƽ����
		ui.progressBar->setMinimum(0);  // ��Сֵ
		ui.progressBar->setMaximum(0);  // ���ֵ
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
