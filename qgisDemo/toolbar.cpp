#include "toolbar.h"
#include"addfeature.h"
#include"qgsadvanceddigitizingdockwidget.h"
#include"qgsmapcanvas.h"
#include"qgsmaptooldigitizefeature.h "
#include"qgsadvanceddigitizingdockwidget.h"
#include"qgsmaptool.h"
#include"qgsmaptoolcapture.h"
#include"qgsfeature.h"
#include"qgisDemo.h"
#include <QtWidgets/QMainWindow>
#include"qmessagebox.h"
#include"opensample.h"
#include"classmangement.h"
#include"majority.h"
#include"qtablewidget.h"
#include"progress.h"
#include"windows.h"
#include"WuLasFile.h"

toolbar::toolbar(QgsMapCanvas* canvas, QgsAdvancedDigitizingDockWidget* dockwidget,QList<QgsMapLayer*> mapCanvasLayerSet1,QWidget *parent)
	: QWidget(parent)
{
    ui.setupUi(this);
    QTextCodec* codec = QTextCodec::codecForName("GBK");//修改这两行
    //m_model->setHeaderData(0, Qt::Horizontal, "ID");
    //m_model->setHeaderData(1, Qt::Horizontal, "classname");
    QStringList header;
    header << "ID" << codec->toUnicode("地物名称") ;
    ui.tableWidget->setColumnCount(2);   //设置列数为4
    ui.tableWidget->setEnabled(true);
    ui.tableWidget->setHorizontalHeaderLabels(header);
    ui.tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows); //整行选中的方式
    ui.tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers); //禁止编辑
    ui.tableWidget->horizontalHeader()->setStretchLastSection(true); //行头自适应表格
    ui.progressBar->hide();
    ui.textBrowser->hide();
    ui.progressBar->setRange(0, 100);
    ui.progressBar->setMinimum(0);
    ui.progressBar->setValue(0);
    //m_progress->setHidden(true);
   // m_progress->hide();
    m_mapcanvas = canvas;
    mapCanvasLayerSet = mapCanvasLayerSet1;
    m_opensample = new opensampleq(m_mapcanvas);
    m_addfeature = new addfeature(m_mapcanvas,dockwidget,QgsMapToolCapture::CapturePolygon);
    m_majority = new majority(m_mapcanvas);
    connect(m_majority, SIGNAL(start(QList<QString>)), m_addfeature, SLOT(lvboo(QList<QString>)));
    m_majority->hide();
    m_classmangement = new classmangement(m_mapcanvas);
    connect(m_classmangement, SIGNAL(start(QList<QString>)), m_addfeature, SLOT(classifyaction(QList<QString>)));
    m_classmangement->hide();
    m_mapcanvas->setMapTool(m_addfeature);
    //delete a;
   // ui.pushButton_2->setEnabled(true);
    foreach(QgsMapLayer * index, m_mapcanvas->layers())
    {
        if(index->type()==QgsMapLayerType::RasterLayer)
            ui.comboBox_2->addItem(index->name());
    }

	ui.comboBox->addItem(codec->toUnicode("竹子"));
	ui.comboBox->addItem(codec->toUnicode("桉树"));
	ui.comboBox->addItem(codec->toUnicode("其他林地"));
	ui.comboBox->addItem(codec->toUnicode("草地/灌木"));
	ui.comboBox->addItem(codec->toUnicode("水体"));
	ui.comboBox->addItem(codec->toUnicode("建筑"));
    ui.comboBox->addItem(codec->toUnicode("裸地"));

   // QTextCodec* codec = QTextCodec::codecForName("GBK");//修改这两行
    QColor rgb1;
    rgb1.setRgb(237, 189, 101, 255);
    colortable.insert(pair<int, QColor>(0, rgb1));
    rgb1.setRgb(192, 192, 192, 255);
    colortable.insert(pair<int, QColor>(6, rgb1));
    rgb1.setRgb(0, 0, 255, 255);
    colortable.insert(pair<int, QColor>(9, rgb1));
    rgb1.setRgb(107, 142, 35, 255);
    colortable.insert(pair<int, QColor>(12, rgb1));
    rgb1.setRgb(34, 139, 34, 255);
    colortable.insert(pair<int, QColor>(13, rgb1));
    rgb1.setRgb(0, 255, 0, 255);
    colortable.insert(pair<int, QColor>(14, rgb1));
    rgb1.setRgb(255, 0, 0, 255);
    colortable.insert(pair<int, QColor>(15, rgb1));

    connect(m_addfeature, SIGNAL(creatfeature(QgsFeature&)), SLOT(write(QgsFeature&)));
    connect(this, SIGNAL(deletfeature(QgsFeature&)), m_addfeature,SLOT(deletfeature1(QgsFeature&)));
    connect(this, SIGNAL(selectfeature(QgsFeature&)), m_addfeature, SLOT(select(QgsFeature&)));
    connect(this, SIGNAL(cancleselect()), m_addfeature, SLOT(cancle()));
    connect(m_addfeature, SIGNAL(error(int)), SLOT(error(int)));//保存错误返回
    connect(this, SIGNAL(classificationactive()), m_addfeature, SLOT(classify()));
    connect(this, SIGNAL(output()), m_addfeature, SLOT(outputlayer()));
    connect(this, SIGNAL(clearall()), m_addfeature, SLOT(clearallsample()));
    connect(m_addfeature, SIGNAL(loadshp(QString)), this, SLOT(getloadshp(QString)));
    connect(m_addfeature, SIGNAL(newraster(QString)), this, SLOT(getrastershp(QString)));
    connect(m_addfeature, SIGNAL(progressvalue(int)), this, SLOT(setprogressvalue(int)));
    connect(m_addfeature, SIGNAL(progressmessage(QString&)), this, SLOT(setprogressmessage(QString&)));
    connect(m_addfeature, SIGNAL(progressfresh()), this, SLOT(setprogressfresh()));
    connect(m_addfeature, SIGNAL(progressshow(bool)), this, SLOT(setprogressshow(bool)));
    connect(this, SIGNAL(youhua()), m_addfeature, SLOT(youhua1()));
    connect(m_opensample, SIGNAL(openshp(QgsVectorLayer*, QString)), m_addfeature, SLOT(addfeaturefromshp(QgsVectorLayer*, QString)));
  //  delete codec;
}

void toolbar::setprogressvalue(int i)
{
    ui.progressBar->setValue(i);
}
void toolbar::setprogressmessage(QString& text)
{
    ui.textBrowser->insertPlainText(text+"\n");
    ui.textBrowser->moveCursor(QTextCursor::End);
}
void toolbar::setprogressfresh()
{
    ui.progressBar->reset();
}
void toolbar::getinformation(QgsMapCanvas* canvas, QList<QgsMapLayer*> mapCanvasLayerSet1)
{
	
}
void toolbar::layerchange(QString name)
{
    emit layerchanged(name);
}
void toolbar::click_pushButton_2()
{
    if (m_majority == NULL)
    {
        m_majority->fresh(m_mapcanvas);
        m_majority->show();
        m_majority->freshui1();
    }
    else
    {
        m_majority->fresh(m_mapcanvas);
        m_majority->show();
        m_majority->freshui1();
    }
        
}
void toolbar::click_deletButoon()
{
    QList<QTableWidgetSelectionRange> ranges = ui.tableWidget->selectedRanges();
    int count = ranges.count();
    for (int i1 = count-1; i1 >= 0; i1--)
    {
        for (int j = ranges[i1].bottomRow(); j >= ranges[i1].topRow(); j--)
        {
            ui.tableWidget->removeRow(j);
            QgsFeature aa = m_list.takeAt(j);// takeAt(selected.at(i).row());
            emit deletfeature(aa);
            i--;
        }
    }
}
void toolbar::xuanzhong(QModelIndex index)
{
    emit cancleselect();
    QList<QTableWidgetSelectionRange> ranges = ui.tableWidget->selectedRanges();
    int count = ranges.count();
    for (int i1 = count - 1; i1 >= 0; i1--)
    {
        for (int j = ranges[i1].bottomRow(); j >= ranges[i1].topRow(); j--)
        {
            QgsFeature aa = m_list.at(ranges[i1].topRow());
            emit selectfeature(aa);
        }
    }
}
void toolbar::clearsample()
{
    m_list.clear();
    ui.tableWidget->selectAll();
    QList<QTableWidgetSelectionRange> ranges = ui.tableWidget->selectedRanges();
    for (int i1 = ranges.count() - 1; i1 >= 0; i1--)
    {
        for (int j = ranges[i1].bottomRow(); j >= ranges[i1].topRow(); j--)
        {
            ui.tableWidget->removeRow(j);
            i--;
        }
    }
   // ui.tableView->clearSpans();
    emit clearall();
}
void toolbar::click_outputButton()
{
    emit output();
}
void toolbar::textchanged(QString name)
{
    QTextCodec* codec = QTextCodec::codecForName("GBK");//修改这两行
    QString a = ui.comboBox->currentText();
    if (ui.comboBox->currentText() == codec->toUnicode("竹子"))
        m_addfeature->setid(14);
    else if (ui.comboBox->currentText() == codec->toUnicode("桉树"))
        m_addfeature->setid(13);
    else if (ui.comboBox->currentText() == codec->toUnicode("草地/灌木"))
        m_addfeature->setid(12);
    else if (ui.comboBox->currentText() == codec->toUnicode("其他林地"))
        m_addfeature->setid(15);
    else if (ui.comboBox->currentText() == codec->toUnicode("水体"))
        m_addfeature->setid(9);
    else if (ui.comboBox->currentText() == codec->toUnicode("建筑"))
        m_addfeature->setid(6);
    else if (ui.comboBox->currentText() == codec->toUnicode("裸地"))
        m_addfeature->setid(0);
}
void toolbar::freshtable()
{
    m_list = m_addfeature->returnlist();

    foreach(QgsFeature index , m_list)
    {
        QString hang = QString::number(i);
        ui.tableWidget->setItem(i, 0, new QTableWidgetItem(index.attribute("id").toString()));
        ui.tableWidget->setItem(i, 1, new QTableWidgetItem(index.attribute("classname").toString()));
        ui.tableWidget->item(i, 1)->setBackground(colortable.find(index.attribute("id").toInt())->second);
        i++;
    }
}
void toolbar::write(QgsFeature& a)
{
    QString hang = QString::number(i);
    m_list.append(a);
    //m_model->setHeaderData(i, Qt::Vertical, hang);
    int row = ui.tableWidget->rowCount();
    ui.tableWidget->insertRow(row);
    ui.tableWidget->setItem(i-1, 0, new QTableWidgetItem(a.attribute("id").toString()));
    ui.tableWidget->setItem(i-1, 1, new QTableWidgetItem(a.attribute("classname").toString()));
    ui.tableWidget->item(i-1, 1)->setBackground(colortable.find(a.attribute("id").toInt())->second);
    i++;
}
void toolbar::error(int i)
{
    if(i==0)
        QMessageBox::critical(this, QStringLiteral("错误"), QStringLiteral("保存路径无效!\n"));

}
void toolbar::starclassification()
{

    if (m_classmangement == nullptr)
    {

        m_classmangement->show();
        m_classmangement->freshui1();
    }
    else
    {
        m_classmangement->show();
        m_classmangement->freshui1();
    }
    ui.checkBox->setChecked(true);
   // ui.progressBar->show();
    //ui.textBrowser->show();
    //m_mapcanvas->unsetMapTool(m_addfeature);
    ui.pushButton_2->setEnabled(true);
}
void toolbar::opensample()
{
    m_opensample->show();
}
void toolbar::pointclassify()
{
   /* CWuLasFile* file = new CWuLasFile();
    FPT3D_RGBA* rgbp;
    string name = "E:\\二次开发\\点云";
    bool openfile=file->Open("E:\\二次开发\\点云");
    if (openfile == true)
    {
        long total = file->GetPtSum();
        rgbp = new FPT3D_RGBA[8192];
        if (total > 8192)
        {
            for (int i = 0; i < total; i += )
            {
                ReadLas(LPCSTR lpstrLas, FPT3D_RGBA * pPts, LONG64 startPt, int ptSum, double* offX, double* offY, BOOL bMsg = FALSE);
                ReadLas(name.c_str(), rgbp, LONG64 startPt, int ptSum, double* offX, double* offY, BOOL bMsg = FALSE);
            }
            
        }
    }
    
    if(total>)
    CArray_FPT3D_RGBA* rgbpoint = new CArray_FPT3D_RGBA();*/
}