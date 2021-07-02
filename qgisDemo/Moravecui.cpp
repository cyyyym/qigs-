#include "Moravecui.h"
#include"qgsmapcanvas.h"
#include"qtextcodec.h"
#include"qfiledialog.h"
#include"qmessagebox.h"
#include"qgsvectorlayer.h"
#include"qgsrasterlayer.h"
#include<vector>
#include"qgspoint.h"
#include"qmath.h"
#include <Eigen/Core>
#include <Eigen\Dense> 
using namespace std;
using namespace Eigen;
Moravecui::Moravecui(QgsMapCanvas*& canvas, QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	m_canvas = canvas;
	freshui1();
	ui.horizontalSlider->hide();
	ui.spinBox->hide();
	ui.pushButton_3->hide();
	connect(ui.spinBox, SIGNAL(valueChanged(int)), ui.horizontalSlider, SLOT(setValue(int)));
	connect(ui.horizontalSlider, SIGNAL(valueChanged(int)), ui.spinBox, SLOT(setValue(int)));
}
void Moravecui::freshui(QgsMapCanvas*& can)
{
	m_canvas = can;
	freshui1();
}
void Moravecui::suanzi(QString text)
{
	if (text =="Moravec" )
	{
		ui.label_2->setText(QStringLiteral("卷积核长"));
		ui.label_3->setText(QStringLiteral("卷积核宽"));
		//ui.toolButton_2->hide();
	}
	if (text == "Harris")
	{
		ui.label_2->setText(QStringLiteral("高斯卷积核大小（必须为奇数）"));
		ui.label_3->setText(QStringLiteral("兴趣值K（0.04-0.06）"));
	}
	if (text == "GeneralizedHoughtransform")
	{
		ui.label_2->hide();// setText(GeneralizedHoughtransform("高斯卷积核大小（必须为奇数）"));
		ui.label_3->hide();// setText(QStringLiteral("兴趣值K（0.04-0.06）"));
		ui.label_4->setText(QStringLiteral("边界提取阈值"));
		ui.lineEdit_2->hide();
		ui.lineEdit_3->hide();
		ui.label_6->setText(QStringLiteral("霍夫变换精度（1-180整数）"));
	}
}
void Moravecui::freshui1()
{
	ui.comboBox->clear();
	ui.comboBox_2->clear();
	foreach(QgsMapLayer * index, m_canvas->layers())
	{
		if (index->type() == QgsMapLayerType::RasterLayer)
		{
			ui.comboBox->addItem(index->name());
		}
	}
	ui.comboBox_2->addItem("Moravec");
	ui.comboBox_2->addItem("Harris");
	ui.comboBox_2->addItem("GeneralizedHoughtransform");
}
void Moravecui::save()
{
	QString fileName = QFileDialog::getSaveFileName(0, tr("save"), "", tr(" (*.tif)"));
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
void Moravecui::yunsuan()
{
	if (ui.comboBox_2->currentText() == "Moravec")
	{
		QgsMapLayer* a = NULL;
		foreach(QgsMapLayer * index, m_canvas->layers())
		{
			if (index->name() == ui.comboBox->currentText())
			{
				a = index;
			}
		}
		QgsRasterLayer* raster = qobject_cast<QgsRasterLayer*> (a);
		QgsRasterDataProvider* data = raster->dataProvider();
		int xsize = data->xSize();
		int ysize = data->ySize();
		int wide = ui.lineEdit_2->text().toInt();
		int high = ui.lineEdit_3->text().toInt();
		int yuzhi = ui.lineEdit_4->text().toInt();
		int iyy = wide / 2;
		int ixx = high / 2;
		QgsRasterBlock* block = data->block(1, data->extent(), xsize, ysize);
		QgsRasterBlock* result = new QgsRasterBlock(Qgis::DataType::UInt32, data->xSize(), data->ySize());
		QTextCodec* codec = QTextCodec::codecForName("GBK");//修改这两行
		for (int x = 0; x < xsize; x++)
		{
			QCoreApplication::processEvents();
			//		omp_set_nested(1);
		//#pragma omp parallel for
			for (int y = 0; y < ysize; y++)
			{
				if (x <= iyy || x >= xsize - iyy || y <= ixx || y >= ysize - ixx)//x == 0 || y == 0 || x == xsize - 1 || y == ysize - 1)//|| y = ysize - 1)
				{
					result->setValue(y, x, 255);
				}
				else
				{
					double mer[4] = { 0,0,0,0 };
					int cen = block->value( y + (high - 1) / 2 - ixx, x + (wide - 1) / 2 - iyy);
					for (int iy = 0; iy < wide; iy++)
					{
						for (int ix = 0; ix < high; ix++)
						{
							int e = block->value(y + ix - ixx, x + iy - iyy);
							if (e == 255)
							{
								result->setValue(y,x, 255);
								goto lan;
							}
							else
							{
								if (ix == iy)
								{
									mer[0] += (e - cen) * (e - cen);
								}
								else if (ix == (high - 1) / 2)
								{
									mer[1] += (e - cen) * (e - cen);
								}
								else if (iy == (wide - 1) / 2)
								{
									mer[3] += (e - cen) * (e - cen);
								}
								else if ((iy + ix) == wide || (iy > (wide - 1) & ix > (high - 1) / 2))
								{
									mer[2] += (e - cen) * (e - cen);
								}
							}
						}
					}
					int num = *min_element(mer, mer + 4);
					if (num > yuzhi)
					{
						result->setValue(y, x, num);
					}
					else
					{
						result->setValue(y, x, 255);
					}
				}
			lan:;
			}
		}
		QgsRectangle extent_result = data->extent();
		int nCols = raster->width();
		int nRows = raster->height();
		double tform[] =
		{
			extent_result.xMinimum(),extent_result.width() / nCols,0.0,
			extent_result.yMaximum(),0.0,-extent_result.height() / nRows
		};
		QgsRasterDataProvider* dp = QgsRasterDataProvider::create(QStringLiteral("gdal"),
			ui.lineEdit_5->text(),
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
		QString filename = ui.lineEdit_5->text();
		QStringList temp = filename.split(QDir::separator());
		QString basename = temp.at(temp.size() - 1);
		QgsRasterLayer::LayerOptions layerOption;
		layerOption.loadDefaultStyle = false;
		QgsRasterLayer* rasterLayer1 = new QgsRasterLayer(filename, basename, "gdal", layerOption);
		data = rasterLayer1->dataProvider();
		block = data->block(1, data->extent(), xsize, ysize);
		int sizee = ui.lineEdit->text().toInt();
		iyy = sizee / 2;
		ixx = sizee / 2;
		QgsRasterBlock* result2 = new QgsRasterBlock(Qgis::DataType::UInt32, data->xSize(), data->ySize());
		for (int x = 0; x < xsize; x++)
		{
			QCoreApplication::processEvents();
			//		omp_set_nested(1);
		//#pragma omp parallel for
			for (int y = 0; y < ysize; y++)
			{
				if (x <= iyy || x >= xsize - iyy || y <= ixx || y >= ysize - ixx)//x == 0 || y == 0 || x == xsize - 1 || y == ysize - 1)//|| y = ysize - 1)
				{
					result2->setValue(y, x, 255);
				}
				else
				{
					vector<int>dataa;
					vector<QgsPointXY>pointall;
					for (int iy = 0; iy < sizee; iy++)
					{
						for (int ix = 0; ix < sizee; ix++)
						{
							QgsPoint point(x + iy - iyy, y + ix - ixx);
							//QgsPoint point1 = data->transformCoordinates(point, QgsRasterDataProvider::TransformType::TransformImageToLayer);
							//int e = data->sample(pointxy, 1);
							int e = block->value(y + ix - ixx,x + iy - iyy);
							if (e == 255)
							{
							}
							else
							{
								dataa.push_back(e);
								pointall.push_back(QgsPointXY(point));
							}
							result2->setValue(point.y(), point.x(), 255);
						}

					}
					if (dataa.size() != 0)
					{
						int num = max_element(dataa.begin(), dataa.end()) - dataa.begin();
						result2->setValue(pointall[num].y(), pointall[num].x(), 0);
					}
				}
			}
			//x = new QgsPalettedRasterRenderer(data, 1, classdata);
		}
		int namee = ui.lineEdit_5->text().length();
		QString name1 = ui.lineEdit_5->text().left(namee - 4) + "result.tif";
		QgsRasterDataProvider* dp1 = QgsRasterDataProvider::create(QStringLiteral("gdal"),
			name1,
			QStringLiteral("GTiff"),
			1,
			Qgis::DataType::UInt32,
			data->xSize(),
			data->ySize(),
			tform,
			raster->crs()
		);
		dp1->writeBlock(result2, 1);
		delete result2;// 这两一定要释放，不然程序不关闭，图像block导入不了
		delete dp1;
		emit rraster(name1);
	}
	if (ui.comboBox_2->currentText() == "Harris")
	{
		QgsMapLayer* a = NULL;
		foreach(QgsMapLayer * index, m_canvas->layers())
		{
			if (index->name() == ui.comboBox->currentText())
			{
				a = index;
			}
		}
		QgsRasterLayer* raster = qobject_cast<QgsRasterLayer*> (a);
		QgsRasterDataProvider* data = raster->dataProvider();
		int xsize = data->xSize();
		int ysize = data->ySize();
		int wide = ui.lineEdit_2->text().toInt();
		int high = ui.lineEdit_2->text().toInt();
		int iyy = wide / 2;
		int ixx = high / 2;
		double** gaosi = new double* [wide];
		for (int i = 0; i < wide; i++)
		{
			gaosi[i] = new double[wide];
		}
		for (int i = 0; i < wide; i++)
		{
			for (int y = 0; y < wide; y++)
			{
				gaosi[i][y] = exp(-((i - wide / 2) * (i - wide / 2) + (y - wide / 2) * (y - wide / 2)) / (2 * 1.4 * 1.4)) / (2 * 3.1415926 * 1.4 * 1.4);
			}
		}
		QgsRasterBlock* block = data->block(1, data->extent(), xsize, ysize);
		double k = ui.lineEdit_3->text().toDouble();
		MatrixXf ixm(xsize - 2, ysize - 2);
		MatrixXf iym(xsize - 2, ysize - 2);
		MatrixXf ixy(xsize - 2, ysize - 2);
		iyy = 1;
		ixx = 1;
		for (int x = 0; x < xsize; x++)
		{
			QCoreApplication::processEvents();
			//		omp_set_nested(1);
		//#pragma omp parallel for
			for (int y = 0; y < ysize; y++)
			{
				if (x < iyy || x >= xsize - iyy || y < ixx || y >= ysize - ixx)//x == 0 || y == 0 || x == xsize - 1 || y == ysize - 1)//|| y = ysize - 1)
				{
					//result2->setValue(y, x, 0);
				}
				else
				{
					float xt = block->value(x + 1, y) - block->value(x - 1, y);
					float yt = block->value(x, y - 1) - block->value(x, y + 1);
					ixm(x - iyy, y - ixx) = xt * xt;
					iym(x - iyy, y - ixx) = yt * yt;
					ixy(x - iyy, y - ixx) = xt * yt;
				}
			}
			//x = new QgsPalettedRasterRenderer(data, 1, classdata);
		}
		iyy = wide / 2;
		ixx = wide / 2;
		xsize -= 2;
		ysize -= 2;

		MatrixXf ix2g(xsize - 2 * ixx, ysize - 2 * iyy);
		MatrixXf iy2g(xsize - 2 * ixx, ysize - 2 * iyy);
		MatrixXf ixy2g(xsize - 2 * ixx, ysize - 2 * iyy);
		//int yuzhi = ui.lineEdit_4->text().toInt();
		for (int x = 0; x < xsize; x++)
		{
			QCoreApplication::processEvents();
			//		omp_set_nested(1);
		//#pragma omp parallel for
			for (int y = 0; y < ysize; y++)
			{
				if (x <= iyy || x >= xsize - iyy || y <= ixx || y >= ysize - ixx)//x == 0 || y == 0 || x == xsize - 1 || y == ysize - 1)//|| y = ysize - 1)
				{
				}
				else
				{
					float lastdatax = 0;
					float lastdatay = 0;
					float lastdataxy = 0;
					for (int ix = 0; ix < wide; ix++)
					{
						for (int iy = 0; iy < wide; iy++)
						{
							lastdatax += ixm(x + iy - iyy, y + ix - ixx) * gaosi[ix][iy];
							lastdatay += iym(x + iy - iyy, y + ix - ixx) * gaosi[ix][iy];
							lastdataxy += ixy(x + iy - iyy, y + ix - ixx) * gaosi[ix][iy];
						}
					}
					ix2g(x - ixx, y - iyy) = lastdatax;
					iy2g(x - ixx, y - iyy) = lastdatay;
					ixy2g(x - ixx, y - iyy) = lastdataxy;
				}
			}
		}
		//计算R
		k = ui.lineEdit_3->text().toFloat();
		wide = ui.lineEdit->text().toInt();
		MatrixXf res(xsize - 2 * ixx, ysize - 2 * ixx);
		res.setZero();
		ixx = wide / 2;
		iyy = wide / 2;
		xsize -= 2 * ixx;
		ysize -= 2 * iyy;
		QgsRasterBlock* block1 = new QgsRasterBlock(Qgis::DataType::Float32, xsize, ysize);
		for (int x = 0; x < xsize; x++)
		{
			QCoreApplication::processEvents();
			//		omp_set_nested(1);
		//#pragma omp parallel for
			for (int y = 0; y < ysize; y++)
			{
				MatrixXf h(2, 2);
				h << ix2g(x, y), ixy2g(x, y),
					ixy2g(x, y), iy2g(x, y);
				res(x, y) = h.determinant() - k * h.trace();
			}
		}
		float yuzhi = res.maxCoeff() * 0.2;
		for (int x = 0; x < xsize; x++)
		{
			for (int y = 0; y < ysize; y++)
			{
				vector<float> dataaa;
				vector<pair<int, int>>point;
				if (x <= iyy || x >= xsize - iyy || y <= ixx || y >= ysize - ixx)//x == 0 || y == 0 || x == xsize - 1 || y == ysize - 1)//|| y = ysize - 1)
				{
					block1->setValue(x, y, 255);
				}
				else
				{
					for (int ix = 0; ix < wide; ix++)
					{
						for (int iy = 0; iy < wide; iy++)
						{
							if (res(x + ix - ixx, y + iy - iyy) > yuzhi)
							{
								dataaa.push_back(res(x + ix - ixx, y + iy - iyy));
								point.push_back(pair<int, int>(x + ix - ixx, y + iy - iyy));
							}
						}
					}
				}
				if (dataaa.size() != 0)
				{
					int num = max_element(dataaa.begin(), dataaa.end()) - dataaa.begin();
					block1->setValue(point[num].first,point[num].second, 0);
				}
				else
					block1->setValue(x, y, 255);
			}
		}
		QgsRectangle extent_result = data->extent();
		int nCols = raster->width();
		int nRows = raster->height();
		double tform[] =
		{
			extent_result.xMinimum(),extent_result.width() / nCols,0.0,
			extent_result.yMaximum(),0.0,-extent_result.height() / nRows
		};
		QgsRasterDataProvider* dp = QgsRasterDataProvider::create(QStringLiteral("gdal"),
			ui.lineEdit_5->text(),
			QStringLiteral("GTiff"),
			1,
			Qgis::DataType::Float32,
			data->xSize(),
			data->ySize(),
			tform,
			raster->crs()
		);
		dp->writeBlock(block1, 1);
		delete block1;// 这两一定要释放，不然程序不关闭，图像block导入不了
		delete dp;
		emit rraster(ui.lineEdit_5->text());

	}
	if (ui.comboBox_2->currentText() == "GeneralizedHoughtransform")
	{
		QgsMapLayer* a = NULL;
		foreach(QgsMapLayer* index, m_canvas->layers())
		{
			if (index->name() == ui.comboBox->currentText())
			{
				a = index;
			}
		}
		QgsRasterLayer* raster = qobject_cast<QgsRasterLayer*> (a);
		QgsRasterDataProvider* data = raster->dataProvider();
		int col = data->xSize();//列数
		int row = data->ySize();//行数
		int iyy = 1;
		int ixx = 1;
		double degree = ui.lineEdit->text().toDouble();
		degrees = degree;
		int edgeyuzhi = ui.lineEdit_4->text().toInt();
		QgsRasterBlock* block = data->block(1, data->extent(), col, row);
		QgsRasterBlock* result = new QgsRasterBlock(Qgis::DataType::UInt32, col, row);
		QgsRasterBlock* result1 = new QgsRasterBlock(Qgis::DataType::UInt32, col, row);
		int Sobel1[3][3] = { -1,0,1,-2,0,2,-1,0,1 };
		int Sobel2[3][3] = { 1,2,1,0,0,0,-1,-2,-1 };
		for (int x = 0; x < col; x++)
		{
			QCoreApplication::processEvents();
			//		omp_set_nested(1);
		//#pragma omp parallel for
			for (int y = 0; y < row; y++)
			{
				if (x < iyy || x > col - iyy || y <= ixx || y >= row - ixx)//x == 0 || y == 0 || x == xsize - 1 || y == ysize - 1)//|| y = ysize - 1)
				{
					result->setValue(x, y, 255);
					result1->setValue(x, y, 255);
				}
				else
				{
					int edge1 = 0;
					int edge2 = 0;
					for (int iy = 0; iy < 3; iy++)
					{
						for (int ix = 0; ix < 3; ix++)
						{
							//QgsPoint point1 = data->transformCoordinates(point, QgsRasterDataProvider::TransformType::TransformImageToLayer);
							//int e = data->sample(pointxy, 1);
							edge1 += block->value(x + iy - iyy, y + ix - ixx) * Sobel1[iy][ix];
							edge2 += block->value(x + iy - iyy, y + ix - ixx) * Sobel2[iy][ix];

						}

					}
					if (abs(edge1) + abs(edge2) > edgeyuzhi)
					{
						result->setValue(x, y, 0);
						result1->setValue(x, y, 0);

					}
					else
					{
						result->setValue(x, y, 255);
						result1->setValue(x, y, 255);
					}
				}
			}
			//x = new QgsPalettedRasterRenderer(data, 1, classdata);
		}
		QgsRectangle extent_result = data->extent();
		int nCols = raster->width();
		int nRows = raster->height();
		double tform[] =
		{
			extent_result.xMinimum(),extent_result.width() / nCols,0.0,
			extent_result.yMaximum(),0.0,-extent_result.height() / nRows
		};
		int namee = ui.lineEdit_5->text().length();
		QString name1 = ui.lineEdit_5->text().left(namee - 4) + "edge.tif";
		QgsRasterDataProvider* dp = QgsRasterDataProvider::create(QStringLiteral("gdal"),
			name1,
			QStringLiteral("GTiff"),
			1,
			Qgis::DataType::UInt32,
			data->xSize(),
			data->ySize(),
			tform,
			raster->crs()
		);
		dp->writeBlock(result, 1);
	//	QgsRasterBlock* result1 =new QgsRasterBlock();
		//result1 = result;
		delete result;// 这两一定要释放，不然程序不关闭，图像block导入不了
		delete dp;
		emit rraster(name1);
		int i, j;
		//行列
		//极径最大值为 对角线+宽
		max_r = col+sqrt(double(row * row + col * col));
		//累加器 三角函数
		MatrixXd line_cnt(degrees, max_r);
		line_cnt.setZero();
		sin_ = new double[degree];
		cos_ = new double[degree];
		double rad_ = 3.1415926 / degree;
		for (i = 0; i < degree; i++)
		{
			//初始化累加器为0
			//初始化三角函数
			sin_[i] = sin(i * rad_);
			cos_[i] = cos(i * rad_);
		}

		//极径 极角
		int r = 0;
		int theta = 0;
		//遍历图像 判断并进行累加
		for (i = 0; i < row; i++)
		{
			for (j = 0; j < col; j++)
			{
				if (result1->value(j,i) == 0)
				{
					for (theta = 0; theta < degrees; theta++)
					{
						//极坐标 直线方程
						r =round(j * cos_[theta] + i * sin_[theta]);
						//偏移 无负值
						r = r+col;
						line_cnt(theta,r)+=1;
					}
				}
			}
		}
		int nMin = line_cnt.minCoeff();
		int nMax = line_cnt.maxCoeff();
		int nSingleStep = 1;
		line_cnt1 = line_cnt;
		ui.spinBox->show();
		ui.horizontalSlider->show();
		ui.pushButton_3->show();
			// 微调框
		ui.spinBox->setMinimum(nMin);  // 最小值
		ui.spinBox->setMaximum(nMax);  // 最大值
		ui.spinBox->setSingleStep(nSingleStep);  // 步长
			// 滑动条
		ui.horizontalSlider->setOrientation(Qt::Horizontal);  // 水平方向
		ui.horizontalSlider->setMinimum(nMin);  // 最小值
		ui.horizontalSlider->setMaximum(nMax);  // 最大值
		ui.horizontalSlider->setSingleStep(nSingleStep);  // 步长
	}
}
void Moravecui::huofu()
{
	QgsMapLayer* a = NULL;
	foreach(QgsMapLayer * index, m_canvas->layers())
	{
		if (index->name() == ui.comboBox->currentText())
		{
			a = index;
		}
	}
	QgsRasterLayer* raster = qobject_cast<QgsRasterLayer*> (a);
	QgsRasterDataProvider* data = raster->dataProvider();
	int col = data->xSize();//列数
	int raw = data->ySize();//行数
	QgsRasterBlock* block1 = new QgsRasterBlock(Qgis::DataType::UInt32, col, raw);
	vector<pair<double, double>> kb;
	for (int i = 0; i < degrees; i++)
	{
		for (int y = col; y < max_r; y++)
		{
			int u = ui.spinBox->value();
			if (line_cnt1(i, y) > ui.spinBox->value())
			{
				QgsPoint pt1, pt2;
				double a = cos_[i], b = sin_[i];
				double x0 = a * (y - col), y0 = b * (y - col);
				pt1.setX((x0 + max_r * (-b)));
				pt1.setY((y0 + max_r * (a)));
				pt2.setX((x0 - max_r * (-b)));
				pt2.setY((y0 - max_r * (a)));
				if (pt2.x() - pt1.x() != 0)
				{
					double k = (pt2.y() - pt1.y()) / (pt2.x() - pt1.x());
					double d = pt2.y() - pt2.x()*k;
					kb.push_back(pair<double, double>(k, d));
				}
			}
		}
	}
	for (int i = 0; i < col; i++)
	{
		for (int y = 0; y < raw; y++)
		{
			block1->setValue(i, y, 255);
		}
	}
	for (int i = 0; i < col; i++)
	{
		for (int y = 0; y < raw; y++)
		{
			if (block1->value(i, y) != 15)
			{
				for (int u = 0; u < kb.size(); u++)
				{
					double k = kb[u].first;
					double b = kb[u].second;
					int iu = y - k * i - b;
					if (iu <= 1&&iu>=-1 )
					{
						block1->setValue(i, y, 15);
					}
				}
			}
		}
	}
	QgsRectangle extent_result = data->extent();
	int nCols = raster->width();
	int nRows = raster->height();
	double tform[] =
	{
		extent_result.xMinimum(),extent_result.width() / nCols,0.0,
		extent_result.yMaximum(),0.0,-extent_result.height() / nRows
	};
	QgsRasterDataProvider* dp = QgsRasterDataProvider::create(QStringLiteral("gdal"),
		ui.lineEdit_5->text(),
		QStringLiteral("GTiff"),
		1,
		Qgis::DataType::UInt32,
		data->xSize(),
		data->ySize(),
		tform,
		raster->crs()
	);
	dp->writeBlock(block1, 1);
	//	QgsRasterBlock* result1 =new QgsRasterBlock();
	delete block1;// 这两一定要释放，不然程序不关闭，图像block导入不了
	delete dp;
	emit rraster(ui.lineEdit_5->text());
}
Moravecui::~Moravecui()
{
}
