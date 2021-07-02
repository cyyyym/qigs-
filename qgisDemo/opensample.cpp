#include "opensample.h"
#include"qgsmapcanvas.h"
#include"qgsfields.h"
#include"qvariant.h"
#include"qgsvectorlayer.h"
#include"qfiledialog.h"
#include"qmessagebox.h"
opensampleq::opensampleq(QgsMapCanvas*& canvas,QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	m_mapcanvas = canvas;
	//m_layerlist = m_mapcanvas->layers();
	fresh();
}

void opensampleq::fresh()
{
	foreach(QgsMapLayer * index, m_mapcanvas->layers())
	{
		if (index->type() == QgsMapLayerType::VectorLayer)
		{
			ui.comboBox->addItem(index->name());

		}
	}
}
void opensampleq::freshfield(QString name)
{
	ui.comboBox_2->clear();
	if (newlayer!=nullptr&&newlayer->name() == name)
	{
		QgsFields fields = newlayer->fields();
		for (int i = 0; i < fields.count(); i++)
		{
			if (fields[i].type() == QVariant::Int)
				ui.comboBox_2->addItem(fields[i].name());
		}
	}
	foreach(QgsMapLayer * index, m_mapcanvas->layers())
	{
		if (index->type() == QgsMapLayerType::VectorLayer&&index->name()==name)
		{
			QgsVectorLayer* a = qobject_cast<QgsVectorLayer*>(index);
			QgsFields fields = a->fields();
			for (int i = 0; i < fields.count(); i++)
			{
				if(fields[i].type()==QVariant::Int)
				    ui.comboBox_2->addItem(fields[i].name());
			}
		}
	}
}
void opensampleq::openfrom()
{
	QString fileName = QFileDialog::getOpenFileName(this, tr("Open shape file"), "", QStringLiteral("ESRI矢量文件(*.shp);;GeoPackage文件 (*.gpkg)"));
	QStringList temp = fileName.split('/');
	QString basename = temp.at(temp.size() - 1);
	newlayer = new QgsVectorLayer(fileName, basename, "ogr");

	if (!newlayer->isValid())
	{
		QMessageBox::critical(this, QStringLiteral("错误"), QStringLiteral("图层无效!\n") + fileName);
		return;
	}
	//m_layerlist.append(newlayer);
	ui.comboBox->addItem(newlayer->name());
	ui.comboBox->setCurrentText(newlayer->name());

}
void opensampleq::loadsample()
{

	if (ui.comboBox_2->count() != 0)
	{
		emit openshp(newlayer, ui.comboBox_2->currentText());
		close();
	}
	else
	{
		QMessageBox::critical(this, QStringLiteral("错误"), QStringLiteral("未选择有效字段或不存在有效字段"));
		return;
	}
	
}