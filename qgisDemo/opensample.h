#pragma once

#include <QWidget>
#include "ui_opensample.h"
#include"qgsmapcanvas.h"
#include"qgsfields.h"
#include"qgsvectorlayer.h"

class opensampleq : public QWidget
{
	Q_OBJECT

public:
	opensampleq(QgsMapCanvas* &canvas,QWidget *parent = Q_NULLPTR);
	~opensampleq()
	{
		if (newlayer != nullptr)
		{
			delete newlayer;
			newlayer = nullptr;
		}
	}
	void fresh();
signals:
	void openshp(QgsVectorLayer*,QString);
private slots:
	void openfrom();
	void loadsample();
	void freshfield(QString);
private:
	Ui::opensample ui;
	QgsMapCanvas* m_mapcanvas=nullptr;
	//QList<QgsMapLayer*> m_layerlist;
	QgsVectorLayer* newlayer=nullptr;
};
