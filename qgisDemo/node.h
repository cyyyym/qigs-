#pragma once
#include <stdio.h>
#include "memory.h"
#include "vector"
using namespace std;

// �����
/*const int numOfTypes = 2; // �����
const int numOfFeatures = 10;// ������
const int numOfTest = 289;// ���Ե����������������õ�*/
struct Node {
    int attribute; // ��ǰ�ڵ�ѡȡ�ĸ����������з���
    double value; // ��ǰ�ڵ�ѡȡ���Ե��ĸ�ֵ�����з���
    Node* left; // ���ӽڵ�
    Node* right;  // ���ӽڵ�
    int sum; // ��ǰ�ڵ����������
    //int num[numOfTypes]; // ��ǰ�ڵ�ÿ���������
    int* num;
    int numOfTypes;
    int depth;
    vector<pair<int, vector<double> > > samples; //<label, features>
    Node(int num);

    // �������캯��
    Node(const Node& other);

};
