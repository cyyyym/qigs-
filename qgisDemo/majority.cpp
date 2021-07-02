#include "majority.h"
#include"qgsmapcanvas.h"
#include"qtextcodec.h"
#include"qfiledialog.h"
#include"qmessagebox.h"
majority::majority(QgsMapCanvas*& canvas,QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	m_canvas = canvas;
	//m_layerlist = m_canvas->layers();
	freshui1();
}
void majority::fresh(QgsMapCanvas*& can)
{
	m_canvas = can;
}
void majority::freshui1()
{
	ui.comboBox->clear();
	foreach(QgsMapLayer * index, m_canvas->layers())
	{
		if (index->type() == QgsMapLayerType::RasterLayer)
		{
			ui.comboBox->addItem(index->name());
		}
	}
	ui.lineEdit_2->setText("5");
	ui.lineEdit_3->setText("5");
	ui.lineEdit_4->setText("1");
}
void majority::savepath()
{
	QString fileName = QFileDialog::getSaveFileName(0, tr("save"), "", tr(" (*.tiff)"));
	if (fileName.isNull())
	{
		QMessageBox::information(NULL, "Information", QString::fromLocal8Bit("文件创建失败"), QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
		return;
	}
	else
	{
		ui.lineEdit_5->setText(fileName);
	}
}
void majority::returntext()
{
	this->close();
	name.clear();
	name.append(ui.comboBox->currentText());
	name.append(ui.lineEdit_2->text());
	name.append(ui.lineEdit_3->text());
	name.append(ui.lineEdit_4->text());
	name.append(ui.lineEdit_5->text());
	emit start(name);

}