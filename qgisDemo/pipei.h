#pragma once

#include <QWidget>
#include "ui_pipei.h"
#include"qgsmapcanvas.h"
#include <Eigen/Core>
#include <Eigen\Dense> 
#include<vector>
#include"qgsrasterlayer.h"
#include<map>
using namespace std;

class pipei : public QWidget
{
	Q_OBJECT

public:
	pipei(QgsMapCanvas*& canvas, QWidget* parent = Q_NULLPTR);
	~pipei();
	void freshui(QgsMapCanvas*& can);
	void freshui1();
	pair<pair<int, int>, double> pipei1(int x, int y, QgsRasterBlock* right, QgsRasterBlock* left);
	//bool cmp_value(const pair<pair<int, int>, double> left, const pair<pair<int, int>, double> right);
signals:
	void raste(QString);
public slots:
	void addxy();
	void deletexy();
	void pipeijisuan();
	void frompicture();
	void save();
private:
	Ui::pipei ui;
	QgsMapCanvas* m_canvas = nullptr;
	int rowtable;
	vector<pair<int, int>> XY;
	int wide;
	int high;

};
