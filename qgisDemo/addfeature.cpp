#include "addfeature.h"

#include"qgsmaptooldigitizefeature.h "
#include"qgsadvanceddigitizingdockwidget.h"
#include"qgsmapcanvas.h"
#include"qgsmaptool.h"
#include"qgsmaptoolcapture.h"
#include"qgsfeature.h"
#include"qgsgraduatedsymbolrenderer.h"
#include<qgsgeometry.h>
#include <qgsrectangle.h>
#include <qgsfillsymbollayer.h>
#include <qgsclassificationmethod.h>
#include <qgsrendercontext.h>
#include <qgsrendererrange.h>

//#include"ogrsf_frmts.h"

#include"qfiledialog.h"
#include"qmessagebox.h"
#include"qgsvectorfilewriter.h"
#include "qgsvectordataprovider.h"
#include<string>
#include <qgscoordinatetransform.h>d
#include <stdio.h>
#include <stdlib.h>
#include<vector>
#include <assert.h>
//#include "SLinkList.h"
#include <initializer_list>
#include <list>
#include<map>
#include<math.h>
#include <utility>
#include <algorithm>
#include <qgsrasterblock.h>
//#define DEBUG_LINKLIST
#define pi 3.1415926
#include<iostream>
#include<fstream>
#include <qgspalettedrasterrenderer.h>
#include<qthread.h>

#include <Eigen/Core>
#include <Eigen\Dense> 
#include <QProgressDialog>
#include<omp.h>
#include"randomforest.h"
using namespace std;
using namespace Eigen;

addfeature::addfeature(QgsMapCanvas* canvas, QgsAdvancedDigitizingDockWidget* cadDockWidget, CaptureMode mode)
	:QgsMapToolDigitizeFeature(canvas, cadDockWidget,mode)
{
	QTextCodec* codec = QTextCodec::codecForName("GBK");//修改这两行
	foreach(QgsMapLayer * index, canvas->layers())
	{
		if (index->name() == codec->toUnicode("sample"))
		{
			m_layer = qobject_cast<QgsVectorLayer*>(index);
		}
	}
	connect(this, SIGNAL(digitizingCompleted(const QgsFeature&)), SLOT(addfeature1(const QgsFeature&)));
	id = 0;

	bool o = cadDockWidget->cadEnabled();
	if (o == false)
		cadDockWidget->enable();
	setLayer(canvas->currentLayer());
	mRenderer = new QgsGraduatedSymbolRenderer();
	activate();

}

void addfeature::addfeature1(const QgsFeature& fea)
{
	QTextCodec* codec = QTextCodec::codecForName("GBK");//修改这两行
	if (id == 14)
	{
		m_name = codec->toUnicode("竹子");
	}
	else if (id == 13)
	{
		m_name = codec->toUnicode("桉树");
	}
	else if (id == 12)
	{
		m_name = codec->toUnicode("草地/灌木");
	}
	else if (id == 11)
	{
		m_name = codec->toUnicode("电线");
	}
	else if(id==0)
	{
		m_name = codec->toUnicode("裸地");
	}
	else if (id == 9)
	{
		m_name = codec->toUnicode("水体");
	}
	else if (id == 6)
	{
		m_name = codec->toUnicode("建筑");
	}
	else if(id==15)
	{
		m_name = codec->toUnicode("其他林地");
	}
	QgsFeature* feature = new QgsFeature(m_layer->fields(), 0);
	feature->setGeometry(fea.geometry());
	for (int idx = 0; idx < m_layer->fields().count(); ++idx)
	{
		if (idx == 0)
		{
			QVariant v(id);
			feature->setAttribute(idx, v);
		}
		else
		{
			QVariant v(m_name);
			feature->setAttribute(idx, v);
		}
	}
	//m_layer->setRenderer(mRenderer);
	m_layer->addFeature(*feature);
	mCanvas->refresh();
	emit creatfeature(*feature);
	delete feature;
}

void addfeature::addfeaturefromshp(QgsVectorLayer* newshp, QString name)
{
	newshp->selectAll();
	QgsFeatureList newfeatures = newshp->selectedFeatures();
	int idnow = id;
	foreach(QgsFeature index, newfeatures)
	{
		id = index.attribute(name).toInt();
		addfeature1(index);
	}
	id = idnow;
}
void addfeature::InitField()
{
}
void addfeature::outputlayer()
{
	//m_layer->selectAll();
	QString fileName = QFileDialog::getSaveFileName(0, tr("Open Config"), "", tr("ESRI Shapefile (*.shp)"));
	if (fileName.isNull())
	{
		emit error(1);
		return;
	}
	else
	{
		//const QgsCoordinateTransformContext a1;
		//const QgsVectorFileWriter::SaveVectorOptions aa;
        QString code = "UTF-8";
        QString drivername = "ESRI Shapefile";
        const QgsCoordinateTransform a2;
 		QgsVectorFileWriter::WriterError a = QgsVectorFileWriter::writeAsVectorFormat(m_layer, fileName,code, a2,drivername);
	}
	emit loadshp(fileName);
}
void addfeature::lvboo(QList<QString> name)
{
	QgsRasterLayer* raster = nullptr;
	foreach(QgsMapLayer * index, mCanvas->layers())
	{
		if (index->name() == name[0])
		{
			raster = qobject_cast<QgsRasterLayer*>(index);
		}
	}
	lvbo(name[1].toInt(), name[2].toInt(), name[3].toInt(), raster, name[4]);
}
void addfeature::classify()
{
}
void addfeature::makelabletiff(QList<QString>& name)
{
	m_layer->removeSelection();
	emit progressfresh();
	emit progressshow(true);
	QTextCodec* codec = QTextCodec::codecForName("GBK");//修改这两行
	QString text = name.takeLast();
	if (text == codec->toUnicode("随机森林分类"))
	{
		QgsRasterLayer* raster = nullptr;
		QgsVectorLayer* shp = nullptr;
		foreach(QgsMapLayer * index, mCanvas->layers())
		{
			if (index->name() == name[0])
			{
				raster = qobject_cast<QgsRasterLayer*>(index);
			}
			if (index->name() == name[1])
			{
				shp = qobject_cast<QgsVectorLayer*>(index);
			}
			if (raster != nullptr && shp != nullptr)
			{
				break;
			}
		}
		int bands = raster->bandCount();
		int types = 7;
		random(shp, raster->dataProvider(), raster, name.last().toStdString(), bands, types, name[3], name[2], name[4], name[5].toInt());
	}
	if (text == codec->toUnicode("最大似然分类"))
	{
		QString fileName = name[2];
		QgsRasterLayer* raster = nullptr;
		QgsVectorLayer* shp = nullptr;
		foreach(QgsMapLayer * index, mCanvas->layers())
		{
			if (index->name() == name[0])
			{
				raster = qobject_cast<QgsRasterLayer*>(index);
			}
			if (index->name() == name[1])
			{
				shp = qobject_cast<QgsVectorLayer*>(index);
			}
			if (raster != nullptr && shp != nullptr)
			{
				break;
			}
		}
		QgsRasterDataProvider* data = raster->dataProvider();

	//	QgsRectangle angle = data->extent();
		maketext(shp, data, fileName, raster, name[3]);
	}

	/*for (int x = angle.xMinimum(); x < angle.xMaximum(); x++)
	{
		for (int y = angle.yMinimum(); y < angle.yMaximum(); y++)
		{
			QgsPointXY point(x,y);
			contain(shp, data, &point, fileName);
		}
	}*/
	

}
void addfeature::random(QgsVectorLayer* vector1, QgsRasterDataProvider* data, QgsRasterLayer* raster, string output, int& numband, int& types, QString& min_sample, QString& samp, QString& featurep,int nums)
{
	map<int, int>table;
	table.insert(pair<int, int>(0, 1));
	table.insert(pair<int, int>(6, 2));
	table.insert(pair<int, int>(9, 3));
	table.insert(pair<int, int>(12, 4));
	table.insert(pair<int, int>(13, 5));
	table.insert(pair<int, int>(14, 6));
	table.insert(pair<int, int>(15, 7));
	map<int, int>returntable;
	returntable.insert(pair<int, int>(1, 0));
	returntable.insert(pair<int, int>(2, 6));
	returntable.insert(pair<int, int>(3, 9));
	returntable.insert(pair<int, int>(4, 12));
	returntable.insert(pair<int, int>(5, 13));
	returntable.insert(pair<int, int>(6, 14));
	returntable.insert(pair<int, int>(7, 15));

	QThread* m_workerThread = new QThread();
	random1=new RandomForest(data, raster, output, numband, types, min_sample, samp, featurep, returntable);
	random1->moveToThread(m_workerThread);

	connect(random1, SIGNAL(clearmessage1()), SLOT(clear()));
	connect(random1, SIGNAL(setprogressvalue1(int)), SLOT(setvalue(int)));
	connect(random1, SIGNAL(setprogressmessage1(QString&)), SLOT(setmessage(QString&)));
	vector1->selectAll();
	QgsFeatureList listall = vector1->selectedFeatures();
	vector1->reselect();
	//进度条
	QTextCodec* codec = QTextCodec::codecForName("GBK");//修改这两行
	emit progressmessage(codec->toUnicode("正在统计样本数据......."));
	emit progressvalue(20);
	int bandsize = raster->bandCount();
	int featuresize = listall.size();
#pragma omp parallel for
	for (int u = 0; u < featuresize; u++)
	{

		//progressDlg1->setValue(50*u/featuresize);
		vector<VectorXd> sample;//样本暂存
		QgsFeature index = listall[u];//获取样本
		//定义向量并初始化为0
		vector <double> bands;
		//bands.setZero();
		QgsGeometry a = index.geometry();//样本的地理区域外界矩形
		QgsRectangle angel = a.boundingBox();
		//各个顶点的坐标
		double xmin = angel.xMinimum();
		double xmax = angel.xMaximum();
		double ymin = angel.yMinimum();
		double ymax = angel.yMaximum();
		QgsPoint min(xmin, ymax);
		QgsPoint minpoint = data->transformCoordinates(min, QgsRasterDataProvider::TransformType::TransformLayerToImage);//图像坐标
		QgsPoint max(xmax, ymin);
		QgsPoint maxpoint = data->transformCoordinates(max, QgsRasterDataProvider::TransformType::TransformLayerToImage);
		int xmin1 = minpoint.x();
		int xmax1 = maxpoint.x();
		int ymin1 = minpoint.y();
		int ymax1 = maxpoint.y();
		for (int x = xmin1; x < xmax1; x++)
		{
			for (int y = ymin1; y < ymax1; y++)
			{
				//点坐标转换
				QgsPoint linshi1(x, y);
				QgsPoint linshi = data->transformCoordinates(linshi1, QgsRasterDataProvider::TransformType::TransformImageToLayer);//地理坐标
				QgsPointXY linshi2(linshi);
				if (a.contains(&linshi2))
				{
					for (int b = 1; b <= bandsize; b++)
					{
						bands.push_back( data->sample(linshi2, b));
					}
#pragma omp critical
					random1->readImgTrain(bands, table.find(index.attribute("id").toInt())->second);
						
				}
				bands.clear();
			}
		}
	}
	QCoreApplication::processEvents();
	QString tiffname= random1->buildTree(nums);	
	emit progressmessage(codec->toUnicode("正在删除分类树......."));
	QCoreApplication::processEvents();
	delete random1;
	emit newraster(tiffname);
	emit progressvalue(100);
}
void addfeature::maketext(QgsVectorLayer* vector1, QgsRasterDataProvider* raster, QString txt,QgsRasterLayer *data,QString tiffname)
{
	vector1->selectAll();
	//qpolygon=feature;
	//Qvector<QPolygon>;
	QgsFeatureList listall = vector1->selectedFeatures();
	QFile make_each(txt);//在训练数据中写入数据集
	if (!make_each.open(QIODevice::WriteOnly | QIODevice::Append)) {
		QMessageBox::information(NULL, "Information", QString::fromLocal8Bit("文件打开失败"), QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
		//return false;
	}

	QTextStream write_file(&make_each);
	map<int, vector<VectorXd>> mysymple;//样本储存
	map<int, VectorXd> mymean;//均值储存
	map<int, MatrixXd> myvar;//协方差储存
	map<int, int>lable;

	QTextCodec* codec = QTextCodec::codecForName("GBK");//修改这两行
	//进度条

	//progressdialog->show();
	emit progressmessage(codec->toUnicode("正在统计样本数据......."));
	emit progressvalue(20);

	int bandsize = raster->bandCount();
	int featuresize = listall.size();
	vector1->reselect();
#pragma omp parallel for
	for (int u = 0; u < featuresize; u++)
	{
		//progressDlg1->setValue(50*u/featuresize);
		vector<VectorXd> sample;//样本暂存
		QgsFeature index = listall[u];//获取样本
		//定义向量并初始化为0
		VectorXd bands(bandsize);

		bands.setZero();
		QgsGeometry a = index.geometry();//样本的地理区域外界矩形
		QgsRectangle angel = a.boundingBox();//
		//各个顶点的坐标
		double xmin = angel.xMinimum();
		double xmax = angel.xMaximum();
		double ymin = angel.yMinimum();
		double ymax = angel.yMaximum();
	//	QPointF = QgsPoint;
		QgsPoint min(xmin, ymax);
		//
		QgsPoint minpoint = raster->transformCoordinates(min, QgsRasterDataProvider::TransformType::TransformLayerToImage);//图像坐标
		QgsPoint max(xmax, ymin);
		QgsPoint maxpoint = raster->transformCoordinates(max, QgsRasterDataProvider::TransformType::TransformLayerToImage);
		int xmin1 = minpoint.x();
		int xmax1 = maxpoint.x();
		int ymin1 = minpoint.y();
		int ymax1 = maxpoint.y();
		for (int x = xmin1; x < xmax1; x++)
		{
			for (int y = ymin1; y < ymax1; y++)
			{
				//点坐标转换
				QgsPoint linshi1(x, y);
				QgsPoint linshi = raster->transformCoordinates(linshi1, QgsRasterDataProvider::TransformType::TransformImageToLayer);//地理坐标
				QgsPointXY linshi2(linshi);
				if (a.contains(&linshi2))
				{
					for (int b = 1; b <= bandsize; b++)
					{
						bands[b - 1] = raster->sample(linshi2, b);
					}
					if (mysymple.count(index.attribute("id").toInt()) == 0)

					{
						sample.push_back(bands);
#pragma omp critical
						{mysymple.insert(pair<int, vector<VectorXd>>(index.attribute("id").toInt(), sample)); }
						sample.clear();
						sample.swap(vector<VectorXd>(sample));
					}
					else
#pragma omp critical
					{
						mysymple.find(index.attribute("id").toString().toInt())->second.push_back(bands);
					}
				}

			}
		}
		/*if (a.contains(&ceshi))
		{
			write_file << index.attribute("id").toString() << ",";
			for (int i = 0; i < raster->bandCount(); i++)
			{
				write_file << raster->sample(ceshi, i) << ",";// "," << g << "," << b << "," << mnCurrentIndex << "," << mqvecClassifyNames[mnCurrentIndex] << endl;
			}
		}*/
	}
	QCoreApplication::processEvents();
	int  oo = 0;
	int symplenumber = mysymple.size();//样本个数
	emit progressmessage(codec->toUnicode("正在计算样本数据特征......."));


	for (auto iter = mysymple.begin(); iter != mysymple.end(); iter++)//map遍历
	{

		emit progressvalue(20 + 30 * oo / mysymple.size());
		QCoreApplication::processEvents();
		write_file << iter->first << ";";
		//定义临时存储并初始化
		VectorXd total(bandsize);
		total.setZero();
		VectorXd mean(bandsize);
		mean.setZero();
		MatrixXd var(bandsize, bandsize);//设置于此避免出现副本问题
		var.setZero();
		MatrixXd totalvar(bandsize, bandsize);
		totalvar.setZero();
		int pointsize = iter->second.size();//样本点个数
#pragma omp parallel for
		for (int i = 0; i < pointsize; i++)//一个类的所有像素点遍历
		{
			// vector<float> lishi1 = lishi[i];
#pragma omp critical
				total += iter->second[i];
		}
		total /= iter->second.size();
		for(int i=0;i<bandsize;i++)
			write_file << total[i] << " ";
		mean = total;
		write_file << ";";
#pragma omp parallel for
		for (int i = 0; i < pointsize; i++)//计算方差的上面
		{
#pragma omp critical
			totalvar = totalvar + (iter->second[i] - mean) * (iter->second[i] - mean).transpose();
		}
		//var = totalvar.inverse();
		var=(totalvar /(iter->second.size()-1));
		for (int i = 0; i < bandsize; i++)
			for (int u = 0; u < bandsize; u++)
				write_file << var(i,u) << " ";
		write_file << endl;
		mymean.insert(pair<int, VectorXd>(iter->first, mean));
		myvar.insert(pair<int, MatrixXd>(iter->first, var));
		lable.insert(pair<int, int>(oo, iter->first));
		oo++;
	}
	mysymple.clear();
	mysymple.swap(mysymple);
	int samplnum = mymean.size();

/*	for (int i = 0; i < samplnum; i++)
	{
		percent[i] = 0;
	}*/

	//创建分类结果图像
	QgsRasterBlock *result=new QgsRasterBlock(Qgis::DataType::UInt32, raster->xSize(), raster->ySize());
	emit progressmessage(codec->toUnicode("正在进行最大似然分类......."));
	//progressdialog->setmessage(codec->toUnicode("正在进行最大似然分类......."));
	int xsize = raster->xSize();
	int ysize = raster->ySize();
	double* percent = new double[samplnum];
	for (int x = 0; x < xsize; x++)
	{
		emit progressvalue(50 + 50 * x / xsize);
		QCoreApplication::processEvents();
		//progressdialog->setprogressvalue(50 + 50 * x / xsize);
		for (int y = 0; y < ysize; y++)
		{
			QgsPoint point(x, y);
			QgsPoint point1 = raster->transformCoordinates(point, QgsRasterDataProvider::TransformType::TransformImageToLayer);
			QgsPointXY pointxy(point1);
			if (raster->sample(pointxy, 1) == 0)
			{
				result->setValue(y, x, 255); 
				//	result->setColor(y, x, rgb.find(0)->second);
				goto label;
			}
			else
			{
				auto iter = mymean.begin();
				VectorXd data1(bandsize);
				data1.setZero();
				for (int y1 = 0; y1 < samplnum; y1++)
				{	
					for (int b = 1; b <= bandsize; b++)
					{
						int b1 = b - 1;
						data1[b1] = raster->sample(pointxy, b);
					}
					//VectorXd xx = data1 - iter->second;
				//	MatrixXd xx1 = myvar.find(iter->first)->second.adjoint() / myvar.find(iter->first)->second.determinant();
					//MatrixXd xx2=xx1.inverse();
					//double e11;
					//e11 = xx.transpose() * xx1 * xx;
					double e1 = (data1 - iter->second).transpose() * myvar.find(iter->first)->second.inverse() * (data1 - iter->second);
					double e2 = log(myvar.find(iter->first)->second.determinant());
					percent[y1] = -e1 / 2 - e2; 
					iter++;
				}
		/*		for (auto iter = mymean.begin(); iter != mymean.end(); iter++)
				{
					
				}*/
				int num = max_element(percent, percent + samplnum) - percent;
				result->setValue(y, x, lable.find(num)->second); 
				/*for (int p = 0; p < samplnum; p++)
				{
					auto iter = mymean.begin();
					for (int io = 0; io < p; io++)
					{
						iter++;
					}
					if (percent[p] == *max_element(percent, percent + samplnum))
					{
						result->setValue(y, x, iter->first);
						break;
					}
				}*/
				
			}
			/*for (auto iter1 = mymean.begin(); iter1 != mymean.end(); iter1++)v
			{
				if (percent[i] == *max_element(percent, percent + mymean.size()))
				{
					int it = iter1->first;
					result->setValue(y, x, it);
				//	result->setColor(y, x, rgb.find(iter1->first)->second);
				}
				i++;
			}*/
		label:;
		}
	}
	delete[]percent;
	QCoreApplication::processEvents();
	emit progressmessage(codec->toUnicode("分类结束"));
	QgsRectangle extent_result = data->extent();
	int nCols = data->width();
	int nRows = data->height();
	qDebug() << nCols << endl << nRows << endl;
	double tform[] =
	{
		extent_result.xMinimum(),extent_result.width() / nCols,0.0,
		extent_result.yMaximum(),0.0,-extent_result.height() / nRows
	};
	QgsRasterDataProvider* dp = QgsRasterDataProvider::create(QStringLiteral("gdal"),
		tiffname,
		QStringLiteral("GTiff"),
		1,
		Qgis::DataType::UInt32,
		raster->xSize(),
		raster->ySize(),
		tform,
		data->crs()
	);
	dp->writeBlock(result, 1);
	delete dp;
	delete result;// 这两一定要释放，不然程序不关闭，图像block导入不了

	emit progressshow(false);
	//x=new QgsPalettedRasterRenderer(raster, 1, classdata);
	emit newraster(tiffname);
	emit progressvalue(100);
}
void addfeature::youhua1()
{
}
void addfeature::lvbo(int wide, int high, int dpi, QgsRasterLayer* raster, QString tiffname)
{
	QgsRasterDataProvider* data = raster->dataProvider();
	int xsize = data->xSize();
	int ysize = data->ySize();
	int iyy = wide / 2;
	int ixx = high / 2;
	map<int, int>num;
	QgsRasterBlock* result = new QgsRasterBlock(Qgis::DataType::UInt32, data->xSize(), data->ySize());
	QTextCodec* codec = QTextCodec::codecForName("GBK");//修改这两行
	emit progressfresh();
	emit progressmessage(codec->toUnicode("正在进行结果优化......."));

		for (int x = 0; x < xsize; x++)
		{
			emit progressvalue(100 * x / xsize);
			QCoreApplication::processEvents();
		//		omp_set_nested(1);
	//#pragma omp parallel for
			for (int y = 0; y < ysize; y++)
			{
				if (x<=iyy|| x>=xsize - iyy || y<=ixx || y>=ysize - ixx)//x == 0 || y == 0 || x == xsize - 1 || y == ysize - 1)//|| y = ysize - 1)
				{
					result->setValue(y, x, 255);
				}
				else
				{

					for (int iy = 0; iy < wide; iy++)
					{
						for (int ix = 0; ix < high; ix++)
						{

						QgsPoint point(x + iy - iyy, y + ix - ixx);
						QgsPoint point1 = data->transformCoordinates(point, QgsRasterDataProvider::TransformType::TransformImageToLayer);
						QgsPointXY pointxy(point1);
						int e = data->sample(pointxy, 1);
						if (e == 255)
						{
							QgsPoint point(x, y);
							QgsPoint point1 = data->transformCoordinates(point, QgsRasterDataProvider::TransformType::TransformImageToLayer);
							QgsPointXY pointxy(point1);
							result->setValue(y, x, data->sample(pointxy, 1));
							goto lan;
						}
						else
						{
							if (num.count(e) == 0)
							{
								if (iy == wide / 2 && ix == high / 2)
									num.insert(pair<int, int>(e, dpi));
								else
									num.insert(pair<int, int>(e, 1));
							}
							else
							{
								if (iy == wide / 2 && ix == high / 2)
									num.find(e)->second += dpi;
								else
									num.find(e)->second += 1;
							}
						}
					}
				}
				int key = 0;
				if (num.size() > 0)
				{
					auto iter = num.begin();
					key = iter->first;
					for (int i = 0; i < num.size() - 1; i++)
					{
						int num1 = iter->second;
						iter++;
						int num2 = iter->second;
						if (num2 > num1)
							key = iter->first;
					}
					result->setValue(y, x, key);
				}
				lan:
				num.clear();
			}
		}
	}
	QCoreApplication::processEvents();
	emit progressmessage(codec->toUnicode("优化结束"));
		QgsRectangle extent_result = data->extent();
		int nCols = raster->width();
		int nRows = raster->height();
		qDebug() << nCols << endl << nRows << endl;
		double tform[] =
		{
			extent_result.xMinimum(),extent_result.width() / nCols,0.0,
			extent_result.yMaximum(),0.0,-extent_result.height() / nRows
		};
		QgsRasterDataProvider* dp = QgsRasterDataProvider::create(QStringLiteral("gdal"),
			tiffname,
			QStringLiteral("GTiff"),
			1,
			Qgis::DataType::UInt32,
			data->xSize(),
			data->ySize(),
			tform,
			raster->crs()
		);
		dp->writeBlock(result, 1);
		delete result;// 这两一定要释放，不然程序不关闭，图像block导入不了
		delete dp;
		//x = new QgsPalettedRasterRenderer(data, 1, classdata);
		emit newraster(tiffname);
		emit progressvalue(100);
	
}
void addfeature::classification(QString txt, QgsRasterDataProvider* raster)
{
	const char* path = txt.toStdString().c_str();
	ifstream ifs(path);
	string str;
	int count = 0;

	while (ifs >> str)
	{
		cout << str << endl;
		count++;
	}
	ifs.close();

	return ;

}
bool addfeature::contain(QgsVectorLayer* vector,QgsRasterDataProvider* raster, QgsPointXY* piont,QString txt)
{
	vector->selectAll();
	QgsFeatureList listall = vector->selectedFeatures();

	QFile make_each(txt);//在训练数据中写入数据集
	if (!make_each.open(QIODevice::WriteOnly | QIODevice::Append)) {
		QMessageBox::information(NULL, "Information", QString::fromLocal8Bit("文件打开失败"), QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
		return false;
	}
	QTextStream write_file(&make_each);
	foreach(QgsFeature index, listall)
	{
		QgsGeometry a = index.geometry();
		if (a.contains(piont))
		{
			write_file << index.attribute("id").toString()<<",";
			for (int i = 0; i < raster->bandCount(); i++)
			{
				write_file << raster->sample(*piont,i)<<",";// "," << g << "," << b << "," << mnCurrentIndex << "," << mqvecClassifyNames[mnCurrentIndex] << endl;
			}
			return true;
		}
	}
	return false;
}
void addfeature::classifyaction(QList<QString> a)
{
	makelabletiff(a);
}