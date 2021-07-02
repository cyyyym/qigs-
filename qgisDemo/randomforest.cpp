#include "randomforest.h"
#include "RandomForest.h"
#include "fstream"
#include "node.h"
#include "tree.h"
#include "memory.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QMainWindow>
#include <QDebug>
#include <vector>
#include <iostream>
#include <sstream>
#include <string>
#include <QDialog>
#include <QGraphicsView>
#include <QtGui>
#include <QStandardItemModel>
#include <vector>
#include <qgspoint.h>
#include <qgsrasterdataprovider.h>
#include <qgsrasterblock.h>
#include<qgsrasterlayer.h>
#include<qstring.h>
#include <algorithm>
#include <QProgressDialog>
using namespace std;
RandomForest::RandomForest(QgsRasterDataProvider*& raster,QgsRasterLayer*& layer,string outfile, int& numband, int& types, QString& min_sample, QString& samp, QString& featurep,map<int,int>& a1)
{
    data = raster;
    rasterlayer = layer;
    output = outfile;
    numOfFeatures = numband;
    numOfTypes = types;
    min_leaf_sample=min_sample;//叶子节点最小样本数
    ip=samp;//样本比例
    jp=featurep;//特征比例
    table = a1;
    
    //memset(result, 0, sizeof(result));
    //numOfTest=89401;
  
}



//训练num棵决策树，并预测结果树个数
QString RandomForest::buildTree(int num) {
    if (all_samples.empty()) {
      //  cout << "[Error] all sample is empty" << endl;
        return false;
    }
    QgsRasterBlock* result = new QgsRasterBlock(Qgis::DataType::UInt32, data->xSize(), data->ySize());
    int xsize = data->xSize();
    int ysize = data->ySize();
    QTextCodec* codec = QTextCodec::codecForName("GBK");//修改这两行
    emit setprogressmessage1(codec->toUnicode("正在构建随机森林......."));

   // progressDlg1->show();
    QString text1 = codec->toUnicode("构建第");
    QString text2 = codec->toUnicode("棵树");

    for (int i = 0; i < num; i++) {
        QString int1 = QString::number(i+1);
        QString text3 = text1 + int1 + text2;
        emit setprogressvalue1(20+30*i/num);
        emit setprogressmessage1(text3);
        QCoreApplication::processEvents();
        //cout << i + 1 << " ";
        Tree *tree=new Tree(numOfFeatures, numOfTypes, min_leaf_sample, ip, jp);
        if (tree->readSample(this->all_samples))
        {
            tree->buildTree();
            QCoreApplication::processEvents();
        }
        forest.push_back(tree);
        QCoreApplication::processEvents();
    }
    int* percent = new int[numOfTypes];
    for (int i = 0; i < numOfTypes; i++)
    {
        percent[i] = 0;
    }
    emit setprogressmessage1(codec->toUnicode("随机森林分类中........"));
    QCoreApplication::processEvents();
    for (int x = 0; x < xsize; x++)
    {
        emit setprogressvalue1(50+50*x/xsize);
        QCoreApplication::processEvents();
        for (int y = 0; y < ysize; y++)
        {
            QgsPoint point(x, y);
            QgsPoint point1 = data->transformCoordinates(point, QgsRasterDataProvider::TransformType::TransformImageToLayer);
            QgsPointXY pointxy(point1);
            if (data->sample(pointxy, 1) == 0)
            {
                result->setValue(y, x, 255);
            }
            else
            {
                vector<double> sample;
                for (int i = 1; i <= data->bandCount(); i++)
                {
                    sample.push_back(data->sample(pointxy, i));
                }
                for (int i = 0; i < num; i++)
                {
                    int type = forest[i]->predict(sample);
                    percent[type-1] += 1;
                }
                int num1 = max_element(percent, percent + numOfTypes) - percent;
                //需要一个对照类别表
                result->setValue(y, x, table.find(num1+1)->second);
                for (int i = 0; i < numOfTypes; i++)
                {
                    percent[i] = 0;
                }
            }
        }
    }
    emit setprogressmessage1(codec->toUnicode("随机森林分类结束"));
    QCoreApplication::processEvents();
    QgsRectangle extent_result = rasterlayer->extent();
    int nCols = rasterlayer->width();
    int nRows = rasterlayer->height();
    qDebug() << nCols << endl << nRows << endl;
    double tform[] =
    {
        extent_result.xMinimum(),extent_result.width() / nCols,0.0,
        extent_result.yMaximum(),0.0,-extent_result.height() / nRows
    };
    QgsRasterDataProvider* dp = QgsRasterDataProvider::create(QStringLiteral("gdal"),
        QString::fromStdString(output),
        QStringLiteral("GTiff"),
        1,
        Qgis::DataType::UInt32,
        data->xSize(),
        data->ySize(),
        tform,
        data->crs()
    );
    dp->writeBlock(result, 1);
    delete result;// 这两一定要释放，不然程序不关闭，图像block导入不了
    delete dp;
    QString a = QString::fromStdString(output);
  //  emit resultfile(a);
    /*for (int i = 0; i < trees.size(); i++)
    {
        delete trees[i];
    }*/
    return a;
}


//将预测结果归总，并写入到文件中
void RandomForest::predict(const QString file) 
{
    
}




bool RandomForest::readTrainSample(string file) {
   /* //ifstream train("C:\\Users\\85726\\Documents\\RandomForest2\\train.csv");
    ifstream train(file);
    if (!train.is_open()) {
        cout << "[Error] : Failed to open file " << file << endl;
        return false;
    }
    cout << "reading sample ..." << endl;

    string s;
    getline(train, s); //过滤掉第一行标签

    double value;
    char dot;
    int id;
    int label;
    while (true) {
        train >> id;
        if (train.eof())
            break;
        //cout<<id;
        vector<double> features;
        for (int i = 0; i < numOfFeatures; i++) {
            train >> dot;    //逗号
            train >> value;
            features.push_back(value);
        }
        train >> dot;
        train >> label;
        all_samples.push_back(make_pair(label, features));
        if (train.peek() == '\n')
            train.get();
    }
    cout << all_samples.size();
    train.close();
    cout << "reading sample successful" << endl;
    return true;*/
    return true;
}


bool RandomForest::readTestSample(string file) {
   /* int n = 0;
    ifstream test(file);
    if (!test.is_open()) {
        cout << "[Error] : Failed to open file " << file << endl;
        return false;
    }
    cout << "reading test sample ..." << endl;
    test_samples.clear();
    string s;
    getline(test, s); //过滤掉第一行标签


    double value;
    char dot;
    int id;

    while (true) {
        test >> id;
        if (test.eof())
            break;

        vector<double> features;
        for (int i = 0; i < numOfFeatures; i++) {
            test >> dot;
            test >> value;
            features.push_back(value);
        }

        test_samples.push_back(make_pair(id, features));

        if (test.peek() == '\n')
            test.get();
        n++;
    }
    qDebug() << n;
    test.close();
    cout << "reading test sample successful" << endl;*/
    return true;
}

RandomForest::~RandomForest() {
    for (int i = 0; i < forest.size(); i++) {
        if (forest[i] != NULL)
            delete forest[i];
    }
}

bool RandomForest::readImgTrain(const vector<double>& features, int& label)
{
    all_samples.push_back(pair<int,vector<double>>(label, features));
    return true;
}

bool RandomForest::readImgTest(const vector<double>& features, int& label)
{
    //test_samples.push_back(make_pair(label, features));
    return true;
}






