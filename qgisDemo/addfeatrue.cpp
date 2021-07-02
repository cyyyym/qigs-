#include "addfeatrue.h"
#include<QTime>
#include <QStringList>
#include <QtMath>
#include <qgsvectorlayer.h>
#include <qgslogger.h>
#include <qgsvectordataprovider.h>
//#include <qgscsexception.h>
#include "qgsproject.h"
//#include <qgsmaplayerregistry.h>
#include "qgsmaptopixel.h"
#include "qgsgeometry.h"
#include "qgsfeature.h"
#include<QMessageBox>
#include"dialog.h"
#include<QFileDialog>
#include"qmessagebox.h"
#include"qgsvectorfilewriter.h"
#include "qgsvectordataprovider.h"
#include"gdal.h"

#include <iostream>
#include "gdal_priv.h"
#include "ogrsf_frmts.h"
#include "gdal_alg.h"

#include <qgscoordinatetransformcontext.h>
addfeatrue::addfeatrue(QgsMapCanvas* mapCanvas)
    : QgsMapTool(mapCanvas)
{
    mToolName = tr("Add Feature");
    mRubberBand = 0;
    mTempRubberBand = 0;
    mValidator = 0;
    a = new Dialog();
    a->show();
    layer = qobject_cast<QgsVectorLayer*>(mCanvas->currentLayer());
    connect(a, SIGNAL(mvsigSendData(QList<addfeatrue::yangben>)), this, SLOT(mvslotReceiveData(QList<addfeatrue::yangben>)));
    connect(a, SIGNAL(shuchuyangben()), this, SLOT(saveshp()));
    connect(a, SIGNAL(shanchuyangben(int)), this, SLOT(shanchu(int)));
    connect(a, SIGNAL(open()), this, SLOT(openshp()));
    connect(a1, SIGNAL(openfeatures()), this, SLOT(open()));
}
void addfeatrue::open()
{
    a->show();
}

addfeatrue::~addfeatrue()
{

}
void addfeatrue::yangbenwenjian(QString tiffname, QString shpname, QString shuchu, QString jieguo)
{
    GDALAllRegister();
    OGRRegisterAll();
    char* tiff;
    char* shp;
    char* out;
    char* jieguo1;
    QByteArray ba = shpname.toLatin1(); // must
    shp = ba.data();
    ba = tiffname.toLatin1();
    tiff = ba.data();
    ba = shuchu.toLatin1();
    out = ba.data();
    ba = jieguo.toLatin1();
    jieguo1 = ba.data();
    //打开矢量图层
    CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");//支持中文路径
    GDALAllRegister();
    OGRRegisterAll();
    //打开矢量图层
    GDALDataset* shpData = (GDALDataset*)GDALOpenEx(shp, GDAL_OF_READONLY, NULL, NULL, NULL);
    GDALDataset* tifdata = (GDALDataset*)GDALOpen(tiff, GA_Update);
    if (shpData == NULL)
    {
        // return FALSE;
    }
    tifdata->AddBand(GDT_Float32);
    OGRLayer* shpLayer = tifdata->GetLayer(0);
    OGREnvelope env;
    shpLayer->GetExtent(&env);//获取图层的坐标范围到env指向的内存中
    int m_nImageWidth = tifdata->GetRasterXSize();
    int m_nImageHeight = tifdata->GetRasterYSize();
    OGRSpatialReference* pOgrSRS = shpLayer->GetSpatialRef();//数据投影信息
    char* pPrj = NULL;
    if (pOgrSRS == NULL)
    {
        // cout << "无投影信息...\n";
        m_nImageHeight = (int)env.MaxX;
        m_nImageWidth = (int)env.MaxY;
    }
    else
    {
        pOgrSRS->exportToWkt(&pPrj);
    }

    //获得驱动创建数据集
    GDALDriver* poDriver = GetGDALDriverManager()->GetDriverByName("GTiff");
    GDALDataset* poNewDS = poDriver->Create(tiff, m_nImageWidth, m_nImageHeight, 1, GDT_Int16, NULL);

    double adfGeoTransform[6];
    adfGeoTransform[0] = env.MinX;//左上角经度
    adfGeoTransform[1] = (env.MaxX - env.MinX) / m_nImageWidth;//像元宽度(影像在宽度上的分辨率)
    adfGeoTransform[2] = 0; //如果影像是指北的, 这个参数的值为0
    adfGeoTransform[3] = env.MaxY;//左上角纬度
    adfGeoTransform[4] = 0; //如果影像是指北的, 这个参数的值为0。
    adfGeoTransform[5] = (env.MinY - env.MaxY) / m_nImageHeight; //像元高度(影像在高度上的分辨率)
    GDALSetGeoTransform(poNewDS, adfGeoTransform);
    if (pOgrSRS != NULL)
    {
        poNewDS->SetProjection(pPrj);
    }

    int* pnbandlist = new int[1];
    pnbandlist[0] = 1;

    OGRLayerH* player;
    player = new OGRLayerH[1];
    player[0] = (OGRLayerH)shpLayer;

    char** papszOptions = NULL;
    papszOptions = CSLSetNameValue(papszOptions, "CHUNKSIZE", "1");
    papszOptions = CSLSetNameValue(papszOptions, "ATTRIBUTE", "id");//矢量图层属性字段名,我这里是DF

    CPLErr err = GDALRasterizeLayers((GDALDatasetH)poNewDS, 1, pnbandlist, 1, player,
        NULL, NULL, NULL, papszOptions, NULL, NULL);//这里传了很多空值，只简单展示矢量转栅格的功能，详情以后介绍
    GDALDataset* tifdata2 = (GDALDataset*)GDALOpen(out, GA_Update);
    GDALDataset* poDstDS;
    GDALDriver* poDriver1;
    char** papszOptions2 = NULL;
    const char* pszFormat = "GTiff";
    poDriver1 = GetGDALDriverManager()->GetDriverByName(pszFormat);
    poDstDS = poDriver1->Create(jieguo1, m_nImageWidth, m_nImageHeight, tifdata->GetRasterCount() + 1, GDT_Int16,
        NULL);
    for (int i = 1; i <= poDstDS->GetRasterCount(); i++)
    {
        if (i == poDstDS->GetRasterCount())
        {
            GDALRasterBand* raseterBandRed = tifdata2->GetRasterBand(i);
            GDALRasterBand* outputRasterBand = poDstDS->GetRasterBand(1);
            // 申请存储空间，为一行的大小
            int* bufferBlockRed = (int*)CPLMalloc(sizeof(int) * m_nImageWidth);
            int* outputBufferBlock = (int*)CPLMalloc(sizeof(int) * m_nImageWidth);
            //逐行读取
            for (int i = 0; i < m_nImageHeight; i++)
            {
                raseterBandRed->RasterIO(GF_Read, 0, i, m_nImageWidth, 1, bufferBlockRed, m_nImageWidth, 1, GDT_Int16, 0, 0);
                outputRasterBand->RasterIO(GF_Write, 0, i, m_nImageWidth, 1, outputBufferBlock, m_nImageWidth, 1, GDT_Int16, 0, 0); // 写入数据
            }
            CPLFree(bufferBlockRed);
            CPLFree(outputBufferBlock);
        }
        else
        {
            GDALRasterBand* raseterBandRed = tifdata->GetRasterBand(i);
            GDALRasterBand* outputRasterBand = poDstDS->GetRasterBand(1);
            // 申请存储空间，为一行的大小
            int* bufferBlockRed = (int*)CPLMalloc(sizeof(int) * m_nImageWidth);
            int* outputBufferBlock = (int*)CPLMalloc(sizeof(int) * m_nImageWidth);
            //逐行读取
            for (int i = 0; i < m_nImageHeight; i++)
            {
                raseterBandRed->RasterIO(GF_Read, 0, i, m_nImageWidth, 1, bufferBlockRed, m_nImageWidth, 1, GDT_UInt32, 0, 0);
                outputRasterBand->RasterIO(GF_Write, 0, i, m_nImageWidth, 1, outputBufferBlock, m_nImageWidth, 1, GDT_UInt32, 0, 0); // 写入数据
            }
            CPLFree(bufferBlockRed);
            CPLFree(outputBufferBlock);
        }
    }
    GDALClose(shpData);
    GDALClose(poNewDS);
    GDALDestroyDriverManager();
    delete[]player;
    delete[]pnbandlist;
    if (err != CE_None)
    {
    }
}
void addfeatrue::openshp()
{
    emit openyb();
    QString path = QFileDialog::getOpenFileName(nullptr, tr("Open File"), QString(), "ESRI Shapefile(*.shp)||GPKG(.gpkg)");
    if (path.isEmpty())  return;

    QgsVectorLayer* layer1 = new QgsVectorLayer(path, QFileInfo(path).completeBaseName(), "yangben");
    if (!layer1->isValid())
    {
        QMessageBox::information(NULL, "Title", "Content", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);;
        return;
    }
    layer1->selectAll();
    QgsFeatureList fea = layer1->selectedFeatures();
    int i = fea.count();
    QgsFeature a0 = fea.first();
    /*while(i!=0)
    {
        yangben *a=new yangben();
        qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
        int a1 = qrand()%255; //随机生成0到9的随机数
        int a2=qrand()%255;
        int a3=qrand()%255;
        QSettings settings;
         a->polygon= new QgsRubberBand( mCanvas, QgsWkbTypes::PolygonGeometry );
        a->polygon->setWidth( settings.value( "/qgis/digitizing/line_width", 1 ).toInt() );
        QColor color( settings.value( "/qgis/digitizing/line_color_red", a1 ).toInt(),
                      settings.value( "/qgis/digitizing/line_color_green", a2 ).toInt(),
                      settings.value( "/qgis/digitizing/line_color_blue", a3 ).toInt() );
            double myAlpha = settings.value( "/qgis/digitizing/line_color_alpha", 200 ).toInt() / 255.0;
        a->polygon->addGeometry(a0.geometry(),layer);
        color.setAlphaF( myAlpha );
        a->polygon->setColor( color );
        a->polygon->show();
        QString s = QString::number(i0);
        a->id=a0.fields().field("id").alias().toInt();
        a->classname="classname"+a->id;
        i0++;
        yangbenlist.append(*a);
        foreach (QgsFeature index, fea)
        {
            if(index.fields().field("id")==a0.fields().field("id")&&index!=a0)
            {
                yangben *b=new yangben();
                b->polygon=new QgsRubberBand( mCanvas, QgsWkbTypes::PolygonGeometry );
                b->polygon->setColor()
            }
        }

    }
     foreach (QgsFeature index, a1)
     {
         QgsField id
         yangben *a=new yangben();
         qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
         int a1 = qrand()%255; //随机生成0到9的随机数
         int a2=qrand()%255;
         int a3=qrand()%255;
         QSettings settings;
          a->polygon= new QgsRubberBand( mCanvas, QgsWkbTypes::PolygonGeometry );
         a->polygon->setWidth( settings.value( "/qgis/digitizing/line_width", 1 ).toInt() );
         QColor color( settings.value( "/qgis/digitizing/line_color_red", a1 ).toInt(),
                       settings.value( "/qgis/digitizing/line_color_green", a2 ).toInt(),
                       settings.value( "/qgis/digitizing/line_color_blue", a3 ).toInt() );
             double myAlpha = settings.value( "/qgis/digitizing/line_color_alpha", 200 ).toInt() / 255.0;
         a->polygon->addGeometry(index.geometry(),layer);
         color.setAlphaF( myAlpha );
         a->polygon->setColor( color );
         a->polygon->show();
         QString s = QString::number(i0);
         a->classname="classname"+s;
         a->id=i0;
         i0++;
         yangbenlist.append(*a);
     }
    */

}
void addfeatrue::canvasReleaseEvent(QgsMapMouseEvent* e)
{
    // 获取当前图层


    QgsMapLayer* layer1 = mCanvas->currentLayer();

    // 判断当前图层是否为矢量图层
    if (!layer) { emit messageEmitted(tr("not a valid vector layer.")); return; }

    // 判断数据驱动状态
    QgsVectorDataProvider* provider = layer->dataProvider();
    if (!(provider->capabilities() & QgsVectorDataProvider::AddFeatures))
    {
        QMessageBox::information(NULL, "Title", "Content", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
        return;
    }

    // 判断当前图层是否可编辑
    if (!layer->isEditable()) { emit messageEmitted(tr("can't edit this layer.")); return; }

    // 得到点坐标，转换为地图坐标
    QgsPointXY savePoint;
    try
    {
        savePoint = toLayerCoordinates(layer, mCanvas->mapSettings().mapToPixel().toMapCoordinates(e->pos()));
        QgsDebugMsg("savePoint = " + savePoint.toString());
    }
    catch (QgsCsException& cse)
    {
        Q_UNUSED(cse);
        QMessageBox::information(NULL, "Title", "Content", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
        return;
    }


    switch (layer->geometryType())
    {
    case QgsWkbTypes::PointGeometry:
        m_captureMode = CapturePoint;
        break;
    case QgsWkbTypes::LineGeometry:
        m_captureMode = CaptureLine;
        break;
    case QgsWkbTypes::PolygonGeometry:
        m_captureMode = CapturePolygon;
        break;
    default:
        break;
    }

    QgsGeometry* g = new QgsGeometry(); // 新建一个geometry
    if (m_captureMode == CapturePoint)
    {
        // 转换为geometry
        if (layer->wkbType() == QgsWkbTypes::Point || layer->wkbType() == QgsWkbTypes::Point25D)
        {
            *g = QgsGeometry::fromPointXY(savePoint);
        }
        else if (layer->wkbType() == QgsWkbTypes::MultiPoint || layer->wkbType() == QgsWkbTypes::MultiPoint25D)
        {
            *g = QgsGeometry::fromMultiPointXY(QgsMultiPointXY() << savePoint);
        }

        // 转换为feature
        QgsFeature feature(layer->fields(), 0);
        feature.setGeometry(*g);
        addFeature(layer, &feature, false);
        //layer->addFeature( feature, true );
        mCanvas->setExtent(layer->extent());
        mCanvas->refresh();
    }
    else if (m_captureMode == CaptureLine || m_captureMode == CapturePolygon)
    {
        if (e->button() == Qt::LeftButton) // 鼠标左键
        {
            int error = addVertex(mCanvas->mapSettings().mapToPixel().toMapCoordinates(e->pos()));
            if (error == 1) { return; } // current layer is not a vector layer
            else if (error == 2) // problem with coordinate transformation
            {
                QMessageBox::information(NULL, "Title", "Content", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
                return;
            }
            startCapturing();
        }
        else if (e->button() == Qt::RightButton) // 鼠标右键
        {
            deleteTempRubberBand();
            if (m_captureMode == CaptureLine && m_captureList.size() < 2) { return; }
            if (m_captureMode == CapturePolygon && m_captureList.size() < 3) { return; }

            /*    QgsFeature* feature = new QgsFeature( layer->fields(), 0 );
            QgsGeometry* g = new QgsGeometry; // 新建一个geometry
            if ( m_captureMode == CaptureLine )
            {
                if ( layer->wkbType() == QgsWkbTypes::LineString || layer->wkbType() == QgsWkbTypes::LineString25D )
                {
                    *g = QgsGeometry::fromPolylineXY( m_captureList.toVector() );
                }
                else if ( layer->wkbType() == QgsWkbTypes::MultiLineString || layer->wkbType() == QgsWkbTypes::MultiLineString25D )
                {
                    *g = QgsGeometry::fromMultiPolylineXY( QgsMultiPolylineXY() << m_captureList.toVector() );
                }
                else
                {
                    QMessageBox::information(NULL,  "Title",  "Content", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
                    stopCapturing();
                    delete feature;
                    return;
                }
                feature->setGeometry(*g);
            }
            else if ( m_captureMode == CapturePolygon )
            {
                if ( layer->wkbType() == QgsWkbTypes::Polygon ||  layer->wkbType() == QgsWkbTypes::Polygon25D )
                {
                    *g =QgsGeometry::fromPolygonXY( QgsPolygonXY() << m_captureList.toVector() );
                }
                else if ( layer->wkbType() == QgsWkbTypes::MultiPolygon ||  layer->wkbType() == QgsWkbTypes::MultiPolygon25D )
                {
                    *g = QgsGeometry::fromMultiPolygonXY( QgsMultiPolygonXY() << ( QgsPolygonXY() << m_captureList.toVector() ) );
                }
                else
                {
                    stopCapturing();
                    delete feature;
                    return;
                }

                if ( !g )
                {
                    stopCapturing();
                    delete feature;
                    return; // invalid geometry; one possibility is from duplicate points
                }
                feature->setGeometry( *g );
                addFeature( layer, feature, false);

                layer->selectAll();
                int i=layer->selectedFeatureCount();
*/
            stopCapturing();
            a->daoru(yangbenlist, mCanvas);
            a->fresh();
            yangbenlist = a->list1;
        }
    }
}
void addfeatrue::saveshp()
{
    layer->selectAll();
    layer->deleteSelectedFeatures();
    foreach(yangben a1, yangbenlist)
    {
        QgsFeature* feature = new QgsFeature(layer->fields(), 0);
        QgsGeometry* g = new QgsGeometry(); // 新建一个geometry
        *g = a1.polygon->asGeometry();
        feature->setGeometry(*g);
        addFeature(a1, layer, feature, false);
    }
    layer->selectAll();
    QString fileName = QFileDialog::getSaveFileName(0, tr("Open Config"), "", tr("SHP文件 (*.shp)"));
    if (!fileName.isNull())
    {
        //fileName是文件名
    }
    else
    {
        //点的是取消
    }
    const QgsCoordinateTransformContext a1;
    const QgsVectorFileWriter::SaveVectorOptions aa;
    QgsVectorFileWriter::WriterError a = QgsVectorFileWriter::writeAsVectorFormatV2(layer, fileName, a1, aa);
    int i = layer->selectedFeatureCount();
}
void addfeatrue::shanchu(int i)
{
    yangben a = yangbenlist.takeAt(i);
    a.~yangben();
    //delete a.polygon;
    //.removeAt(i);
}
void addfeatrue::mvslotReceiveData(QList<addfeatrue::yangben> sData)
{
    yangbenlist = sData;
    return;
}
bool addfeatrue::addFeature(yangben a, QgsVectorLayer* layer, QgsFeature* feature, bool showModal /*= true*/)//有错误
{
    if (!layer || !layer->isEditable()) { return false; }

    QgsAttributeMap defaultAttributes;
    QgsFields aa = layer->fields();
    QgsVectorDataProvider* provider = layer->dataProvider();

    QSettings settings;
    bool reuseLastValues = settings.value("/qgis/digitizing/reuseLastValues", false).toBool();
    QgsDebugMsg(QString("reuseLastValues: %1").arg(reuseLastValues));

    // add the fields to the QgsFeature
    const QgsFields& fields = layer->fields();
    feature->initAttributes(fields.count());
    for (int idx = 0; idx < fields.count(); ++idx)
    {
        if (idx == 0)
        {
            QVariant v(a.id);
            feature->setAttribute(idx, v);
        }
        else
        {
            QVariant v(a.classname);
            feature->setAttribute(idx, v);
        }

    }
    features.append(*feature);
    layer->addFeature(*feature);
    mCanvas->freeze(false);
    mCanvas->refresh();
    return true;
}
bool addfeatrue::addFeature(QgsVectorLayer* layer, QgsFeature* feature, bool showModal /*= true*/)//有错误
{
    if (!layer || !layer->isEditable()) { return false; }
    QgsAttributeMap defaultAttributes;
    QgsFields aa = layer->fields();
    QgsVectorDataProvider* provider = layer->dataProvider();
    QSettings settings;
    bool reuseLastValues = settings.value("/qgis/digitizing/reuseLastValues", false).toBool();
    QgsDebugMsg(QString("reuseLastValues: %1").arg(reuseLastValues));
    // add the fields to the QgsFeature
    const QgsFields& fields = layer->fields();
    feature->initAttributes(fields.count());
    for (int idx = 0; idx < fields.count(); ++idx)
    {
        if (idx == 0)
        {
            QVariant v(i0);
            feature->setAttribute(idx, v);
        }
        else
        {
            QVariant v("classname" + i0);
            feature->setAttribute(idx, v);
        }
    }
    features.append(*feature);
    layer->addFeature(*feature);
    mCanvas->freeze(false);
    mCanvas->refresh();
    return true;
}
void addfeatrue::activate()
{
    QgsVectorLayer* layer = qobject_cast<QgsVectorLayer*>(mCanvas->currentLayer());
    if (layer && layer->geometryType() == QgsWkbTypes::NullGeometry)
    {
        QgsFeature f;
        addFeature(layer, &f, false);
        return;
    }

    QgsMapTool::activate();
}

void addfeatrue::notifyNotVectorLayer()
{
    emit messageEmitted(tr("No active vector layer"));
}

void addfeatrue::notifyNotEditableLayer()
{
    emit messageEmitted(tr("Layer not editable"));
}

addfeatrue::CaptureMode addfeatrue::mode()
{
    return m_captureMode;
}

int addfeatrue::addVertex(const QgsPointXY& point)
{

    if (mode() == CaptureNone) { QgsDebugMsg("invalid capture mode"); return 2; }

    QgsPointXY layerPoint;
    int res = nextPoint(point, layerPoint);
    if (res != 0) { return res; } // 当前点必须是最后一个点

    if (!mRubberBand) // 没有rubber band，就创建一个
    {

        mRubberBand = createRubberBand(m_captureMode == CapturePolygon ? QgsWkbTypes::PolygonGeometry : QgsWkbTypes::LineGeometry);

    }
    mRubberBand->addPoint(point);
    m_captureList.append(layerPoint);

    if (!mTempRubberBand)
    {
        mTempRubberBand = createRubberBand(m_captureMode == CapturePolygon ? QgsWkbTypes::PolygonGeometry : QgsWkbTypes::LineGeometry, true);
    }
    else
    {
        if (m_captureMode == CapturePolygon)
        {
            mTempRubberBand->reset();
        }

    }

    if (m_captureMode == CaptureLine)
    {
        mTempRubberBand->addPoint(point);
    }
    else if (m_captureMode == CapturePolygon)
    {
        const QgsPointXY* firstPoint = mRubberBand->getPoint(0, 0);
        mTempRubberBand->addPoint(*firstPoint);
        mTempRubberBand->movePoint(point);
        mTempRubberBand->addPoint(point);
    }

    validateGeometry(); // 验证几何有效性

    return 0;
}

void addfeatrue::startCapturing()
{
    mCapturing = true;
}

void addfeatrue::deleteTempRubberBand()
{
    if (mTempRubberBand)
    {
        delete mTempRubberBand;
        mTempRubberBand = 0;
    }
}

void addfeatrue::stopCapturing()
{

    // ploygons.append(mRubberBand);
    if (mRubberBand)
    {
        yangben* a = new yangben();
        QString s = QString::number(i0);
        a->classname = "classname" + s;
        a->id = i0;
        a->polygon = mRubberBand;
        i0++;
        yangbenlist.append(*a);
        QMessageBox::information(NULL, "提示", "样本创建成功" + yangbenlist.last().classname, QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
        //delete mRubberBand;
        mRubberBand = 0;
    }

    //if ( mTempRubberBand )
    //  {
    //      delete mTempRubberBand;
    //      mTempRubberBand = 0;
    //  }

    while (!mGeomErrorMarkers.isEmpty())
    {
        delete mGeomErrorMarkers.takeFirst();
    }

    mGeomErrors.clear();

    mCapturing = false;
    m_captureList.clear();
    mCanvas->refresh();
}

int addfeatrue::nextPoint(const QgsPointXY& mapPoint, QgsPointXY& layerPoint)
{
    QgsVectorLayer* vlayer = qobject_cast<QgsVectorLayer*>(mCanvas->currentLayer());
    if (!vlayer) { QgsDebugMsg("no vector layer"); return 1; }
    try
    {
        layerPoint = toLayerCoordinates(vlayer, mapPoint); //transform snapped point back to layer crs
    }
    catch (QgsCsException& cse)
    {
        Q_UNUSED(cse);
        QgsDebugMsg("transformation to layer coordinate failed");
        return 2;
    }

    return 0;
}

QgsRubberBand* addfeatrue::createRubberBand(QgsWkbTypes::GeometryType geometryType /*= QGis::Line*/, bool alternativeBand /*= false */)
{
    qsrand(QTime(0, 0, 0).secsTo(QTime::currentTime()));
    int a = qrand() % 255; //随机生成0到9的随机数
    int a1 = qrand() % 255;
    int a2 = qrand() % 255;
    QSettings settings;
    QgsRubberBand* rb = new QgsRubberBand(mCanvas, geometryType);
    rb->setWidth(settings.value("/qgis/digitizing/line_width", 1).toInt());
    QColor color(settings.value("/qgis/digitizing/line_color_red", a).toInt(),
        settings.value("/qgis/digitizing/line_color_green", a1).toInt(),
        settings.value("/qgis/digitizing/line_color_blue", a2).toInt());
    double myAlpha = settings.value("/qgis/digitizing/line_color_alpha", 200).toInt() / 255.0;
    if (alternativeBand)
    {
        myAlpha = myAlpha * settings.value("/qgis/digitizing/line_color_alpha_scale", 0.75).toDouble();
        rb->setLineStyle(Qt::DotLine);
    }
    if (geometryType == QgsWkbTypes::PolygonGeometry)
    {
        color.setAlphaF(myAlpha);
    }
    color.setAlphaF(myAlpha);
    rb->setColor(color);
    rb->show();
    return rb;
}

void addfeatrue::validateGeometry()
{
    QSettings settings;
    if (settings.value("/qgis/digitizing/validate_geometries", 1).toInt() == 0) { return; }

    if (mValidator)
    {
        mValidator->deleteLater();
        mValidator = 0;
    }

    mTip = "";
    mGeomErrors.clear();
    while (!mGeomErrorMarkers.isEmpty())
    {
        delete mGeomErrorMarkers.takeFirst();
    }

    QgsGeometry* g = new QgsGeometry();
    switch (m_captureMode)
    {
    case CaptureNone:
    case CapturePoint:
        return;

    case CaptureLine:
        if (m_captureList.size() < 2) { return; }
        *g = QgsGeometry::fromPolylineXY(m_captureList.toVector());
        break;

    case CapturePolygon:
        if (m_captureList.size() < 3) { return; }
        *g = QgsGeometry::fromPolygonXY(QgsPolygonXY() << (QgsPolylineXY() << m_captureList.toVector() << m_captureList[0]));
        break;
    }

    if (!g) { return; }

    mValidator = new QgsGeometryValidator(*g);
    connect(mValidator, SIGNAL(errorFound(QgsGeometry::Error)), this, SLOT(addError(QgsGeometry::Error)));
    connect(mValidator, SIGNAL(finished()), this, SLOT(validationFinished()));
    mValidator->start();
}
