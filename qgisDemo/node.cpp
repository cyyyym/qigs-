#include "node.h"
#include "vector"
#include <QDebug>
using namespace std;



Node::Node(int num1) {
    left = NULL;
    right = NULL;
    sum = 0;
    numOfTypes = num1;
    //memset(num, 0, sizeof(num));
    num = new int[numOfTypes];
    for (int i = 0; i < numOfTypes; i++)
    {
        num[i] = 0;
    }
    attribute = 0;
    value = 0;
    depth = 0;
    //qDebug()<<"建造节点成功！"<<endl;
}

//拷贝构造函数
Node::Node(const Node& other) {
    this->attribute = other.attribute;
    this->value = other.value;
    this->left = other.left;
    this->right = other.right;
    this->sum = other.sum;
    this->numOfTypes = other.numOfTypes;
    this->samples = other.samples;
    this->depth = other.depth;
    this->num = new int[other.numOfTypes];
    for (int i = 0; i < other.numOfTypes; i++)
        num[i] = other.num[i];
    //qDebug()<<"拷贝成功！"<<endl;
}
