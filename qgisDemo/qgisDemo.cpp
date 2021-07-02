#include "qgisDemo.h"
#include"qfiledialog.h"
#include"qmessagebox.h"
#include"qgsvectorlayer.h"
#include"qgsrasterlayer.h"
#include"qgsdockwidget.h"
#include"qboxlayout.h"
#include"qgsdockwidget.h"
#include"toolbar.h"
#include"qgsmaptooldigitizefeature.h "
#include"qgsadvanceddigitizingdockwidget.h"
#include"ui_qgsadvanceddigitizingdockwidgetbase.h"
#include"qgsmaptoolzoom.h"
#include <qgsrasterlayerproperties.h>
#include <qgslayertreeview.h>
#include <qgsvectorlayer.h>
#include <qgsrasterlayer.h>
#include <qgsproject.h>
#include <qgslayertreemodel.h>
#include <qgsapplication.h>
#include <qgslayertreelayer.h>
#include <qgslayertreegroup.h>
#include <qgslayertreeregistrybridge.h>
#include <qgslayertreeviewdefaultactions.h>
#include"right.h"
#include<Qcolor>
#include <qgssymbol.h>
#include <qgscategorizedsymbolrenderer.h>
#include <qgsmarkersymbollayer.h>
#include<string>
#include<stdio.h>
#include<stdlib.h>
#include<Windows.h>
#include<math.h>
#include<buildPyramid.h>
#include"Moravecui.h"
#include"pipei.h"
qgisDemo::qgisDemo(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
	if (m_MapCanvas == NULL)
	{
		m_MapCanvas = new QgsMapCanvas();
	}
	//ui.widget_mapCavans->set
	//this->setCentralWidget(m_MapCanvas);
	/*QDockWidget* debugging = new QDockWidget("调试", this);//构建停靠窗口，指定父类
	debugging->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetClosable);//设置停靠窗口特性，可移动，可关闭
	debugging->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);//设置可停靠区域为主窗口左边和右边
	debugging->setMinimumWidth(200);*/
	mLayerTreeDock = new QgsDockWidget(QStringLiteral("图层"), this);
	mLayerTreeDock->setObjectName(QStringLiteral("图层"));
	mLayerTreeDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	m_MapCanvas->enableAntiAliasing(true);
	m_MapCanvas->setCanvasColor(QColor(255, 255,255));//设置背景颜色
	m_MapCanvas->setVisible(true);//设置是否可见
	m_LayerTreeView = new QgsLayerTreeView(this);
	m_model = new QgsLayerTreeModel(QgsProject::instance()->layerTreeRoot(), this);
	m_model->setFlag(QgsLayerTreeModel::AllowNodeRename);
	m_model->setFlag(QgsLayerTreeModel::AllowNodeReorder);
	m_model->setFlag(QgsLayerTreeModel::AllowNodeChangeVisibility);
	m_model->setFlag(QgsLayerTreeModel::ShowLegendAsTree);
	m_model->setAutoCollapseLegendNodes(10);

	m_rightmenu = new right1(m_LayerTreeView, m_MapCanvas);

	m_LayerTreeView->setModel(m_model);
	m_LayerTreeView->setMenuProvider(m_rightmenu);
	m_layerTreeCanvasBridge = new QgsLayerTreeMapCanvasBridge(QgsProject::instance()->layerTreeRoot(), m_MapCanvas, this);
	connect(QgsProject::instance(), SIGNAL(writeProject(QDomDocument&)),
		m_layerTreeCanvasBridge, SLOT(writeProject(QDomDocument&)));
	connect(QgsProject::instance(), SIGNAL(readProject(QDomDocument)),
		m_layerTreeCanvasBridge, SLOT(readProject(QDomDocument)));
	
	QVBoxLayout *vboxLayout = new QVBoxLayout;
	vboxLayout->setMargin(0);
	vboxLayout->setContentsMargins(0, 0, 0, 0);
	vboxLayout->setSpacing(0); 
	vboxLayout->addWidget(m_LayerTreeView);
	QWidget* w = new QWidget;
	w->setLayout(vboxLayout);
	mLayerTreeDock->setWidget(w);
	addDockWidget(Qt::LeftDockWidgetArea, mLayerTreeDock);
	//ui.widget_treeView->setLayout(vboxLayout);
	QVBoxLayout *vboxLayout1 = new QVBoxLayout;
	vboxLayout1->setMargin(0);
	vboxLayout1->setContentsMargins(0, 0, 0, 0);
	vboxLayout1->setSpacing(0);
	vboxLayout1->addWidget(m_MapCanvas);
	ui.widget_mapCavans->setLayout(vboxLayout1);
	
	m_StyleManger = nullptr;
	m_VecProperty = nullptr;
	m_RasterProperty = nullptr;
	QTextCodec* codec = QTextCodec::codecForName("GBK");//修改这两行
	QColor rgb1;
	rgb1.setRgb(237, 189, 101, 255);
	QgsPalettedRasterRenderer::Class a(0, rgb1, codec->toUnicode("裸地"));
	QgsFillSymbol*	aa=new QgsFillSymbol();
	aa->setColor(rgb1);
	QVariant value(0);
	QgsRendererCategory aaa;
	aaa.setValue(value);
	aaa.setSymbol(aa);
	aaa.setLabel(codec->toUnicode("裸地"));
	shpsmdata.append(aaa);
	classdata.append(a);
	rgb1.setRgb(192, 192, 192, 255);
	QgsPalettedRasterRenderer::Class a1(6, rgb1, codec->toUnicode("建筑"));
	QgsFillSymbol* aa1 = new QgsFillSymbol();
	aa1->setColor(rgb1);
	value.setValue(6);
	aaa.setValue(value);
	aaa.setSymbol(aa1);
	aaa.setLabel(codec->toUnicode("建筑"));
	shpsmdata.append(aaa);
	classdata.append(a1);
	rgb1.setRgb(0, 0, 255, 255);
	QgsPalettedRasterRenderer::Class a2(9, rgb1, codec->toUnicode("水体"));
	QgsFillSymbol* aa2 = new QgsFillSymbol();
	aa2->setColor(rgb1);
	value = 9;
	aaa.setValue(value);
	aaa.setSymbol(aa2);
	aaa.setLabel(codec->toUnicode("水体"));
	shpsmdata.append(aaa);
	classdata.append(a2);
	rgb1.setRgb(107, 142, 35, 255);
	QgsPalettedRasterRenderer::Class a3(12, rgb1, codec->toUnicode("草地"));
	QgsFillSymbol* aa3 = new QgsFillSymbol();
	aa3->setColor(rgb1);
	value = 12;
	aaa.setValue(value);
	aaa.setSymbol(aa3);
	aaa.setLabel(codec->toUnicode("草地"));
	shpsmdata.append(aaa);
	classdata.append(a3);
	rgb1.setRgb(34, 139, 34, 255);
	QgsPalettedRasterRenderer::Class a4(13, rgb1, codec->toUnicode("桉树"));
	QgsFillSymbol* aa4 = new QgsFillSymbol();
	aa4->setColor(rgb1);
	value = 13;
	aaa.setValue(value);
	aaa.setSymbol(aa4);
	aaa.setLabel(codec->toUnicode("桉树"));
	shpsmdata.append(aaa);
	classdata.append(a4);
	rgb1.setRgb(0, 255, 0, 255);
	QgsPalettedRasterRenderer::Class a5(14, rgb1, codec->toUnicode("竹子"));
	QgsFillSymbol* aa5 = new QgsFillSymbol();
	aa5->setColor(rgb1);
	value = 14;
	aaa.setValue(value);
	aaa.setSymbol(aa5);
	aaa.setLabel(codec->toUnicode("竹子"));
	shpsmdata.append(aaa);
	classdata.append(a5);
	rgb1.setRgb(255, 0, 0, 255);
	QgsPalettedRasterRenderer::Class a6(15, rgb1, codec->toUnicode("其他林地"));
	QgsFillSymbol* aa6 = new QgsFillSymbol();
	aa6->setColor(rgb1);
	value = "15";
	aaa.setValue(value);
	aaa.setSymbol(aa6);
	aaa.setLabel(codec->toUnicode("其他林地"));
	shpsmdata.append(aaa);
	classdata.append(a6);

	shpp = new QgsCategorizedSymbolRenderer("id", shpsmdata);
	QPushButton* portBtn = new QPushButton(this);
	portBtn->setIcon(QIcon(tr(":/new/prefix1/ZoomContinuousTool32.png")));	//按钮添加图片
	portBtn->setToolTip(codec->toUnicode("拖动"));			//鼠标移动到按钮上时显示提示 aaaa
	ui.mainToolBar->addWidget(portBtn);//根据工具栏添加方式选择添加按钮方式
	ui.mainToolBar->addSeparator(); //添加分 隔符
	connect(portBtn, SIGNAL(clicked()), this, SLOT(portBtnclick()));
	QPushButton* portBtn1 = new QPushButton(this);
	portBtn1->setIcon(QIcon(":/new/prefix1/ZoomInTool_B_32.png"));	//按钮添加图片
	portBtn1->setToolTip(codec->toUnicode("放大"));			//鼠标移动到按钮上时显示提示 aaaa
	ui.mainToolBar->addWidget(portBtn1);//根据工具栏添加方式选择添加按钮方式
	ui.mainToolBar->addSeparator(); //添加分 隔符
	connect(portBtn1, SIGNAL(clicked()), this, SLOT(portBtn1click()));
	QPushButton* portBtn2 = new QPushButton(this);
	portBtn2->setIcon(QIcon(":/new/prefix1/ZoomOutTool_B_32.png"));	//按钮添加图片
	portBtn2->setToolTip(codec->toUnicode("缩小"));			//鼠标移动到按钮上时显示提示 aaaa
	ui.mainToolBar->addWidget(portBtn2);//根据工具栏添加方式选择添加按钮方式
	ui.mainToolBar->addSeparator(); //添加分 隔符
	connect(portBtn2, SIGNAL(clicked()), this, SLOT(portBtn2click()));
	connect(ui.actionaddVectorLayer, &QAction::triggered, this, &qgisDemo::addVecLayer);
	connect(ui.actionaddRasterLaer, &QAction::triggered, this, &qgisDemo::addRasterLayer);
	connect(ui.actionlayerProperty, &QAction::triggered, this, &qgisDemo::layerProperty);
	connect(ui.actionstyleManager, &QAction::triggered, this, &qgisDemo::styleManager);
	connect(ui.actionMoravec, &QAction::triggered, this, &qgisDemo::Moravec);
	connect(ui.actionpipei, &QAction::triggered, this, &qgisDemo::pipeiyingxiang);
	connect(ui.action, &QAction::triggered, this, &qgisDemo::classification);
	connect(m_rightmenu, SIGNAL(setting(QgsMapLayer*)), this, SLOT(setting1(QgsMapLayer*)));
	connect(m_rightmenu, SIGNAL(deletelayers(QgsMapLayer*)), this, SLOT(deletelayer(QgsMapLayer*)));
	connect(m_rightmenu, SIGNAL(setlayer(QgsMapLayer*)), this, SLOT(setcolor(QgsMapLayer*)));
}

qgisDemo::~qgisDemo()
{
	/*if (m_layers.size() >= 0)
	{
		for (QList<QgsMapLayer*>::iterator iter = m_layers.begin(); iter != m_layers.end(); iter++)
		{
			QgsMapLayer* layer = *iter;
			m_layers.removeOne(layer);
			if (layer != nullptr)
			{
				delete layer;
				layer = nullptr;
			}
		}
	}*/

	if (m_layerTreeCanvasBridge != nullptr)
	{
		delete m_layerTreeCanvasBridge;
		m_layerTreeCanvasBridge = nullptr;
	}
	if (shpp != nullptr)
	{
		delete shpp;
		shpp = nullptr;
	}
	if (mor != nullptr)
	{
		delete mor;
	}
	if (x != nullptr)
	{
		delete x;
		x = nullptr;
	}

	if (m_classi != nullptr)
	{
		delete m_classi;
		m_classi = nullptr;
	}
	
	if (m_model != nullptr)
	{
		delete m_model;
		m_model = nullptr;
	}
	if (m_LayerTreeView != nullptr)
	{
		delete m_LayerTreeView;
		m_LayerTreeView = nullptr;
	}
	if (m_StyleManger != nullptr)
	{
		delete m_StyleManger;
		m_StyleManger = nullptr;
	}
	if (m_VecProperty != nullptr)
	{
		delete m_VecProperty;
		m_VecProperty = nullptr;
	}
	if (m_RasterProperty != nullptr)
	{
		delete m_RasterProperty;
		m_RasterProperty = nullptr;
	}
	if (mLayerTreeDock != nullptr)
	{
		delete mLayerTreeDock;
		mLayerTreeDock = nullptr;
	}
	//if (m_RasterProperty != NULL)
	//{
	//	delete m_RasterProperty;
	//	m_RasterProperty = NULL;
	//}
	//if (m_VecProperty != NULL)
	//{
	//	delete m_VecProperty;
	//	m_VecProperty = NULL;
	//}
	if (m_dockwidegt != nullptr)
	{
		delete m_dockwidegt;
		m_dockwidegt = nullptr;
	}

	if (m_MapCanvas != nullptr)
	{
		//delete m_MapCanvas;
		m_MapCanvas = nullptr;
	}
}

void qgisDemo::addRasterLayer()
{
	QString filename = QFileDialog::getOpenFileName(this, QStringLiteral("打开栅格图像"), "", "(*.tiff);;(*.tif)");
	QStringList temp = filename.split(QDir::separator());
	QString basename = temp.at(temp.size() - 1);
	QgsRasterLayer::LayerOptions layerOption;
	layerOption.loadDefaultStyle = false;
	QgsRasterLayer* rasterLayer1 = new QgsRasterLayer(filename, basename, "gdal", layerOption);
	if (!rasterLayer1->isValid())
	{
		QMessageBox::critical(this, QStringLiteral("错误"), QStringLiteral("图层无效!\n") + filename);
		return;
	}
	QgsRasterDataProvider* data = rasterLayer1->dataProvider();
	if ((data->xSize() > 1000 || data->ySize() > 1000) && data->hasPyramids() == false)
	{
		buildPyramid* py = new buildPyramid(data);
		py->exec();
		delete py;
	}
	delete rasterLayer1;
	QgsRasterLayer* rasterLayer = new QgsRasterLayer(filename, basename, "gdal", layerOption);
	m_MapCanvas->setExtent(rasterLayer->extent());
	m_layers.prepend(rasterLayer);
	m_MapCanvas->setLayers(m_layers);
	m_MapCanvas->refresh();
	m_MapCanvas->setCurrentLayer(rasterLayer);
    QgsProject::instance()->addMapLayer(rasterLayer);
}
void qgisDemo::layerProperty()
{
	if (m_MapCanvas->currentLayer() != nullptr)
	{
		if (m_MapCanvas->currentLayer()->type() == QgsMapLayerType::RasterLayer)
		{
			if (m_RasterProperty == NULL)
			{
				m_RasterProperty = new QgsRasterLayerProperties(qobject_cast<QgsRasterLayer*>(m_MapCanvas->currentLayer()), m_MapCanvas, NULL);
			}
			m_RasterProperty->exec();
			delete m_RasterProperty;
			m_RasterProperty = nullptr;
		}
		if (m_MapCanvas->currentLayer()->type() == QgsMapLayerType::VectorLayer)
		{
			QgsMessageBar* messageBar = new QgsMessageBar();
			QgsVectorLayer* vec = qobject_cast<QgsVectorLayer*>(m_MapCanvas->currentLayer());
			if (m_VecProperty == NULL)
			{
				m_VecProperty = new QgsVectorLayerProperties(m_MapCanvas, messageBar, vec);
			}
			m_VecProperty->exec();
			delete m_VecProperty;
			m_VecProperty = nullptr;
		}
	}
	else 
	{
		QMessageBox::critical(this, QStringLiteral("错误"), QStringLiteral("没有数据"));
	}
}
void qgisDemo::setting1(QgsMapLayer* layer)
{

	if (layer->type() == QgsMapLayerType::VectorLayer)
	{
		QgsMessageBar* messageBar = new QgsMessageBar();
		QgsVectorLayer* vec = qobject_cast<QgsVectorLayer*>(layer);
		if (m_VecProperty == NULL)
		{
			m_VecProperty = new QgsVectorLayerProperties(m_MapCanvas, messageBar, vec);
		}
		m_VecProperty->exec();
		delete m_VecProperty;
		m_VecProperty = nullptr;
	}
	if (layer->type() == QgsMapLayerType::RasterLayer)
	{
		if (m_RasterProperty == NULL)
		{
			m_RasterProperty = new QgsRasterLayerProperties(qobject_cast<QgsRasterLayer*>(layer), m_MapCanvas, NULL);
		}
		m_RasterProperty->exec();
		delete m_RasterProperty;
		m_RasterProperty = nullptr;
	}
}
void qgisDemo::deletelayer(QgsMapLayer* layer)
{
	
	if (m_layers.removeOne(layer))
	{
		//m_MapCanvas->setLayers(m_layers);
		QgsProject::instance()->removeMapLayer(layer);
		m_MapCanvas->refresh();
		//m_MapCanvas->setCurrentLayer(m_layers.first());
	}
	
}
void qgisDemo::styleManager()
{
	QgsStyle *style = QgsStyle::defaultStyle();
	if (m_StyleManger == NULL)
	{
		m_StyleManger = new QgsStyleManagerDialog(style);
	}
	m_StyleManger->show();
}
void qgisDemo::addVecLayer()
{
	QString fileName = QFileDialog::getOpenFileName(this, QStringLiteral("打开矢量文件"), "", QStringLiteral("ESRI矢量文件(*.shp);;GeoPackage文件 (*.gpkg)"));
	QStringList temp = fileName.split('/');
	QString basename = temp.at(temp.size() - 1);
	QgsVectorLayer* vecLayer = new QgsVectorLayer(fileName, basename, "ogr");

	if (!vecLayer->isValid())
	{
		QMessageBox::critical(this, QStringLiteral("错误"), QStringLiteral("图层无效!\n") + fileName);
		return;
	}
	m_MapCanvas->setExtent(vecLayer->extent());
	m_layers.prepend(vecLayer);
	m_MapCanvas->setLayers(m_layers);
	m_MapCanvas->refresh();
	m_MapCanvas->setCurrentLayer(vecLayer);
	QgsProject::instance()->addMapLayer(vecLayer);
}
bool qgisDemo::hadraster()
{
	foreach(QgsMapLayer * index, m_MapCanvas->layers())
	{
		if (index->type() == QgsMapLayerType::RasterLayer)
		{
			return true;
		}
	}
	return false;
}
void qgisDemo::classification()
{
	QTextCodec* codec = QTextCodec::codecForName("GBK");
	if (hadraster()==false)
	{
		QMessageBox::critical(this, QStringLiteral("错误"), QStringLiteral("没有可分类图像") );
		return;
	}
	foreach(QgsMapLayer * index, m_MapCanvas->layers())
	{
		if (index->name() == "sample")
		{
			m_classi->show();
			return;
		}
	}
	QgsMapLayer* a = m_layers.first();
	QString crs = a->crs().authid();
	QgsCoordinateReferenceSystem aa = a->crs();
	QString crs1 = "crs=EPSG:4326";
	QString path = "polygon?";
	// 几何类型,"point", "linestring", "polygon", "multipoint","multilinestring","multipolygon"
	path.append(QString(crs1 + "&")); //参照坐标系
	path.append(QString("field=id:integer&field=classname:string(50)&"));
	path.append(QString("index=yes&")); // 创建索引
	path.append(QString("memoryid=%1").arg(QUuid::createUuid().toString())); // 临时编码
	// 创建新矢量图层
	QgsField shpField("classname", QVariant::String);
	QgsField shpField1("UID", QVariant::Int);
	QgsVectorLayer* NewLayer = new QgsVectorLayer(path, "sample", QString("memory"));
	NewLayer->setRenderer(shpp);
	NewLayer->startEditing();
	m_layers.prepend(NewLayer);
	// m_mapCanvas->setExtent( NewLayer->extent() );
	m_MapCanvas->setLayers(m_layers);
	m_MapCanvas->setVisible(true);
	m_MapCanvas->freeze(false);
	m_MapCanvas->refresh();
	m_MapCanvas->setCurrentLayer(NewLayer);
	//	NewLayer->triggerRepaint();
	QgsProject::instance()->addMapLayer(NewLayer);
	m_dockwidegt = new QgsAdvancedDigitizingDockWidget(m_MapCanvas, this);
	m_dockwidegt->hide();
	m_classi = new toolbar(m_MapCanvas, m_dockwidegt, m_layers);
	m_classi->setParent(this);//指定父窗口
	m_classi->setWindowFlags(m_classi->windowFlags() | Qt::Dialog);

	//splitDockWidget(mLayerTreeDock, m_dockwidegt, Qt::Vertical);
	//m_dockwidegt->enable();
	bool o = m_dockwidegt->cadEnabled();
	//m_dockwidegt->releaseLocks();
	connect(m_classi, SIGNAL(loadvector(QString)), this, SLOT(loadvectorlayer(QString)));
	connect(m_classi, SIGNAL(loadraster(QString)), this, SLOT(loadrasterlayer(QString)));
	connect(m_classi, SIGNAL(layerchanged(QString)), this, SLOT(changecrs(QString)));
	m_classi->show();
}
void qgisDemo::changecrs(QString layername)
{
	QTextCodec* codec = QTextCodec::codecForName("GBK");
	QgsMapLayer* map;
	QgsMapLayer* map2;
	foreach(QgsMapLayer* index, m_MapCanvas->layers())
	{
		if (index->name() == layername)
		{
			map = index;
		}
		if (index->name() == "sample")
		{
			map2 = index;
		}
	}
	map2->setCrs(map->crs());
}
/*
	if (m_MapCanvas->currentLayer()->type() == QgsMapLayerType::VectorLayer)
	{
		if (m_classi != nullptr)
			m_classi->show();
		else
		{
			toolbar a(m_MapCanvas, m_dockwidegt, m_layers);
			//splitDockWidget(mLayerTreeDock, m_dockwidegt, Qt::Vertical);
			//m_dockwidegt->enable();
			bool o = m_dockwidegt->cadEnabled();
			//m_dockwidegt->releaseLocks();
			connect(&a, SIGNAL(loadvector(QString)), this, SLOT(loadvectorlayer(QString)));
			connect(&a, SIGNAL(loadraster(QgsRasterLayer*)), this, SLOT(loadrasterlayer(QgsRasterLayer*)));
			a.show();
		}
	}
	else
	{
		if (m_classi == nullptr) {
			QgsMapLayer* a = m_layers.first();
			QString crs = a->crs().authid();
			QgsCoordinateReferenceSystem aa = a->crs();
			QString crs1 = "crs=EPSG:4326";
			QString path = "polygon?";
			// 几何类型,"point", "linestring", "polygon", "multipoint","multilinestring","multipolygon"
			path.append(QString(crs1 + "&")); //参照坐标系
			path.append(QString("field=id:integer&field=classname:string(50)&"));
			path.append(QString("index=yes&")); // 创建索引
			path.append(QString("memoryid=%1").arg(QUuid::createUuid().toString())); // 临时编码
			// 创建新矢量图层
			QgsField shpField("classname", QVariant::String);
			QgsField shpField1("UID", QVariant::Int);
			QgsVectorLayer* NewLayer = new QgsVectorLayer(path, codec->toUnicode("样本图层"), QString("memory"));
			NewLayer->setRenderer(shpp);
			NewLayer->startEditing();
			m_layers.prepend(NewLayer);
			// m_mapCanvas->setExtent( NewLayer->extent() );
			m_MapCanvas->setLayers(m_layers);
			//m_MapCanvas->setVisible(true);
			//m_MapCanvas->freeze(false);
			m_MapCanvas->refresh();
			m_MapCanvas->setCurrentLayer(NewLayer);
			//	NewLayer->triggerRepaint();
			QgsProject::instance()->addMapLayer(NewLayer);
			m_dockwidegt = new QgsAdvancedDigitizingDockWidget(m_MapCanvas, this);
			m_dockwidegt->hide();
			toolbar a1(m_MapCanvas, m_dockwidegt, m_layers);
			//splitDockWidget(mLayerTreeDock, m_dockwidegt, Qt::Vertical);
			//m_dockwidegt->enable();
			bool o = m_dockwidegt->cadEnabled();
			//m_dockwidegt->releaseLocks();
			connect(&a1, SIGNAL(loadvector(QString)), this, SLOT(loadvectorlayer(QString)));
			connect(&a1, SIGNAL(loadraster(QString)), this, SLOT(loadrasterlayer(QString)));
			a1.show();
		}
		else
		{
			m_classi = nullptr;
			//delete m_classi;
			QgsMapLayer* a = m_layers.first();
			QString crs = a->crs().authid();
			QgsCoordinateReferenceSystem aa = a->crs();
			QString crs1 = "crs=EPSG:4326";
			QString path = "polygon?";
			// 几何类型,"point", "linestring", "polygon", "multipoint","multilinestring","multipolygon"
			path.append(QString(crs1 + "&")); //参照坐标系
			path.append(QString("field=id:integer&field=classname:string(50)&"));
			path.append(QString("index=yes&")); // 创建索引
			path.append(QString("memoryid=%1").arg(QUuid::createUuid().toString())); // 临时编码
			// 创建新矢量图层
			QgsField shpField("classname", QVariant::String);
			QgsField shpField1("UID", QVariant::Int);
			QgsVectorLayer* NewLayer = new QgsVectorLayer(path, QString("Temporary Layer"), QString("memory"));
			NewLayer->setRenderer(shpp);
			NewLayer->startEditing();
			m_layers.prepend(NewLayer);
			// m_mapCanvas->setExtent( NewLayer->extent() );
			m_MapCanvas->setLayers(m_layers);
			//m_MapCanvas->setVisible(true);
			//m_MapCanvas->freeze(false);
			m_MapCanvas->refresh();
			m_MapCanvas->setCurrentLayer(NewLayer);
			//	NewLayer->triggerRepaint();
			QgsProject::instance()->addMapLayer(NewLayer);
			m_dockwidegt = new QgsAdvancedDigitizingDockWidget(m_MapCanvas, this);
			m_dockwidegt->hide();
			toolbar a1(m_MapCanvas, m_dockwidegt, m_layers);
			//splitDockWidget(mLayerTreeDock, m_dockwidegt, Qt::Vertical);
			//m_dockwidegt->enable();
			bool o = m_dockwidegt->cadEnabled();
			//m_dockwidegt->releaseLocks();
			connect(a1, SIGNAL(loadvector(QString)), this, SLOT(loadvectorlayer(QString)));
			connect(a1, SIGNAL(loadraster(QString)), this, SLOT(loadrasterlayer(QString)));
			a1.show();
		}
	}
}*/
void qgisDemo::loadvectorlayer(QString path)
{
	QStringList temp = path.split('/');
	QString basename = temp.at(temp.size() - 1);
	QgsVectorLayer* vecLayer = new QgsVectorLayer(path, path, "ogr");

	if (!vecLayer->isValid())
	{
		QMessageBox::critical(this, QStringLiteral("错误"), QStringLiteral("图层无效!\n") + path);
		return;
	}
	m_MapCanvas->setExtent(vecLayer->extent());
	m_layers.prepend(vecLayer);
	m_MapCanvas->setLayers(m_layers);
	m_MapCanvas->refresh();
	m_MapCanvas->setCurrentLayer(vecLayer);
	QgsProject::instance()->addMapLayer(vecLayer);
}
void qgisDemo::loadrasterlayer(QString path)
{
	QStringList temp = path.split(QDir::separator());
	QString basename = temp.at(temp.size() - 1);
	QgsRasterLayer::LayerOptions layerOption;
	layerOption.loadDefaultStyle = false;
	QgsRasterLayer* rasterLayer1 = new QgsRasterLayer(path, basename, "gdal", layerOption);
	if (!rasterLayer1->isValid())
	{
		QMessageBox::critical(this, QStringLiteral("错误"), QStringLiteral("分类结果错误!\n") + path);
		return;
	}
	QgsRasterDataProvider* data1 = rasterLayer1->dataProvider();
	if ((data1->xSize() > 1000 || data1->ySize() > 1000) && data1->hasPyramids() == false)
	{
		buildPyramid* py = new buildPyramid(data1);
		py->exec();
		delete py;
	}
	delete rasterLayer1;
	QgsRasterLayer* rasterLayer = new QgsRasterLayer(path, basename, "gdal", layerOption);
	QgsRasterDataProvider* data = rasterLayer->dataProvider();
	if (x == nullptr)
	{
		x = new QgsPalettedRasterRenderer(rasterLayer->dataProvider(), 1, classdata);
	}
	rasterLayer->setRenderer(x);
	data->setNoDataValue(1, 255);
	m_MapCanvas->setExtent(rasterLayer->extent());
	m_layers.prepend(rasterLayer);
	m_MapCanvas->setLayers(m_layers);
	m_MapCanvas->refresh();
	m_MapCanvas->setCurrentLayer(rasterLayer);
	QgsProject::instance()->addMapLayer(rasterLayer);
}
void qgisDemo::loadrasterlayer(QgsRasterLayer* path)
{
	m_MapCanvas->setExtent(path->extent());
	m_layers.prepend(path);
	m_MapCanvas->setLayers(m_layers);
	m_MapCanvas->refresh();
	m_MapCanvas->setCurrentLayer(path);
	QgsProject::instance()->addMapLayer(path);
}
void qgisDemo::portBtnclick()
{
	zoompan = new QgsMapToolPan(m_MapCanvas);
	m_MapCanvas->setMapTool(zoompan);
	zoompan->activate();
}
void qgisDemo::portBtn1click()
{
	zoomin = new QgsMapToolZoom(m_MapCanvas, false /* zoomIn */);
	m_MapCanvas->setMapTool(zoomin);
	zoomin->activate();
}
void qgisDemo::portBtn2click()
{
	zoomout = new QgsMapToolZoom(m_MapCanvas, true /* zoomIn */);
	m_MapCanvas->setMapTool(zoomout);
	zoomout->activate();
}
void qgisDemo::Moravec()
{

    mor = new Moravecui(m_MapCanvas);
	connect(mor, SIGNAL(rraster(QString)), this, SLOT(loadrasterlayer(QString)));
	mor->show();

}
void qgisDemo::pipeiyingxiang()
{
	pi = new pipei(m_MapCanvas);
	connect(pi, SIGNAL(raste(QString)), this, SLOT(loadrasterlayer(QString)));
	pi->show();
}