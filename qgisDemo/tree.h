#pragma once
#include "Node.h"
#include "string"
#include"qstring.h"
using namespace std;


class Tree {
private:
    Node* root;
    int numOfFeatures;//波段书
    int numOfTypes;//类别数
    QString min_leaf_sample;//叶子节点最小样本数
    QString ip;//样本比例
    QString jp;//特征比例
    

public:
    Tree(int& numband, int& types, QString& min_sample, QString& samp, QString& featurep)
    {
            root = new Node(types);
            numOfFeatures = numband;
            numOfTypes = types;
            min_leaf_sample = min_sample;
            ip = samp;
            jp = featurep;
    }
    bool readSample(vector<pair<int, vector<double> > > all_samples);
    bool buildTree();
    pair<Node*, Node*> selectBestPart(Node* current);
    vector<int> randomSelect();
    int predict(vector<double> features);
    //int getType(Node* node);
    ~Tree();
};

