#include "pipei.h"
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
#include<map>
using namespace std;
using namespace Eigen;
pipei::pipei(QgsMapCanvas*& canvas,QWidget *parent)
	: QWidget(parent)
{
	m_canvas = canvas;
	
	QStringList header;
	//窗口大小
	wide = high = 13;
	//
	ui.setupUi(this);
freshui1();
	rowtable = 0;
	header << "X" << "Y";
	ui.tableWidget->setColumnCount(2);   //设置列数为4
	ui.tableWidget->setEnabled(true);
	ui.tableWidget->setHorizontalHeaderLabels(header);
	ui.tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows); //整行选中的方式
	//ui.tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers); //禁止编辑
	//ui.tableWidget->horizontalHeader()->setStretchLastSection(true); //行头自适应表格
}
bool cmp_value(const pair<pair<int, int>, double> left, const pair<pair<int, int>, double> right)
{
	return left.second < right.second;
}
void pipei::freshui(QgsMapCanvas*& can)
{
	m_canvas = can;
	freshui1();
}
void pipei::freshui1()
{
	ui.comboBox->clear();
	ui.comboBox_2->clear();
	foreach(QgsMapLayer * index, m_canvas->layers())
	{
		if (index->type() == QgsMapLayerType::RasterLayer)
		{
			ui.comboBox->addItem(index->name());
			ui.comboBox_2->addItem(index->name());
		}
	}
}
void pipei::addxy()
{
	ui.tableWidget->insertRow(rowtable);
	ui.tableWidget->setItem(rowtable, 0, new QTableWidgetItem(ui.lineEdit->text()));
	ui.tableWidget->setItem(rowtable, 1, new QTableWidgetItem(ui.lineEdit_2->text()));
	rowtable++;
	XY.push_back(pair<int, int>(ui.lineEdit->text().toInt(), ui.lineEdit_2->text().toInt()));
	//ui.lineEdit->clear();
	//ui.lineEdit_2->clear();
}
void pipei::deletexy()
{
	QList<QTableWidgetSelectionRange> ranges = ui.tableWidget->selectedRanges();
	int count = ranges.count();
	for (int i1 = count - 1; i1 >= 0; i1--)
	{
		for (int j = ranges[i1].bottomRow(); j >= ranges[i1].topRow(); j--)
		{
			ui.tableWidget->removeRow(j);
			XY.erase(XY.begin() + j);
			rowtable--;
		}
	}
}
void pipei::pipeijisuan()
{
	int namee = ui.lineEdit_3->text().length();
	QString name1 = ui.lineEdit_3->text().left(namee - 4) + "pipei.tif";
	QString txt = ui.lineEdit_3->text().left(namee - 4) + ".txt";
	QFile make_each(txt);//在训练数据中写入数据集
	if (!make_each.open(QIODevice::WriteOnly | QIODevice::Append)) {
		QMessageBox::information(NULL, "Information", QString::fromLocal8Bit("文件打开失败"), QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
		//return false;
	}
	QTextStream write_file(&make_each);
	QgsRasterLayer* left;
	QgsRasterLayer* right;
	foreach(QgsMapLayer * index, m_canvas->layers())
	{
		if (index->name() == ui.comboBox->currentText())
		{
			left = qobject_cast<QgsRasterLayer*>(index);
		}
		if (index->name() == ui.comboBox_2->currentText())
		{
			right = qobject_cast<QgsRasterLayer*>(index);
		}
	}
	QgsRasterDataProvider* leftdata = left->dataProvider();
	int leftxsize = leftdata->xSize();
	int leftysize = leftdata->ySize();
	QgsRasterBlock* blockleft = leftdata->block(1, leftdata->extent(), leftxsize, leftxsize);
	QgsRasterDataProvider* rightData = right->dataProvider();
	int rightxsize = rightData->xSize();
	int rightysize = rightData->ySize();
	QgsRasterBlock* blockright = rightData->block(1, rightData->extent(), rightxsize, rightysize);
	vector<pair<int, int>>res;
	write_file <<"; ENVI Image to Image GCP File" << endl;
	write_file << "; base file : "<<ui.comboBox->currentText() << endl;
	write_file << "; warp file :" << ui.comboBox_2->currentText() << endl;
	write_file << "; Base Image(x, y), Warp Image(x, y)" << endl;
	write_file << ";" << endl;
	for (int i = 0; i < XY.size(); i++)
	{
		pair<pair<int, int>, double> a1 = pipei1(XY[i].first, XY[i].second, blockright, blockleft);
		pair<pair<int, int>, double>b1 = pipei1(a1.first.first, a1.first.second, blockleft, blockright);
		if (XY[i].first == b1.first.first && XY[i].second == b1.first.second)
		{
			res.push_back(a1.first);
			write_file << "        " << XY[i].first;
			write_file << "        " << XY[i].second;
			write_file << "        " << a1.first.first;
			write_file << "        " << a1.first.second<<endl;

		}
	}
	QgsRasterBlock* result1 = new QgsRasterBlock(Qgis::DataType::UInt32, rightxsize, rightysize);
	for (int x = 0; x < rightxsize; x++)
	{
		for (int y = 0; y < rightysize; y++)
		{
			result1->setValue(y, x, 255);
		}
	}
	for (int i = 0; i < res.size(); i++)
	{
		result1->setValue(res[i].second, res[i].first, 0);
	}
	QgsRectangle extent_result = rightData->extent();
	int nCols = right->width();
	int nRows = right->height();
	double tform[] =
	{
		extent_result.xMinimum(),extent_result.width() / nCols,0.0,
		extent_result.yMaximum(),0.0,-extent_result.height() / nRows
	};

	QgsRasterDataProvider* dp = QgsRasterDataProvider::create(QStringLiteral("gdal"),
		name1,
		QStringLiteral("GTiff"),
		1,
		Qgis::DataType::UInt32,
		rightData->xSize(),
		rightData->ySize(),
		tform,
		rightData->crs()
	);
	dp->writeBlock(result1, 1);
	//	QgsRasterBlock* result1 =new QgsRasterBlock();
		//result1 = result;
	delete result1;// 这两一定要释放，不然程序不关闭，图像block导入不了
	delete dp;
	emit raste(name1);
}
pair<pair<int, int>, double> pipei::pipei1(int x, int y, QgsRasterBlock* right, QgsRasterBlock* left)
{
	map<pair<int, int>, double> xiangguan;
	if (y - wide / 2 >= 0 && y + wide / 2 < left->height() && x - wide / 2 >= 0 && x + wide / 2 < left->width())
	{
		ArrayXXd dataleft(wide, wide);
		dataleft.setZero();
		for (int x1 = 0; x1 < wide - 1; x1++)
		{
			for (int y1 = 0; y1 < wide - 1; y1++)
			{
				double aa = left->value(y - wide / 2 + x1, x - wide / 2 + y1);
				//dataright(x1, y1) = right->value(y - 1 + x1, ix - 1 + y1);
				dataleft(y1, x1) = left->value(y - wide / 2 + x1, x - wide / 2 + y1);
			}
		}
		int l = right->width();

		for (int ix = wide / 2; ix < l - wide / 2; ix++)
		{
			ArrayXXd dataright(wide, wide);
			dataright.setZero();
			//Array33d dataleft;
			for (int x1 = 0; x1 < wide - 1; x1++)
			{
				for (int y1 = 0; y1 < wide - 1; y1++)
				{
					dataright(y1, x1) = right->value(y - wide / 2 + x1, ix - wide / 2 + y1);
					///dataleft(x1, y1) = left->value(y - 1 - x1, x - 1 + y1);
				}
			}
			double legtave = dataleft.mean();
			double rightave = dataright.mean();
			ArrayXXd result = (dataleft - legtave) * (dataright - rightave);
			double resultall = result.sum();
			ArrayXXd result1 = (dataleft - legtave) * (dataleft - legtave);
			double result1all = result1.sum();
			ArrayXXd result2 = (dataright - rightave) * (dataright - rightave);
			double result2all = result2.sum();
			double xia = sqrt(result1all * result2all);
			double xiang = resultall / xia;
			pair<int, int>a(ix, y);
			xiangguan.insert(pair<pair<int, int>, double>(a, xiang));
		}
		
		auto i = max_element(xiangguan.begin(), xiangguan.end(), cmp_value);
		if (i->second > 0.95)
		{
			return pair<pair<int, int>, double>(i->first, i->second);
		}
		else
		{
		}
	}
}
void pipei::frompicture()
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
	QgsRasterDataProvider* leftdata = rasterLayer1->dataProvider();
	int leftxsize = leftdata->xSize();
	int leftysize = leftdata->ySize();
	QgsRasterBlock* blockleft = leftdata->block(1, leftdata->extent(), leftxsize, leftxsize);
	for (int x = 0; x < leftxsize; x++)
	{
		for (int y = 0; y < leftysize; y++)
		{
			if (x > wide / 2 && y > wide / 2 && x + wide / 2 < leftxsize && y + wide / 2 < leftysize)
			{
				if (blockleft->value(y, x) == 0)
				{
					ui.tableWidget->insertRow(rowtable);
					ui.tableWidget->setItem(rowtable, 0, new QTableWidgetItem(QString::number(x)));
					ui.tableWidget->setItem(rowtable, 1, new QTableWidgetItem(QString::number(y)));
					rowtable++;
					XY.push_back(pair<int, int>(x, y));
				}
			}
		}
	}
	delete rasterLayer1;
}
void pipei::save()
{
	QString fileName = QFileDialog::getSaveFileName(0, tr("save"), "", tr(" (*.tif)"));
	if (fileName.isNull())
	{
		QMessageBox::information(NULL, "Information", QString::fromLocal8Bit("文件创建失败"), QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
		return;
	}
	else
	{
		ui.lineEdit_3->setText(fileName);
	}
}
pipei::~pipei()
{
}
