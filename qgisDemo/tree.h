#pragma once
#include "Node.h"
#include "string"
#include"qstring.h"
using namespace std;


class Tree {
private:
    Node* root;
    int numOfFeatures;//������
    int numOfTypes;//�����
    QString min_leaf_sample;//Ҷ�ӽڵ���С������
    QString ip;//��������
    QString jp;//��������
    

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

