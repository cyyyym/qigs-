#pragma once
#include <stdio.h>
#include "memory.h"
#include "vector"
using namespace std;

// 类别数
/*const int numOfTypes = 2; // 类别数
const int numOfFeatures = 10;// 特征数
const int numOfTest = 289;// 测试的样本数，无需设置的*/
struct Node {
    int attribute; // 当前节点选取哪个属性来进行分裂
    double value; // 当前节点选取属性的哪个值来进行分裂
    Node* left; // 左子节点
    Node* right;  // 右子节点
    int sum; // 当前节点的样本总数
    //int num[numOfTypes]; // 当前节点每类的样本数
    int* num;
    int numOfTypes;
    int depth;
    vector<pair<int, vector<double> > > samples; //<label, features>
    Node(int num);

    // 拷贝构造函数
    Node(const Node& other);

};
