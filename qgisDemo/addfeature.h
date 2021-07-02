#pragma once
#include"qgis.h"
#include"qgsmaptooldigitizefeature.h"
#include"qgsadvanceddigitizingdockwidget.h"
#include"qgsmapcanvas.h"
#include"qgsmaptool.h"
#include"qgsmaptoolcapture.h"
#include"qgsfeature.h"
#include"qgsgraduatedsymbolrenderer.h"
#include"qgssymbol.h"
#include "qgsrendererrange.h"
#include "qgsclassificationmethod.h"
#include<map>
#include <qgspalettedrasterrenderer.h>
using namespace std;
class RandomForest;
class addfeature :
    public QgsMapToolDigitizeFeature
{
    Q_OBJECT
public:
    addfeature(QgsMapCanvas* canvas, QgsAdvancedDigitizingDockWidget* cadDockWidget, CaptureMode mode = QgsMapToolCapture::CapturePolygon);

    void setid(int i1)
    {
        id = i1;
    }

   
    //颜色控制
    void InitField();
    //返回qgsfeature列表
    QgsFeatureList returnlist()
    {
        m_layer->selectAll();
        return m_layer->selectedFeatures();
    }
    //类别控制
    void makelabletiff(QList<QString>& );
    void random(QgsVectorLayer* ,QgsRasterDataProvider*, QgsRasterLayer*, string output, int& numband, int& types, QString& min_sample, QString& samp, QString& featurep,int nums);
    void maketext(QgsVectorLayer* vector, QgsRasterDataProvider* raster, QString txt, QgsRasterLayer* data,QString tiffname);
    void classification(QString txt, QgsRasterDataProvider* raster);
    void lvbo(int wide, int high, int dpi,QgsRasterLayer* raster,QString tiffname);
    bool contain(QgsVectorLayer* vector, QgsRasterDataProvider* raster, QgsPointXY* piont,QString txt);

signals:
    void creatfeature(QgsFeature&);
    void error(int);
    void loadshp(QString);
    void progressvalue(int);
    void progressmessage(QString&);
    void progressfresh();
    void progressshow(bool);
    void newraster(QString);
public slots:
    void clearallsample()
    {
        m_layer->selectAll();
        m_layer->deleteSelectedFeatures();
    }
    void deletfeature1(QgsFeature& a)
    {
        m_layer->deleteFeature(a.id());
        mCanvas->refresh();
    }
    void select(QgsFeature& a)
    {
        m_layer->select(a.id());
    }
    void cancle()
    {
        m_layer->removeSelection();
    }
    void clear()
    {
        emit progressfresh();
    }
    void setvalue(int i)
    {
        emit progressvalue(i);
    }
    void setmessage(QString& text)
    {
        emit progressmessage(text);
    }
    void lvboo(QList<QString>);
    void youhua1();
    void addfeature1(const QgsFeature&);
    void outputlayer();
    void classify();
    void classifyaction(QList<QString>);
    void addfeaturefromshp(QgsVectorLayer*, QString);
private:
    int id;//地物编码
    QString m_name;//地物名称
    QgsGraduatedSymbolRenderer* mRenderer = nullptr;
  //  QgsSymbolLayerList m_Sym_Layer_List;
    QgsVectorLayer* m_layer = nullptr;
    QList<QColor> m_colorlist;
    map<int, QRgb> rgb;
    RandomForest* random1;
    QgsPalettedRasterRenderer::ClassData classdata;
};