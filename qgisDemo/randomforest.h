#pragma once


#include "iostream"
#include "vector"
#include "string"
#include "Node.h"
#include "map"
#include <QString>
#include <qgsrasterdataprovider.h>
#include <qgsrasterblock.h>
#include<qgsrasterlayer.h>
#include<map>
using namespace std;
class Tree;

class RandomForest :public QObject
{
    Q_OBJECT
private:
    vector<pair<int, vector<double> > > all_samples; //<label ,features> ,ѵ������
  //  vector<pair<int, vector<double> > > test_samples; //<id, features>��Ԥ������
    vector<Tree*> forest;
    QgsRasterDataProvider* data;
    QgsRasterLayer* rasterlayer;
    string output;
    int numOfFeatures;//������
    int numOfTypes;//�����
    QString min_leaf_sample;//Ҷ�ӽڵ���С������
    QString ip;//��������
    QString jp;//��������
    //int result[numOfTest][numOfTypes + 1];
  //  int** result;
   // int* result2;
    map<int, int> table;
signals:
    void clearmessage1();
    void setprogressvalue1(int);
    void setprogressmessage1(QString&);
public:
    bool readTrainSample(string file);
    //bool readTrainSample(QString fileName);
    QString buildTree(int num);
    ~RandomForest();
    bool readTestSample(string test);
    void predict(const QString file);
    RandomForest(QgsRasterDataProvider*&,QgsRasterLayer*&,string output, int& numband, int& types, QString& min_sample, QString& samp, QString& featurep,map<int,int>& a);
    QString build();
    bool readImgTrain(const vector<double>& file, int& label);
    bool readImgTest(const vector<double>& features, int& label);
    //bool readTrainSample();
    //bool readTestSample();

};
