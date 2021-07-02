#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_qgisDemo.h"
#include"qgsmapcanvas.h"
#include"qgsvectorlayerproperties.h"
#include"qgsrasterlayerproperties.h"
#include"qgsmessagebar.h"
#include"qgsstylemanagerdialog.h"
#include"qgslayertreeview.h"
#include"qgsdockwidget.h"
#include <qgsadvanceddigitizingdockwidget.h>
#include"qgsmaptoolzoom.h"
#include"ui_qgsadvanceddigitizingdockwidgetbase.h"
#include"qgsmaptoolpan.h"
#include <qgslayertreeview.h>
#include <qgspalettedrasterrenderer.h>
#include <qgssymbol.h>
#include <qgslayertreemapcanvasbridge.h>
#include <qgscategorizedsymbolrenderer.h>
#include<qmessagebox.h>
class toolbar;
class right1;
class Moravecui;
class pipei;
class qgisDemo : public QMainWindow
{
    Q_OBJECT

public:
    qgisDemo(QWidget *parent = Q_NULLPTR);
	~qgisDemo();
	bool hadraster();
private slots:
	void loadvectorlayer(QString);
	void loadrasterlayer(QgsRasterLayer* path);
	void portBtnclick();
	void portBtn1click();
	void portBtn2click();
	void setting1(QgsMapLayer* layer);
	void deletelayer(QgsMapLayer* layer);
	void loadrasterlayer(QString path);
	void changecrs(QString);
private:
    Ui::qgisDemoClass ui;
private:
	QgsMapCanvas *m_MapCanvas= nullptr;//画布
	QList<QgsMapLayer*> m_layers;//存放所有添加的图层
	QgsStyleManagerDialog *m_StyleManger= nullptr;
	QgsVectorLayerProperties *m_VecProperty= nullptr;
	QgsRasterLayerProperties *m_RasterProperty= nullptr;
	QgsLayerTreeView *m_LayerTreeView= nullptr;
	QgsLayerTreeModel *m_model= nullptr;
	QgsDockWidget* mLayerTreeDock = nullptr;
	QgsAdvancedDigitizingDockWidget* m_dockwidegt = nullptr;
	toolbar* m_classi = nullptr;
	QgsMapToolZoom *zoomin = nullptr;
	QgsMapToolZoom* zoomout = nullptr;
	QgsMapToolPan* zoompan = nullptr;
	right1* m_rightmenu = nullptr;
	QgsPalettedRasterRenderer::ClassData classdata;
	QgsPalettedRasterRenderer* x = nullptr;
	QgsCategorizedSymbolRenderer* shpp = nullptr;
	QgsCategoryList shpsmdata;
	QgsLayerTreeMapCanvasBridge* m_layerTreeCanvasBridge=nullptr;
	Moravecui* mor = nullptr;
	pipei* pi = nullptr;
public slots:
	void addVecLayer();//添加矢量图层
	void addRasterLayer();//添加栅格图层
	void layerProperty();//图层属性
	void styleManager();//风格管理器
	void classification();
	void Moravec();
	void pipeiyingxiang();
	void setcolor(QgsMapLayer* layer)
	{
		QgsRasterLayer* rasterlayer = qobject_cast<QgsRasterLayer*>(layer);
		rasterlayer->dataProvider()->setNoDataValue(1, 255);//->setNoDataValue(255);
		if (rasterlayer->renderer() != x)
		{
			if (x == nullptr)
			{
				x = new QgsPalettedRasterRenderer(rasterlayer->dataProvider(), 1, classdata);
				rasterlayer->setRenderer(x);
				m_MapCanvas->refresh();
			}
			else
			{
				rasterlayer->setRenderer(x);
				m_MapCanvas->refresh();
			}
		}
		else
		{
			
			QMessageBox::information(NULL, QStringLiteral("信息"), QStringLiteral("已经分类显示"));
			return;
		}
	}
};
