#pragma once

#include <QWidget>
#include "ui_toolbar.h"
//qgis
#include"qgsadvanceddigitizingdockwidget.h"
#include"qgsmapcanvas.h"
#include"qgsfields.h"
#include"qstandarditemmodel.h"
#include<map> 

using namespace std;

class classmangement;
class majority;
class addfeature;
class opensampleq;

class toolbar : public QWidget
{
	Q_OBJECT

public:
	toolbar(QgsMapCanvas* canvas, QgsAdvancedDigitizingDockWidget* dockwidget,QList<QgsMapLayer*> mapCanvasLayerSet1,QWidget *parent = Q_NULLPTR);
	~toolbar()
	{
		if (m_classmangement != nullptr)
		{
			delete m_classmangement;
			m_classmangement = nullptr;
		}
		if (m_majority != nullptr)
		{
			delete m_majority;
			m_majority = nullptr;
		}
		if (m_cadDockWidget != nullptr)
		{
			delete m_cadDockWidget;
			m_cadDockWidget = nullptr;
		}
		if (m_fields!=nullptr)
		{
			m_fields->clear();
			delete m_fields;
			m_fields = nullptr;
		}
		if (m_opensample != nullptr)
		{
			delete m_opensample;
			m_opensample = nullptr;
		}
		if (m_mapcanvas != nullptr)
		{
			delete m_mapcanvas;
			m_mapcanvas = nullptr;
		}
		//m_mapcanvas = nullptr;
	}
	void getinformation(QgsMapCanvas* canvas, QList<QgsMapLayer*> mapCanvasLayerSet1);
	//类别控制
	void freshtable();
signals:
	void deletfeature(QgsFeature&);
	void output();
	void  classificationactive();
	void youhua();
	void loadvector(QString);
	void loadraster(QString);
	void layerchanged(QString);
	void clearall();
	void selectfeature(QgsFeature&);
	void cancleselect();
	//void loadvector(QString);
private:
	Ui::toolbar ui;
	QgsMapCanvas* m_mapcanvas=nullptr;
	QgsAdvancedDigitizingDockWidget* m_cadDockWidget=nullptr;
	addfeature * m_addfeature=nullptr;
	QList<QgsMapLayer*> mapCanvasLayerSet;
	QgsFields* m_fields=nullptr;
	QgsFeatureList m_list;
	opensampleq* m_opensample = nullptr;
	classmangement* m_classmangement = nullptr;
	majority* m_majority = nullptr;
	map<int, QColor> colortable;
	int i = 1;
	int i0 = 0;//当i0为1时不进行操作；当i0=0是操作；
private slots:
	void layerchange(QString);
	void click_pushButton_2();
	void click_deletButoon();
	void click_outputButton();
	void textchanged(QString);
	void write(QgsFeature&);
	void error(int i);
	void pointclassify();
	void starclassification();
	void opensample();
	void clearsample();
	void xuanzhong(QModelIndex);
	void setprogressvalue(int);
	void setprogressmessage(QString&);
	void setprogressfresh();
	void getloadshp(QString path)
	{
		emit loadvector(path);
	}
	void getrastershp(QString path)
	{
		emit loadraster(path);
	}
	void hideprogress(int i)
	{
		if (i == 2)
		{
			ui.progressBar->show();
			ui.textBrowser->show();
		}
		else if (i == 0)
		{
			ui.progressBar->hide();
			ui.textBrowser->hide();
		}
	}
};
